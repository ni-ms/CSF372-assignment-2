#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <fcntl.h>

FILE *fp;
typedef struct fBuffer{
    int length;
    int start, count;
    char** buff;
    char* file_name;

} threadInp;
void* threadfun(void* args){
threadInp *obj1 = (threadInp*)args;
FILE* fp = fopen(obj1->file_name, "r");
    fseek(fp, obj1->start, SEEK_SET);
    for (int i = 0; i < obj1->count ; ++i) {
        fgets(obj1->buff[i],obj1->length,fp);
    }



}

int main(int argc, char * argv[]){
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


    //NUMBER OF THREADS
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
    //free(arr);
}

