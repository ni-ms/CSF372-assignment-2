#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <signal.h>

#define MAX_ROWS 100
#define MAX_COLS 100
#define MAX_THREADS 8  // Optimal thread count based on typical CPU cores

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
    int thread_id;
} ThreadData;

// Global variables for IPC
int shmMatrixId1, shmMatrixId2, semId;
Matrix *sharedMatrix1, *sharedMatrix2;

// Function to wait on semaphore (P operation)
void semWait(int semId) {
    struct sembuf sem_op = {0, -1, 0};
    if (semop(semId, &sem_op, 1) == -1) {
        perror("Failed to wait on semaphore");
        exit(1);
    }
}

// Function to signal semaphore (V operation)
void semSignal(int semId) {
    struct sembuf sem_op = {0, 1, 0};
    if (semop(semId, &sem_op, 1) == -1) {
        perror("Failed to signal semaphore");
        exit(1);
    }
}

// Thread function for matrix multiplication
void *multiply(void *arg) {
    ThreadData *data = (ThreadData *) arg;

    printf("Thread %d: Processing rows %d to %d\n",
           data->thread_id, data->row_start, data->row_end);

    for (int i = data->row_start; i <= data->row_end; i++) {
        for (int j = 0; j < data->matrix2->cols; j++) {
            data->result->matrix[i][j] = 0;
            for (int k = 0; k < data->matrix1->cols; k++) {
                data->result->matrix[i][j] += data->matrix1->matrix[i][k] * data->matrix2->matrix[k][j];
            }
        }
    }

    printf("Thread %d: Completed processing rows %d to %d\n",
           data->thread_id, data->row_start, data->row_end);

    return NULL;
}

// Function to validate matrix compatibility
int validateMatrices(Matrix *mat1, Matrix *mat2) {
    if (mat1->cols != mat2->rows) {
        printf("Error: Matrix dimensions incompatible for multiplication\n");
        printf("Matrix1: %d x %d, Matrix2: %d x %d\n",
               mat1->rows, mat1->cols, mat2->rows, mat2->cols);
        printf("For multiplication: Matrix1 columns (%d) must equal Matrix2 rows (%d)\n",
               mat1->cols, mat2->rows);
        return 0;
    }

    printf("Matrix dimensions compatible for multiplication\n");
    printf("Matrix1: %d x %d, Matrix2: %d x %d\n",
           mat1->rows, mat1->cols, mat2->rows, mat2->cols);
    printf("Result will be: %d x %d\n", mat1->rows, mat2->cols);
    return 1;
}

// Function to print matrix (for debugging)
void printMatrix(const char *name, Matrix *matrix) {
    printf("\n%s (%d x %d):\n", name, matrix->rows, matrix->cols);
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            printf("%4d ", matrix->matrix[i][j]);
        }
        printf("\n");
    }
}

// Function to setup shared memory access
void setupSharedMemory(int shmId1, int shmId2, int semaphoreId) {
    // Attach to shared memory for matrix1
    sharedMatrix1 = (Matrix *) shmat(shmId1, NULL, 0);
    if (sharedMatrix1 == (void *) -1) {
        perror("Failed to attach shared memory for matrix1");
        exit(1);
    }

    // Attach to shared memory for matrix2
    sharedMatrix2 = (Matrix *) shmat(shmId2, NULL, 0);
    if (sharedMatrix2 == (void *) -1) {
        perror("Failed to attach shared memory for matrix2");
        exit(1);
    }

    // Store semaphore ID
    semId = semaphoreId;

    printf("Successfully attached to shared memory:\n");
    printf("Matrix1 ID: %d, Matrix2 ID: %d, Semaphore ID: %d\n",
           shmId1, shmId2, semaphoreId);
}

// Function to cleanup shared memory
void cleanupSharedMemory() {
    printf("Detaching from shared memory...\n");

    if (sharedMatrix1 != NULL) {
        if (shmdt(sharedMatrix1) == -1) {
            perror("Failed to detach shared memory for matrix1");
        }
    }

    if (sharedMatrix2 != NULL) {
        if (shmdt(sharedMatrix2) == -1) {
            perror("Failed to detach shared memory for matrix2");
        }
    }

    printf("Shared memory cleanup completed\n");
}

// Signal handler for cleanup
void signalHandler(int signal) {
    printf("\nReceived signal %d, cleaning up...\n", signal);
    cleanupSharedMemory();
    exit(1);
}

