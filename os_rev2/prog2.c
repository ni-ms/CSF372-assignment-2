#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define MAX_ROWS 100
#define MAX_COLS 100

typedef struct {
    int matrix[MAX_ROWS][MAX_COLS];
    int rows;
    int cols;
} Matrix;

typedef struct {
    int row_start;
    int row_end;
    Matrix *matrix1;
    Matrix *matrix2;
    Matrix *result;
} ThreadData;

void *multiply(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    for (int i = data->row_start; i <= data->row_end; i++) {
        for (int j = 0; j < data->matrix2->cols; j++) {
            data->result->matrix[i][j] = 0;
            for (int k = 0; k < data->matrix1->cols; k++) {
                data->result->matrix[i][j] += data->matrix1->matrix[i][k] * data->matrix2->matrix[k][j];
            }
        }
    }

    return NULL;
}


int main() {
    int shmMatrixId1, shmMatrixId2;
    Matrix *sharedMatrix1, *sharedMatrix2;
    Matrix resultMatrix;


    // Attach to the shared memory for matrix1
    shmMatrixId1 = shmget(1234, sizeof(Matrix), 0666);
    if (shmMatrixId1 == -1) {
        perror("Failed to access shared memory for matrix1");
        exit(1);
    }

    sharedMatrix1 = (Matrix *) shmat(shmMatrixId1, NULL, 0);
    if (sharedMatrix1 == (void *) -1) {
        perror("Failed to attach shared memory for matrix1");
        exit(1);
    }

    // Attach to the shared memory for matrix2
    shmMatrixId2 = shmget(5678, sizeof(Matrix), 0666);
    if (shmMatrixId2 == -1) {
        perror("Failed to access shared memory for matrix2");
        exit(1);
    }

    sharedMatrix2 = (Matrix *) shmat(shmMatrixId2, NULL, 0);
    if (sharedMatrix2 == (void *) -1) {
        perror("Failed to attach shared memory for matrix2");
        exit(1);
    }
    printf("Accessed shared memory for matrix1 and matrix2\n");
    // Print Matrix 1
    printf("Matrix 1 (%d x %d):\n", sharedMatrix1->rows, sharedMatrix1->cols);
    for (int i = 0; i < sharedMatrix1->rows; i++) {
        for (int j = 0; j < sharedMatrix1->cols; j++) {
            printf("%d ", sharedMatrix1->matrix[i][j]);
        }
        printf("\n");
    }

    // Print Matrix 2
    printf("\nMatrix 2 (%d x %d):\n", sharedMatrix2->rows, sharedMatrix2->cols);
    for (int i = 0; i < sharedMatrix2->rows; i++) {
        for (int j = 0; j < sharedMatrix2->cols; j++) {
            printf("%d ", sharedMatrix2->matrix[i][j]);
        }
        printf("\n");
    }

    // Prepare result matrix
    resultMatrix.rows = sharedMatrix1->rows;
    resultMatrix.cols = sharedMatrix2->cols;

    // Create threads for matrix multiplication
    int num_threads = sharedMatrix1->rows;
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].row_start = i;
        thread_data[i].row_end = i;
        thread_data[i].matrix1 = sharedMatrix1;
        thread_data[i].matrix2 = sharedMatrix2;
        thread_data[i].result = &resultMatrix;

        pthread_create(&threads[i], NULL, multiply, (void *)&thread_data[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Detach from the shared memory for matrix1
    if (shmdt(sharedMatrix1) == -1) {
        perror("Failed to detach shared memory for matrix1");
        exit(1);
    }

    // Detach from the shared memory for matrix2
    if (shmdt(sharedMatrix2) == -1) {
        perror("Failed to detach shared memory for matrix2");
        exit(1);
    }


    // Save the result in a text file
    FILE *outputFile = fopen("result.txt", "w");
    if (outputFile == NULL) {
        perror("Failed to open output file");
        exit(1);
    }

    fprintf(outputFile, "Result Matrix (%d x %d):\n", resultMatrix.rows, resultMatrix.cols);
    for (int i = 0; i < resultMatrix.rows; i++) {
        for (int j = 0; j < resultMatrix.cols; j++) {
            fprintf(outputFile, "%d ", resultMatrix.matrix[i][j]);
        }
        fprintf(outputFile, "\n");
    }

    fclose(outputFile);


    return 0;
}
