#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

//Concerns:
//MMAP (fast for more i/o) or read file(fget)?
//


//Global variables
FILE *fp1;
FILE *fp2;

typedef struct arr1{
    int row;
    //Integer array/pointer
    int* list1;
} array1;
typedef struct arr2{
    int col;
    int* list2;
} array2;

typedef struct readAndStore{
    array1 *rVal;
    array2 *cVal;
} RAS_t;

/*
typedef struct readFile{
    int row, column;
    int* list1;
    int* list2;
} RF_t;
*/

void* readFileFun(void* args){





    // Get the row size here
    //listVal = malloc()



};

int main(int argc, char * argv[]) {
    //Fork and exec
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

    fp1 = fopen(mat1,"r");
    fp2 = fopen(mat2,"r");

    //NUMBER OF THREADS
    int n = 10;

    //Thread id is stored
    pthread_t *tid = (pthread_t*)malloc(n * sizeof(pthread_t));
    //Create i threads to read from file1
    for (int numOfThreads = 1; numOfThreads <= n; ++i) {

        //ALLOCATING THE MEMORY FOR 4D ARRAY
        //Storing the first array
        array1 *dat1 = (array1*)malloc(i*sizeof(array1));
        for(int temp = 0; temp < i; temp++){
            dat1[temp].row = temp;
            //need to allocate to the list inside the datatype itself
            dat1[temp].list1 = (int*)malloc(j*sizeof(int));
        }
    /*
    To access the i,j
    dat1[i].list1[j];
    */
            //Store the second array
        array2 *dat2 = (array2*)malloc(k*sizeof (array2));
        for (int temp = 0; temp < k; ++temp) {
            dat2[temp].col = temp;
            dat2[temp].list2 = (int*)malloc(j*sizeof(int));

        }
 //CHECK VALUE

    //Store both array
//    RAS_t **datf_t = malloc(i*sizeof (RAS_t));
//        for (int p = 0; p < i; ++p) {
//            RAS_t *datf = malloc(k*sizeof (RAS_t));
//            datf_t[p] = datf;
//        }
//
//        for (int num1 = 0; num1 < i; ++num1) {
//            for (int num2 = 0; num2 < k; ++num2) {
//                datf_t[num1][num2].rVal = dat1;
//                datf_t[num1][num2].cVal = dat2;
//            }
//        }

        RAS_t *finalAbs;
        finalAbs->rVal = dat1;
        finalAbs->cVal = dat2;



    //Assign row and column for data

    for (int l = i; l > 0 ; l--) {
        //Thread arguments
        if(pthread_create(&tid[i-l],NULL,readFileFun,(void*)finalAbs) != 0){
            fprintf( stderr, "ERROR: Could not create thread\n" );
            exit(EXIT_FAILURE);
        }
    }

    //Create j threads to read from file 2
    //Create ixj threads to get the input from the previous threads
    }

    return 0;
}