//IPC and Multiply
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define getMax(x,y) (((x) >= (y)) ? (x) : (y))

#define MAXSIZE 1000
#define ARR_SIZE 1000

// Program to multiply the two elements
int* arr1, * arr2;
int iVal, jVal, kVal;
char * mat1, * mat2, * mat3;
int ans[MAXSIZE][MAXSIZE];
FILE *op;


int maxThreads;

typedef struct thread{
    int rowS, columnS;
    int rowE, columnE;
}thread_args;

// Integers


void* multiplyFun(void* args){
    //get row and column
    //multiply

}
int main(int argc, char * argv[]){
   if(argc != 7){
        fprintf(stderr,"Usage: ./a.out <i> <j> <k> in1.txt in2.txt out.txt");
        return EXIT_FAILURE;
   }
    iVal = atoi(argv[1]);
    jVal = atoi(argv[2]);
    kVal = atoi(argv[3]);
    mat1 = argv[4];
    mat2 = argv[5];
    mat3 = argv[6];


    key_t key = ftok("./cmake-build-debug/shm/shmfile1.txt", 65);
    int shmid = shmget(key,ARR_SIZE,0666|IPC_CREAT);
    arr1 = (int*) shmat(shmid,(void*)0,0);

    key_t key2 = ftok("./cmake-build-debug/shm/shmfile2.txt", 65);
    int shmid2 = shmget(key2,ARR_SIZE,0666|IPC_CREAT);
    arr2 = (int*) shmat(shmid2,(void*)0,0);


    maxThreads = 50;

    thread_args *inp = malloc(maxThreads* sizeof (thread_args));
    int prev = 0;
    int temp = getMax(((iVal*kVal) / maxThreads) , 1);
    for (int i = 0; i < maxThreads; ++i) {
    inp[i].columnS = prev;
    inp[i].rowS = prev;
    inp[i].rowE = prev + temp;
    inp[i].columnE = prev + temp;
    prev = prev + temp + 1;
   }





    //create threads
    pthread_t * threads = malloc(sizeof(pthread_t)*maxThreads);



 /*   printf("For array1:\n");
    for (int i = 0; i < 1000; ++i) {
        printf("%d ", arr1[i]);
    }
    printf("\n\n\n");
    printf("For array2:\n");
    for (int i = 0; i < 1000; ++i) {
        printf("%d ", arr2[i]);
    }*/
    shmdt(arr1);
    shmctl(shmid,IPC_RMID,NULL);
    shmdt(arr2);
    shmctl(shmid2,IPC_RMID,NULL);
    return 0;
}

