#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <fcntl.h>


//Global variables
FILE *fp1, *fp2;
int i,j,k;
char* mat1, *mat2, *mat3;
int *arr1, *arr2;
int *isVisited1, *isVisited2;
int line1 = 0, line2 = 0;
FILE *fp3;

void getLineIndex(FILE *fp, int var){
    if(fp == NULL){
        fprintf(stderr, "ERROR: File not found\n");
        exit(EXIT_FAILURE);
    }
    char c;
    for (c = getc(fp); c != EOF ; c = getc(fp)) {
        if (c == '\n') {
         var++;
        }
    }
    printf("Number of lines in file: %d\n", var);
}


typedef struct fBuffer{
    int length;
    int start, count;


} threadInp;
/*
void* threadfun(void* args){
threadInp *obj1 = (threadInp*)args;
FILE* fp = fopen(obj1->file_name, "r");
    fseek(fp, obj1->start, SEEK_SET);
    for (int i = 0; i < obj1->count ; ++i) {
        fgets(obj1->buff[i],obj1->length,fp);
    }



}*/

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



    getLineIndex(fp1, line1);
    getLineIndex(fp2, line2);


    //Allocate memory for isvisited
    isVisited1 = malloc(line1*sizeof(int));
    isVisited2 = malloc(line2*sizeof(int));


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

