#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

pid_t P1, P2;
pid_t current_process;
int time_quantum;

// Signal handler for switching processes
void switch_process(int signum) {
    if (current_process == P1) {
        kill(P1, SIGSTOP);
        kill(P2, SIGCONT);
        current_process = P2;
        time_quantum = 2;
    } else {
        kill(P2, SIGSTOP);
        kill(P1, SIGCONT);
        current_process = P1;
        time_quantum = 1;
    }
}

int main() {
    // Create P1 and P2 child processes
    P1 = fork();
    if (P1 == 0) {
        // Execute prog1.c as P1
        execlp("./prog1_1", "prog1_1", NULL);
        perror("Failed to execute prog1");
        return 1;
    }

    P2 = fork();
    if (P2 == 0) {
        // Execute prog2.c as P2
        execlp("./prog2", "prog2", NULL);
        perror("Failed to execute prog2");
        return 1;
    }

    // S code here
    current_process = P1;
    time_quantum = 1;

    // Register signal handler for SIGALRM
    signal(SIGALRM, switch_process);

    // Set the timer interval for the first time quantum
    struct itimerval timer;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = time_quantum * 1000;
    timer.it_value = timer.it_interval;
    setitimer(ITIMER_REAL, &timer, NULL);

    while (1) {
        // S will be interrupted by SIGALRM every time quantum
        pause();
    }

    // Wait for child processes to terminate
    wait(NULL);
    wait(NULL);

    return 0;
}
