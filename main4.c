
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pro1_pid = -1, pro2_pid = -1, sch_pid = -1;
    sch_pid = getpid();
    pro1_pid = fork(); // returns 0 to child and child-pid to parent.
    if (pro1_pid == 0) // if child (pro1)
    {

        execl("./temp1","", NULL);
    }

    // parent
    pro2_pid = fork();
    if (pro2_pid == 0) // if child (pro2)
    {
        execl("./temp2","", NULL);
    }

    printf("%d %d \n", pro1_pid, pro2_pid);

    int quantum_number = 3;

    //pause both processes
    kill(pro1_pid,SIGSTOP); // we are NOT killing the processes.
    kill(pro2_pid,SIGSTOP);

    kill(pro1_pid,SIGCONT);
    while (1)
    {
        printf("QUANTUM NUMBER: %d\n",quantum_number);
        if (quantum_number%2==0) //even implies pro1 should execute
        {
            kill(pro2_pid,SIGSTOP);
            kill(pro1_pid,SIGCONT);
        }
        else
        {
            kill(pro1_pid,SIGSTOP);
            kill(pro2_pid,SIGCONT);
        }
        quantum_number++;
        sleep(5);
    }
    wait(NULL);
    return 0;
}

/*
//SCHEDULING
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define P_NUM 2
#define Quant1 1
#define Quant2 2

*/
/*int sum = 0, count = 0;
float avg_wt, avg_tat;
int main(int argc, char * argv[]) {

    for (int i = 0; i < P_NUM ; ++i) {
        fork
    }

} *//*

//Argument 20 50 20 inp1.txt inp2.txt out.txt

//Global variables

pid_t p1_pid, p2_pid, pids;


int main(){
    p1_pid = -1;
    p2_pid = -1;
    pids = -1;
    pids = getpid();
    p1_pid = fork();

    if (p1_pid == 0){
        //CHILD PROCESS
        //execlp("./cmake-build-debug/main2.out", "50", "20","50","inp1.txt", "inp2.txt","out.txt", NULL);
        execlp("./temp2","",NULL);
   }
    p2_pid = fork();
    if (p2_pid == 0){
        //CHILD PROCESS
        //execlp("./cmake-build-debug/main2.out", "20", "50","20","inp1.txt", "inp2.txt","out.txt", NULL);
        execlp("./temp1","",NULL);
    }

    printf("The pids are %d %d %d", pids, p1_pid, p2_pid);

    int time_q = 1;
    kill(p1_pid, SIGSTOP);
    kill(p2_pid, SIGSTOP);

    kill(p1_pid, SIGCONT);

    while (1){
        printf("Time quantum is %d", time_q);
        if (time_q%2 == 0){
            kill(p1_pid, SIGSTOP);
            kill(p2_pid, SIGCONT);
        }
        else{
            kill(p1_pid, SIGCONT);
            kill(p2_pid, SIGSTOP);
        }
        time_q++;
      //  nanosleep((const struct timespec[]){{0, 1000000L}}, NULL);
        sleep(5);
    }
    wait(NULL);
    return 0;



}
*/
