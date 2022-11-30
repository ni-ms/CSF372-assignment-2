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

#define MAX_SIZE 1000000
#define MEM_SIZE 5120
#define ARR_SIZE 1000


//Global variables
FILE *fp1, *fp2;
int iVal,jVal,kVal;
char* mat1, *mat2, *mat3;
int *arr1;
int *arr2;
//int arr1[ARR_SIZE];
//int arr2[ARR_SIZE];
int isVisited1[MAX_SIZE], isVisited2[MAX_SIZE];
int *offsetarray1, *offsetarray2;
int line1size = 0, line2size = 0;

int maxThreads;


//Time taken:
uint64_t totaltime = 0;

typedef struct files{
    int readStart1, readEnd1;

} thread_inp;


void getLineIndex(FILE *fp, int *var){
    if(fp == NULL){
        fprintf(stderr, "ERROR: File not found\n");
        exit(EXIT_FAILURE);
    }
    char c;
    for (c = getc(fp); c != EOF ; c = getc(fp)) {

        if (c == '\n') {
            (*var)++;
        }
    }
  /*  printf("Number of lines in file: %d\n", *var);*/
}

void getOffset(FILE *fp, int offsetarr[]){
    if(fp == NULL){
        fprintf(stderr, "ERROR: File not found\n");
        exit(EXIT_FAILURE);
    }
    int temp = 0;
    int var = 0;
    char c;
    fseek(fp, 0, SEEK_SET);
    for (c = getc(fp); c != EOF ; c = getc(fp)) {
        temp++;
        if (c == '\n') {
            temp++;
            offsetarr[var] = temp;
         /*   printf("Offset of line %d: %d\n", var, offsetarr[var]);*/
            var++;

        }
    }

}

int arrIndex = 0;
int arrIndex2 = 0;

//TODO
//Make code run for threads less than number of rows
//make sure data is stored in order


