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
#define MEM_SIZE 5120

// Program to multiply the two elements
int* arr1, * arr2;
int iVal, jVal, kVal;
char * mat1, * mat2, * mat3;
long *ans;
FILE *op;
int indexV = 0;


int maxThreads;

typedef struct thread{
    int elemS, elemE;
}thread_args;

// Integers
void writeToFile(FILE *fp, int *arr, int size) {
    for (int i = 0; i < size; ++i) {
        fprintf(fp, "%d ", arr[i]);
    }
}

void* multiplyFun(void* args) {
    //get row and column
    //multiply
    thread_args *t = (thread_args *) args;
    int elemE = t->elemE;
    int elemS = t->elemS;


    //TO SEE IF ARRAY IS BEING READ
  /*  FILE *fpa = fopen("z1.txt", "w");
    fprintf(fpa,"Array 1 is: \n");
    for (int i = 0; i < iVal*jVal; i++) {
        fprintf(fpa,"%d ", arr1[i]);
        if(i%jVal == jVal-1){
            fprintf(fpa,"\n");
        }
    }
    fprintf(fpa,"\n\nArray 2 is: \n");
    for (int i = 0; i < kVal*jVal; i++) {
        fprintf(fpa,"%d ", arr2[i]);
       if(i%jVal == jVal-1){
            fprintf(fpa,"\n");
        }
    }*/

  FILE *fp = fopen("z2.txt", "w");
    //ival = 20, jvl = 50, kval = 20
    for(int p = 0; p < iVal; p++){
        for (int q = 0; q < kVal - 1; ++q) {
            for(int r = 0; r < jVal; r++){
              ans[p*kVal + q] += arr1[p*jVal + r] * arr2[q*jVal + r];

            }
            fprintf(fp,"%ld ", ans[p*kVal + q]);
        }
        fprintf(fp,"\n");

    }
    fclose(fp);





        pthread_exit(NULL);
        return NULL;
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
    printf("ival, jval, kval are: %d %d %d\n", iVal, jVal, kVal);


    key_t key = ftok("./cmake-build-debug/shm/shmfile1.txt", 65);
    int shmid = shmget(key,MEM_SIZE,0666|IPC_CREAT);
    arr1 = (int*) shmat(shmid,(void*)0,0);

    key_t key2 = ftok("./cmake-build-debug/shm/shmfile2.txt", 65);
    int shmid2 = shmget(key2,MEM_SIZE,0666|IPC_CREAT);
    arr2 = (int*) shmat(shmid2,(void*)0,0);

    ans = (long *) malloc(iVal*jVal*sizeof(long ));

    maxThreads = 50;

    //TODO
    //FIX THIS CODE

    thread_args *inp = malloc(maxThreads* sizeof (thread_args));
    int prev = 0;
    int temp = getMax(((iVal*kVal) / maxThreads) , 1);
    for (int i = 0; i < maxThreads; ++i) {
    inp[i].elemS = prev;
    inp[i].elemE = prev + temp;
    prev = prev + temp + 1;
/*

        printf("Thread %d: %d %d %d %d\n", i, inp[i].rowS, inp[i].columnS, inp[i].rowE, inp[i].columnE);
*/

   }


    //Create threads
    pthread_t *threads = malloc(maxThreads * sizeof(pthread_t));

    for (int i = 0; i < maxThreads; ++i) {
        pthread_create(&threads[i], NULL, multiplyFun, &inp[i]);
        pthread_join(threads[i], NULL);
    }


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

