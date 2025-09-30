#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <errno.h>

pid_t P1, P2;
pid_t current_process;
int time_quantum;
int p1_finished = 0, p2_finished = 0;


void set_timer(int seconds) {
    struct itimerval timer;
    timer.it_value.tv_sec = seconds;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("setitimer failed");
        exit(1);
    }
}


int process_exists(pid_t pid) {
    return (kill(pid, 0) == 0);
}


void switch_process(int signum) {
    printf("Timer expired, switching processes...\n");


    if (current_process == P1 && !process_exists(P1)) {
        p1_finished = 1;
        printf("Process P1 has finished\n");
    } else if (current_process == P2 && !process_exists(P2)) {
        p2_finished = 1;
        printf("Process P2 has finished\n");
    }


    if (p1_finished && p2_finished) {
        printf("Both processes finished, scheduler terminating\n");
        return;
    }


    if (current_process == P1) {
        if (!p1_finished) {
            printf("Stopping P1 (PID: %d)\n", P1);
            kill(P1, SIGSTOP);
        }

        if (!p2_finished) {
            printf("Continuing P2 (PID: %d) for %d seconds\n", P2, 2);
            kill(P2, SIGCONT);
            current_process = P2;
            time_quantum = 2;
            set_timer(time_quantum);
        }
    } else {
        if (!p2_finished) {
            printf("Stopping P2 (PID: %d)\n", P2);
            kill(P2, SIGSTOP);
        }

        if (!p1_finished) {
            printf("Continuing P1 (PID: %d) for %d second\n", P1, 1);
            kill(P1, SIGCONT);
            current_process = P1;
            time_quantum = 1;
            set_timer(time_quantum);
        }
    }
}


void child_handler(int signum) {
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (pid == P1) {
            printf("Process P1 (PID: %d) terminated\n", pid);
            p1_finished = 1;
        } else if (pid == P2) {
            printf("Process P2 (PID: %d) terminated\n", pid);
            p2_finished = 1;
        }
    }
}

int main() {
    printf("Starting Round Robin Scheduler\n");
    printf("Time Quantum: P1=1s, P2=2s\n");
    printf("================================\n");


    P1 = fork();
    if (P1 == -1) {
        perror("Failed to fork P1");
        exit(1);
    } else if (P1 == 0) {

        printf("P1 starting...\n");
        execl("./prog1_1", "prog1_1", NULL);
        perror("Failed to execute prog1_1");
        exit(1);
    }


    P2 = fork();
    if (P2 == -1) {
        perror("Failed to fork P2");
        kill(P1, SIGTERM);
        exit(1);
    } else if (P2 == 0) {

        printf("P2 starting...\n");
        execl("./prog2", "prog2", NULL);
        perror("Failed to execute prog2");
        exit(1);
    }


    printf("Scheduler: P1 (PID: %d), P2 (PID: %d)\n", P1, P2);


    sleep(1);
    kill(P2, SIGSTOP);
    printf("Starting with P1 for %d second\n", 1);

    current_process = P1;
    time_quantum = 1;


    signal(SIGALRM, switch_process);
    signal(SIGCHLD, child_handler);


    set_timer(time_quantum);


    while (!p1_finished || !p2_finished) {
        pause();


        if (p1_finished && p2_finished) {
            break;
        }


        if (p1_finished && !p2_finished) {
            printf("Only P2 remains, letting it run to completion\n");
            kill(P2, SIGCONT);
            waitpid(P2, NULL, 0);
            break;
        } else if (p2_finished && !p1_finished) {
            printf("Only P1 remains, letting it run to completion\n");
            kill(P1, SIGCONT);
            waitpid(P1, NULL, 0);
            break;
        }
    }

    printf("================================\n");
    printf("Round Robin Scheduler finished\n");


    while (wait(NULL) > 0);

    return 0;
}