int main(int argc, char *argv[]) {
    // Check command line arguments
    if (argc != 4) {
        printf("Usage: %s <matrix1_shm_id> <matrix2_shm_id> <semaphore_id>\n", argv[0]);
        printf("Example: %s 123456 654321 789012\n", argv[0]);
        exit(1);
    }

    // Parse command line arguments
    int shmId1 = atoi(argv[1]);
    int shmId2 = atoi(argv[2]);
    int semaphoreId = atoi(argv[3]);

    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    printf("Matrix Multiplier Program - Part 2 of Matrix Multiplication System\n");
    printf("================================================================\n");

    // Setup shared memory access
    setupSharedMemory(shmId1, shmId2, semaphoreId);

    // Use semaphore to safely access shared memory
    semWait(semId);

    // Validate matrix compatibility
    if (!validateMatrices(sharedMatrix1, sharedMatrix2)) {
        semSignal(semId);
        cleanupSharedMemory();
        exit(1);
    }

    // Print matrices for verification
    printMatrix("Matrix 1", sharedMatrix1);
    printMatrix("Matrix 2", sharedMatrix2);

    semSignal(semId);

    // Prepare result matrix
    Matrix resultMatrix;
    memset(&resultMatrix, 0, sizeof(Matrix));
    resultMatrix.rows = sharedMatrix1->rows;
    resultMatrix.cols = sharedMatrix2->cols;

    // Determine optimal number of threads (don't exceed matrix rows or MAX_THREADS)
    int num_threads = (sharedMatrix1->rows < MAX_THREADS) ? sharedMatrix1->rows : MAX_THREADS;
    int rows_per_thread = sharedMatrix1->rows / num_threads;
    int remaining_rows = sharedMatrix1->rows % num_threads;

    printf("\nStarting matrix multiplication with %d threads\n", num_threads);
    printf("Rows per thread: %d, Remaining rows: %d\n", rows_per_thread, remaining_rows);

    // Allocate memory for threads and thread data
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    ThreadData *thread_data = malloc(num_threads * sizeof(ThreadData));

    if (threads == NULL || thread_data == NULL) {
        printf("Failed to allocate memory for threads\n");
        cleanupSharedMemory();
        exit(1);
    }

    // Create threads for matrix multiplication
    int current_row = 0;
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].row_start = current_row;
        thread_data[i].row_end = current_row + rows_per_thread - 1;

        // Distribute remaining rows among first threads
        if (i < remaining_rows) {
            thread_data[i].row_end++;
        }

        thread_data[i].matrix1 = sharedMatrix1;
        thread_data[i].matrix2 = sharedMatrix2;
        thread_data[i].result = &resultMatrix;
        thread_data[i].thread_id = i;

        current_row = thread_data[i].row_end + 1;

        if (pthread_create(&threads[i], NULL, multiply, (void *) &thread_data[i]) != 0) {
            printf("Failed to create thread %d\n", i);
            free(threads);
            free(thread_data);
            cleanupSharedMemory();
            exit(1);
        }
    }

    // Wait for all threads to finish
    printf("\nWaiting for threads to complete...\n");
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads completed successfully\n");

    // Print result matrix
    printMatrix("Result Matrix", &resultMatrix);

    // Save the result in a text file
    FILE *outputFile = fopen("result.txt", "w");
    if (outputFile == NULL) {
        perror("Failed to open output file");
        free(threads);
        free(thread_data);
        cleanupSharedMemory();
        exit(1);
    }

    fprintf(outputFile, "Matrix Multiplication Result (%d x %d):\n",
            resultMatrix.rows, resultMatrix.cols);
    fprintf(outputFile, "Matrix1: %d x %d, Matrix2: %d x %d\n\n",
            sharedMatrix1->rows, sharedMatrix1->cols,
            sharedMatrix2->rows, sharedMatrix2->cols);

    for (int i = 0; i < resultMatrix.rows; i++) {
        for (int j = 0; j < resultMatrix.cols; j++) {
            fprintf(outputFile, "%d ", resultMatrix.matrix[i][j]);
        }
        fprintf(outputFile, "\n");
    }

    fclose(outputFile);
    printf("\nResult saved to result.txt\n");

    // Cleanup
    free(threads);
    free(thread_data);
    cleanupSharedMemory();

    printf("\n================================================================\n");
    printf("Matrix multiplication completed successfully\n");
    printf("Result matrix: %d x %d saved to result.txt\n",
           resultMatrix.rows, resultMatrix.cols);

    return 0;
}