void* threadfun(void* args){
    thread_inp *inp = (thread_inp*) args;
    /*int* off1 = inp->off1, *off2 = inp->off2;*/
    int toreadS = inp->readStart1;
    int toreadE = inp->readEnd1;
   /* //Read from file 1
    int p = 0;
    int readStart1 = iVal / maxThreads;
    for (int i = 0; i < line1size; ++i) {
        if(isVisited1[i] == 0){
            for (int num = 0; num <  readStart1; ++num) {
                isVisited1[num] = 1;
            }
        }
    }*/

    //line to store the values for file 1
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    //line to store the values for file 2
    char *line2 = NULL;
    size_t len2 = 0;
    ssize_t read2;


//for loop to read the file
    for(int i = toreadS; i <= toreadE; i++){
        printf("Thread %ld: Reading line %d\n", (long)pthread_self(), i);
      /*  fseek(fp1, offsetarray1[i], SEEK_SET);
        read = getline(&line, &len, fp1);
        if(read == -1){
            fprintf(stderr, "ERROR: File not found\n");
            exit(EXIT_FAILURE);
        }
        //printf("Line: %s\n", line);
        char *token = strtok(line, " ");
        while(token != NULL){
            //printf("Token: %s\n", token);
            arr1[arrIndex] = atoi(token);
            arrIndex++;
            token = strtok(NULL, " ");
        }*/

        if(isVisited1[i] == 0 && i < line1size){
            //FOR FILE ONE
            fseek(fp1, offsetarray1[i] - i - 1, SEEK_SET);
            uint64_t elapsed;
            struct timespec endT, startT;
            int ret = clock_gettime(CLOCK_MONOTONIC, &startT);
            if(ret == -1){
                fprintf(stderr, "ERROR: Clock gettime failed\n");
                exit(EXIT_FAILURE);
            }
            clock_gettime(CLOCK_MONOTONIC, &startT);
            read = getline(&line, &len, fp1);
            clock_gettime(CLOCK_MONOTONIC, &endT);
            elapsed = (endT.tv_sec - startT.tv_sec) * 1000000000 + (endT.tv_nsec - startT.tv_nsec);
/*        printf("Time taken to read line: %lu\n", elapsed);
        printf("threadno: %d: %s", (int)toreadS, line);*/
            totaltime += elapsed;
            //Process line and store in array
            char *token = strtok(line, " ");
            isVisited1[i] = 1;
            int temp = 0;
            while(token != NULL){
                arr1[i*jVal+ temp] = atoi(token);
                /*printf("threadno: %d: %d\n", (int)toreadS, arr1[i*jVal+ temp]);*/
                token = strtok(NULL, " ");
                temp++;
            }
          /*  for (int i = 0; i < line1size; ++i) {
                printf("IsVisited1: %d\n", isVisited1[i]);
            }*/

            //reset line
            line = NULL;

        }

        if(isVisited2[i] == 0 && i < line1size){
            //FOR FILE TWO
            fseek(fp2, offsetarray2[i] - i - 1, SEEK_SET);
            uint64_t elapsed2;
            struct timespec endT2, startT2;
            int ret2 = clock_gettime(CLOCK_MONOTONIC, &startT2);
            if(ret2 == -1){
                fprintf(stderr, "ERROR: Clock gettime failed\n");
                exit(EXIT_FAILURE);
            }
            clock_gettime(CLOCK_MONOTONIC, &startT2);
            read = getline(&line2, &len2, fp2);
            clock_gettime(CLOCK_MONOTONIC, &endT2);
            elapsed2 = (endT2.tv_sec - startT2.tv_sec) * 1000000000 + (endT2.tv_nsec - startT2.tv_nsec);
            /*printf("Time taken to read line: %lu\n", elapsed2);*/
            totaltime += elapsed2;


            int temp = 0;
            isVisited2[i] = 1;
            char *token2 = strtok(line2, " ");
            while(token2 != NULL){
                arr2[i*jVal + temp] = atoi(token2);
                /* printf("arr2[%d]: %d\n", i*jVal + temp, arr2[i*jVal + temp]);*/
                token2 = strtok(NULL, " ");
                temp++;
            }


            //Reset line2
            line2 = NULL;
        }



    }


    /*free(token);
    free(line);*/

/*
   //Read from file 1
    for (int iVal = 0; iVal < line1size; ++iVal) {
        if(isVisited1[iVal] == 0){
            isVisited1[iVal] = 1;
            int temp = iVal;
            char values[1000];
            while(temp != 0){
                off1 += offsetarray1[temp-1];
                temp--;
            }
            fread(values, sizeof(char), off1, fp1);


            break;
        }
    }

    //read from file 2
    *//*for (int kVal = 0; kVal < line2size; ++kVal) {
        if(isVisited2[kVal] == 0){

            isVisited2[kVal] = 1;
            int temp = kVal;
            char values[1000];
            while(temp != 0){
                fseek(fp1, off1, SEEK_SET);
                fread(values, sizeof(char), 10, fp1);
                temp--;
            }
            printf("Thread %d: %s\n", (int)pthread_self(), values);

            break;
        }
    }*/

    pthread_exit(NULL);


}

//MAIN FUNCTION

