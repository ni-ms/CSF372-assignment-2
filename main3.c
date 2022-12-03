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
#define ARR_SIZE 2500
#define MEM_SIZE 5120

// Program to multiply the two elements
int* arr1, * arr2;
int iVal, jVal, kVal;
char * mat1, * mat2, * mat3;
long *ans;
FILE *op;
int indexV = 0;
long savearr[MEM_SIZE][MEM_SIZE];


int maxThreads;

typedef struct thread{
    int elemS, elemE;
    int elem2S, elem2E;
}thread_args;

// Integers
void writeToFile(FILE *fp, int *arr, int size) {
    for (int i = 0; i < size; ++i) {
        fprintf(fp, "%d ", arr[i]);
    }
}

void* multiplyFun(void* args) {
    thread_args *t = (thread_args *) args;
    int elemE = t->elemE;
    int elemS = t->elemS;



    for (int i = elemS; i <= elemE; ++i) {
        int rowno = i/iVal;
        int colno = i%kVal;
        long sum = 0;
        if(rowno < iVal && colno < kVal) {
            for (int j = 0; j < jVal; ++j) {
                    int num1 = arr1[rowno * jVal + j];
                    int num2 = arr2[colno * jVal + j];
                    sum += num1 * num2;
                 }

                ans[i] = sum;
          //  printf("ans[%d] = %ld\n", i, ans[i]);

            savearr[i / jVal][i % jVal] = ans[i];
        }
    }

        pthread_exit(NULL);

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

    printf("Arra1 is: \n");
    for (int i = 0; i < iVal*jVal; ++i) {
        printf("%d ", arr1[i]);
        if(i%jVal == jVal-1){
            printf("\n");
        }
    }
    printf("Arra2 is: \n");
    for (int i = 0; i < jVal*kVal; ++i) {
        printf("%d ", arr2[i]);
        if(i%jVal == jVal-1){
            printf("\n");
        }
    }

    ans = (long *) malloc(iVal*kVal*sizeof(long ));
    printf("ival*jval is: %d\n", iVal*kVal);


    //TODO
    //FIX THIS CODE
    maxThreads = 50;
    thread_args *inp = malloc(maxThreads* sizeof (thread_args));
    int prev = 0;
    int temp = getMax(((iVal*kVal) / maxThreads) , 1);
    for (int i = 0; i < maxThreads; ++i) {
    inp[i].elemS = prev;
    inp[i].elemE = prev + temp;
    inp[i].elem2S = prev;
    inp[i].elem2E = prev + temp;
     //  printf("The start and end are: %d %d\n", inp[i].elemS, inp[i].elemE);

    prev = prev + temp + 1;

   }


    //Create threads
    pthread_t *threads = malloc(maxThreads * sizeof(pthread_t));

    for (int i = 0; i < maxThreads; ++i) {
        pthread_create(&threads[i], NULL, multiplyFun, (void*)(inp + i));
    }
    for (int i = 0; i < maxThreads; ++i) {
        pthread_join(threads[i], NULL);
    }


    FILE *outfp = fopen("out.txt", "w");
    for (int i = 0; i < iVal; ++i) {
        for (int j = 0; j < kVal; ++j) {

            fprintf(outfp, "%ld", ans[i*kVal + j]);
            if(j != kVal-1){
                fprintf(outfp, " ");
            }

        }
        fprintf(outfp, "\n");
    }


    shmdt(arr1);
    shmctl(shmid,IPC_RMID,NULL);
    shmdt(arr2);
    shmctl(shmid2,IPC_RMID,NULL);

    free(threads);
    free(inp);
    free(ans);
    fclose(outfp);

    return 0;
}

