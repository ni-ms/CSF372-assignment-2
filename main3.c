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
long **ans;
FILE *op;
int indexV = 0;


int maxThreads;

typedef struct thread{
    int rowS, columnS;
    int rowE, columnE;
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
    int rowS = t->rowS;
    int columnS = t->columnS;
    int rowE = t->rowE;
    int columnE = t->columnE;
    int i, j, k;
    /*printf("Array 1 is: \n");
    for (i = 0; i < iVal*jVal; i++) {
        printf("%d ", arr1[i]);
        if(i%jVal == jVal-1){
            printf("\n");
        }
    }*/


    //calculate product
if(rowE< iVal && columnE < jVal) {
    for (i = rowS; i < rowE; i++) {
        for (j = columnS; j < columnE; j++) {
            printf("Element being calculated is: %d %d\n", i, j);
            /*ans[i][j] += arr1[i*jVal + k] * arr2[j*jVal + k];*/
            long temp = 0;
            for (int p = 0; p < jVal; p++) {
                temp += arr1[i * jVal + p] * arr2[j * jVal + p];
                /* printf("the multiplicands are: %d %d\n", arr1[i*jVal + p], arr2[j*jVal + p]);*/
            }
            ans[indexV] = temp;
            indexV++;
            printf("The product is: %ld\n", temp);


            /*printf("\nRow values are:\n");
                for(int l = 0; l < jVal; l++){

                    printf("%d ", arr1[i*jVal + l]);

                }
            printf("\nColumn values are:\n");
            for (int m = 0; m < jVal; ++m) {

                printf("%d ",arr2[j*jVal + m] );
            }
                printf("\n");
            printf("\nproduct is: %lu", temp);*/

        }
    }
}


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
    inp[i].columnS = prev;
    inp[i].rowS = prev;
    inp[i].rowE = prev + temp;
    inp[i].columnE = prev + temp;
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

    printf("Output Matrix:");
    for (int i = 0; i < iVal; i++) {
        for (int k = 0; k < kVal; k++) {
            printf("(Index is: %d)", i*jVal + k);
            printf("%lu ", ans[i*jVal + k]);
        }
        printf("\n");
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