int main(int argc, char * argv[]){
    //Fork and exec
    if(argc != 7){
        fprintf(stderr,"ERROR: Invalid Arguments\n" );
        fprintf(stderr,"USAGE: ./group12_assignment2.out iVal jVal kVal in1.txt in2.txt out.txt\n" );
        return EXIT_FAILURE;
    }
    iVal = atoi(argv[1]);
    jVal = atoi(argv[2]);
    kVal = atoi(argv[3]);
    mat1 = argv[4];
    mat2 = argv[5];
    mat3 = argv[6];


    //Allocate memory for array1 and array2

    //arr1 = malloc((iVal * jVal) * sizeof (int));
    //arr2 = malloc((jVal * kVal) * sizeof (int));


    //create file pointers
    fp1 = fopen(mat1, "r");
    fp2 = fopen(mat2, "r");

    //TODO
    //OPTIMIZATION: Use memory mapped files
   /* int fd = open(mat1, O_RDONLY);
    struct stat sb;
    if(fstat(fd, &sb) == -1){
        fprintf(stderr, "ERROR: fstat failed\n");
        exit(EXIT_FAILURE);
    }
    char *file_in_mem = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

*/

    //transpose second matrix
    getLineIndex(fp1, &line1size);
    getLineIndex(fp2, &line2size);


    //Allocate memory for visited
   /* isVisited1 = malloc(line1size * sizeof(int));
    isVisited2 = malloc(line2size * sizeof(int));*/
    offsetarray1 = malloc(line1size * sizeof(int));
    offsetarray2 = malloc(line2size * sizeof(int));

    for (int p = 0; p < line1size; ++p) {
       /* isVisited1[p] = 0;*/
        offsetarray1[p] = 0;
        offsetarray2[p] = 0;
    }
   /* for (int p = 0; p < line2size; ++p) {
        isVisited2[p] = 0;
        offsetarray2[p] = 0;
    }*/


    getOffset(fp1, offsetarray1);
    getOffset(fp2, offsetarray2);

    maxThreads = 22;
    thread_inp *inp = malloc(maxThreads * sizeof(thread_inp));


    int prev = 0;
    int temp = getMax((line1size / maxThreads) , 1);
    for (int i = 0; i < maxThreads; ++i) {
        inp[i].readStart1 = prev;
        inp[i].readEnd1 = prev + temp;
        prev = prev + temp +1;

    }





    //create shared memory for array 1
    key_t key = ftok("./shm/shmfile1.txt",65);
    int shmid = shmget(key,MEM_SIZE,0666|IPC_CREAT);
    arr1 = (int*) shmat(shmid,(void*)0,0);

    //create shared memory for array 2
    key_t key1 = ftok("./shm/shmfile2.txt", 65);
    /*printf("keys: %d %d\n", key, key1);*/
    int shmid1 = shmget(key1, MEM_SIZE, 0666|IPC_CREAT);
    arr2 = (int*) shmat(shmid1, (void*)0, 0);


    pthread_t thread_create[maxThreads];
    for (int i = 0; i < maxThreads; ++i) {
        pthread_create(&thread_create[i], NULL, threadfun, (void*)(inp + i));
    }
    for (int i = 0; i < maxThreads; ++i) {
        pthread_join(thread_create[i], NULL);
    }


    printf("Total time taken for %d threads: %lu\n",maxThreads ,totaltime);

    printf("Write Data : ");
    /*for (int i = 0; i < ARR_SIZE; ++i) {
        shmseg[i] = arr1[i];
    }*/
    printf("Data written in memory:\n");
    for (int i = 0; i < ARR_SIZE; ++i) {
        /*printf("%d ", shmseg[i]);*/
        printf("%d ", arr1[i]);
    }
    printf("\n");
    for (int i = 0; i < ARR_SIZE; ++i) {
        printf("%d ", arr2[i]);
    }
    shmdt(arr1);
    shmdt(arr2);


/*    printf("Value in array:");
    for(int i = 0; i < iVal; i++){
        for(int j = 0; j < jVal; j++){
            printf("%d ", arr1[i * jVal + j]);
        }
        printf("\n");
    }*/

    //Close files


    fclose(fp1);
    fclose(fp2);
//free memory
    //free(arr1);

 /*   free(isVisited1);
    free(isVisited2);*/
    free(offsetarray1);
    free(offsetarray2);
    free(inp);
    return 0;


   /* //NUMBER OF THREADS
    int n = 10;
    int rows = 25, columns = 25;
    int *acc = malloc((rows * columns) * sizeof(int));
    //Thread id is stored
    pthread_t *tid = (pthread_t*)malloc(n * sizeof(pthread_t));
    threadInp *obj = malloc(n * sizeof (threadInp));
    for (int iVal = 0; iVal < n; ++iVal) {
        //for rows, number of cols
        obj[iVal].buff = malloc(columns* sizeof (int));

    }

    int nextStart = 0;
    for (int iVal = 0; iVal < n; ++iVal) {
        obj[iVal].length = 25;
        obj[iVal].start = nextStart;

        int temp;
        if(rows%n == 0)
            temp = rows/n;
        else
            temp = rows/n + 1;

        nextStart += temp;

        if(rows%n == 0)
            obj[iVal].count = temp;
        else{
            if(iVal != n - 1)
                obj[iVal].count = temp;
            else
                obj[iVal].count = rows%n;
        }


        obj[iVal].file_name = "in1.txt";
        pthread_create(&tid[iVal], NULL, threadfun,(void*)obj );
        pthread_join(&tid[iVal], NULL);
    }

    for (int iVal = 0; iVal < rows; iVal++) {

        for (int jVal = 0; jVal < columns; jVal++)

            //printf("%d ", [iVal * columns + jVal]);

        printf("\n");

    }


    */
}

