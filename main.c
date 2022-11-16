#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
typedef struct readFile{
int line;
int* list;
} RF_t;

void* readFileFun(void* args){
    RF_t* obj = (RF_t*)args;
    int lineNo = obj->line;
    int* listVal = obj->line;
    // Get the row size here
    //listVal = malloc()



}

int main(int argc, char * argv[]) {
    if(argc != 7){
        fprintf(stderr,"ERROR: Invalid Arguments\n" );
        fprintf(stderr,"USAGE: ./group12_assignment2.out i j k in1.txt in2.txt out.txt\n" );
        return EXIT_FAILURE;
    }
    int i = atoi(argv[1]);
    int j = atoi(argv[2]);
    int k = atoi(argv[3]);
    char *mat1 = argv[4];
    char *mat2 = argv[5];
    char *mat3 = argv[6];

    printf("Hi");
    //Create i threads to read from file1
    pthread_t *tid = malloc(i * sizeof (pthread_t));
    RF_t *dat1 = malloc(i*sizeof(RF_t));
    for (int l = i; l > 0 ; l--) {
        //Thread arguments
        if(pthread_create(&tid[i-l],NULL,readFileFun,(void*)dat1) != 0){
            fprintf( stderr, "ERROR: Could not create thread\n" );
            exit(EXIT_FAILURE);
        }
    }

    //Create j threads to read from file 2
    //Create ixj threads to get the input from the previous threads

    return 0;
}