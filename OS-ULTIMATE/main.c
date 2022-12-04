#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int iVal, jVal, kVal;
char* mat1, *mat2, *mat3;
int *arr4, *arr3, *ismul;

int main(int argc, char * argv[])
{

    if(argc != 7){
        fprintf(stderr,"ERROR: Invalid Arguments\n" );
        fprintf(stderr,"USAGE: ./main.c iVal jVal kVal in1.txt in2.txt out.txt\n" );
        return EXIT_FAILURE;
    }
    iVal = atoi(argv[1]);
    jVal = atoi(argv[2]);
    kVal = atoi(argv[3]);
    mat1 = argv[4];
    mat2 = argv[5];
    mat3 = argv[6];

    int shmid3 = shmget(1236,(iVal+1),0666|IPC_CREAT);
    if(shmid3 == -1){
        fprintf(stderr,"ERROR: Shared Memory 3 Creation Failed\n" );
        return EXIT_FAILURE;
    }
    arr3 = (int*) shmat(shmid3,(void*)0,0);

    int shmid4 = shmget(1237,(kVal+1),0666|IPC_CREAT);
    if(shmid4 == -1){
        fprintf(stderr,"ERROR: Shared Memory 4 Creation Failed\n" );
        return EXIT_FAILURE;
    }
    arr4 = (int*) shmat(shmid4,(void*)0,0);

    int shmid5 = shmget(1238,(iVal*kVal),0666|IPC_CREAT);
    if(shmid5 == -1){
        fprintf(stderr,"ERROR: Shared Memory 5 Creation Failed\n" );
        return EXIT_FAILURE;
    }
    ismul = (int*) shmat(shmid5,(void*)0,0);


    //init to 0
    for(int i = 0; i < iVal; i++){
        arr3[i] = -1;
    }
    for(int i = 0; i < kVal; i++){
        arr4[i] = -1;
    }
    for (int i = 0; i < iVal * kVal; ++i) {
        ismul[i] = -1;
    }

    int time_el = 5;
    pid_t pro1_pid = -1, pro2_pid = -1, sch_pid = -1;
    sch_pid = getpid();
    pro1_pid = fork();
    if (pro1_pid == 0){
        //child process
        execlp("./p1.out",(char*)(iVal + "0"),(char*)(jVal + "0"),(char*)(kVal + "0"),mat1,mat2,mat3, NULL);
    } else {

        return EXIT_FAILURE;
    }
    //in parent
    pro2_pid = fork();
    if (pro2_pid == 0){
        //child process
        execlp("./p2.out",(char*)(iVal + "0"),(char*)(jVal + "0"),(char*)(kVal + "0"),mat1,mat2,mat3, NULL);
    } else {

        return EXIT_FAILURE;
    }
    //in parent
    int quantum_number = 2;

    sleep(time_el);
    //pause both processes
    kill(pro1_pid,SIGSTOP); // we are NOT killing the processes.
    kill(pro2_pid,SIGSTOP);

    kill(pro1_pid,SIGCONT);
    int timethreshold = 100000;
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
        int flag = 0;
        timethreshold--;
        for(int i = 0; i < iVal * kVal; i++){
            if(ismul[i] != -1){
                flag += 1;
            }
        }
        if(flag == 0 || timethreshold == 0){
            break;
        }
        quantum_number++;
        sleep(5);

    }
    shmdt(arr3);
    shmdt(arr4);
    shmctl(shmid3,IPC_RMID,NULL);
    shmctl(shmid4,IPC_RMID,NULL);

    wait(NULL);
    return 0;
}
