#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


//Global variables
FILE *fp1, *fp2;
int i,j,k;
char* mat1, *mat2, *mat3;
int *arr1, *arr2;
int *isVisited1, *isVisited2;
int *offsetarray1, *offsetarray2;
int line1 = 0, line2 = 0;
FILE *fp3;
int maxThreads;


typedef struct files{
    FILE* fp1;
    FILE* fp2;
    int*off1,*off2;
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
    printf("Number of lines in file: %d\n", *var);
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

            var++;

        }
    }


}



void* threadfun(void* args){
    thread_inp *inp = (thread_inp*) args;
    FILE *fp1 = inp->fp1;
    FILE *fp2 = inp->fp2;
    int* off1 = inp->off1, *off2 = inp->off2;


    //Read from file 1
    int p = 0;
    for (int i = 0; i < line1; ++i) {}

        fseek(fp1, off1[p] - p - 1, SEEK_SET);
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        read = getline(&line, &len, fp1);
        printf("Retrieved line of length %zu :\n", read);
        printf("%s", line);
        free(line);




/*
   //Read from file 1
    for (int i = 0; i < line1; ++i) {
        if(isVisited1[i] == 0){
            isVisited1[i] = 1;
            int temp = i;
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
    *//*for (int k = 0; k < line2; ++k) {
        if(isVisited2[k] == 0){

            isVisited2[k] = 1;
            int temp = k;
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

}

int main(int argc, char * argv[]){
    //Fork and exec
    if(argc != 7){
        fprintf(stderr,"ERROR: Invalid Arguments\n" );
        fprintf(stderr,"USAGE: ./group12_assignment2.out i j k in1.txt in2.txt out.txt\n" );
        return EXIT_FAILURE;
    }
    i = atoi(argv[1]);
    j = atoi(argv[2]);
    k = atoi(argv[3]);
    mat1 = argv[4];
    mat2 = argv[5];
    mat3 = argv[6];

    //Allocate memory for array1 and array2

    arr1 = malloc((i*j)*sizeof (int));
    arr2 = malloc((j*k)*sizeof (int));


    //create file pointers
    fp1 = fopen(mat1, "r");
    fp2 = fopen(mat2, "r");




    //transpose second matrix
    getLineIndex(fp1, &line1);
    getLineIndex(fp2, &line2);


    //Allocate memory for visited
    isVisited1 = malloc(line1*sizeof(int));
    isVisited2 = malloc(line2*sizeof(int));
    offsetarray1 = malloc(line1*sizeof(int));
    offsetarray2 = malloc(line2*sizeof(int));

    for (int p = 0; p < line1; ++p) {
        isVisited1[p] = 0;
        offsetarray1[p] = 0;
    }
    for (int p = 0; p < line2; ++p) {
        isVisited2[p] = 0;
        offsetarray2[p] = 0;
    }


    getOffset(fp1, offsetarray1);
    getOffset(fp2, offsetarray2);



    maxThreads = 10;


    thread_inp *inp = malloc(maxThreads * sizeof(thread_inp));
    for (int i = 0; i < maxThreads; ++i) {
        inp[i].fp1 = fp1;
        inp[i].fp2 = fp2;
        inp[i].off1 = offsetarray1;
        inp[i].off2 = offsetarray2;
    }
    pthread_t *thread_create = malloc(maxThreads* sizeof(pthread_t));

    for (int i = 0; i < maxThreads; ++i) {
        pthread_create(&thread_create[i], NULL, threadfun, (void*)inp);
        pthread_join(thread_create[i], NULL);
    }



    //Close files
    fclose(fp1);
    fclose(fp2);

    //free pointer
    free(isVisited1);
    free(isVisited2);


   /* //NUMBER OF THREADS
    int n = 10;
    int rows = 25, columns = 25;
    int *acc = malloc((rows * columns) * sizeof(int));
    //Thread id is stored
    pthread_t *tid = (pthread_t*)malloc(n * sizeof(pthread_t));
    threadInp *obj = malloc(n * sizeof (threadInp));
    for (int i = 0; i < n; ++i) {
        //for rows, number of cols
        obj[i].buff = malloc(columns* sizeof (int));

    }

    int nextStart = 0;
    for (int i = 0; i < n; ++i) {
        obj[i].length = 25;
        obj[i].start = nextStart;

        int temp;
        if(rows%n == 0)
            temp = rows/n;
        else
            temp = rows/n + 1;

        nextStart += temp;

        if(rows%n == 0)
            obj[i].count = temp;
        else{
            if(i != n - 1)
                obj[i].count = temp;
            else
                obj[i].count = rows%n;
        }


        obj[i].file_name = "in1.txt";
        pthread_create(&tid[i], NULL, threadfun,(void*)obj );
        pthread_join(&tid[i], NULL);
    }

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < columns; j++)

            //printf("%d ", [i * columns + j]);

        printf("\n");

    }
    //free(arr);*/
}

