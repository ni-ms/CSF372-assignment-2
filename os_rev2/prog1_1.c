#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_ROWS 100
#define MAX_COLS 100
#define MAX_THREADS 10

typedef struct {
    int matrix[MAX_ROWS][MAX_COLS];
    int rows;
    int cols;
} Matrix;

Matrix matrix1, matrix2;

typedef struct {
    FILE *file;
    int *values;
    int numCols;
    int line;
} ReadLineArgs;

typedef union {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
} Semun;

int shmMatrixId1, shmMatrixId2;
int semMutexId;
Matrix *sharedMatrix1, *sharedMatrix2;
int mutexSem;

// Function to read a line from the CSV file
void readCSVLine(FILE *file, int line, int *values, int numCols) {
    char buffer[1024];
    fseek(file, 0, SEEK_SET);

    for (int i = 0; i <= line; i++) {
        fgets(buffer, sizeof(buffer), file);
    }

    char *saveptr;
    char *token = strtok_r(buffer, ",", &saveptr);
    int col = 0;

    while (token != NULL && col < numCols) {
        values[col] = atoi(token);
        token = strtok_r(NULL, ",", &saveptr);
        col++;
    }
}

// Function executed by each thread to read a line from the CSV
void *readLine(void *arg) {
    ReadLineArgs *args = (ReadLineArgs *)arg;
    int line = args->line;

    readCSVLine(args->file, line, args->values, args->numCols);

    pthread_exit(NULL);
}

// Function executed by each thread to read the CSV file
void *readCSV(void *arg) {
    int threadId = *(int *)arg;

    FILE *file;
    char filename[10];
    sprintf(filename, "file%d.csv", threadId + 1);
    file = fopen(filename, "r");

    if (file == NULL) {
        printf("Failed to open file %d.csv\n", threadId + 1);
        exit(1);
    }

    int numRows = 0;
    int numCols = 0;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        numRows++;
        char *saveptr;
        char *token = strtok_r(buffer, ",", &saveptr);
        int col = 0;

        while (token != NULL) {
            col++;
            token = strtok_r(NULL, ",", &saveptr);
        }

        if (numCols == 0) {
            numCols = col;
        } else if (numCols != col) {
            printf("Inconsistent number of columns in file %d.csv\n", threadId + 1);
            exit(1);
        }
    }

    Matrix *matrix;
    if (threadId == 0) {
        matrix = sharedMatrix1;
        matrix->rows = numRows;
        matrix->cols = numCols;
    } else if (threadId == 1) {
        matrix = sharedMatrix2;
        matrix->rows = numRows;
        matrix->cols = numCols;
    }

    // Reset file position indicator to the beginning
    fseek(file, 0, SEEK_SET);

    pthread_t lineThreads[MAX_THREADS];
    ReadLineArgs *lineArgs = malloc(numRows * sizeof(ReadLineArgs));

    for (int i = 0; i < numRows; i++) {
        int *values = malloc(numCols * sizeof(int));
        lineArgs[i].file = file;
        lineArgs[i].values = values;
        lineArgs[i].numCols = numCols;
        lineArgs[i].line = i;

        if (pthread_create(&lineThreads[i], NULL, readLine, &lineArgs[i]) != 0) {
            printf("Failed to create line thread for file %d.csv\n", threadId + 1);
            exit(1);
        }
    }

    // Wait for the line threads to finish
    for (int i = 0; i < numRows; i++) {
        pthread_join(lineThreads[i], NULL);
    }

    // Copy the values to the matrix
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            matrix->matrix[i][j] = lineArgs[i].values[j];
        }
        free(lineArgs[i].values);  // Free allocated memory for each line's values
    }

    fclose(file);
    free(lineArgs);  // Free the dynamically allocated lineArgs array
    pthread_exit(NULL);
}

// Function to initialize the semaphore
void initializeSemaphore() {
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } semArg;

    mutexSem = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (mutexSem == -1) {
        perror("Failed to create semaphore");
        exit(1);
    }

    semArg.val = 1;
    if (semctl(mutexSem, 0, SETVAL, semArg) == -1) {
        perror("Failed to initialize semaphore value");
        exit(1);
    }
}

// Function to perform the IPC setup for shared memory and semaphore
void setupIPC() {
    // Create shared memory for matrix1
    shmMatrixId1 = shmget(1234, sizeof(Matrix), IPC_CREAT | 0666);
    if (shmMatrixId1 == -1) {
        perror("Failed to create shared memory for matrix1");
        exit(1);
    }

    sharedMatrix1 = (Matrix *)shmat(shmMatrixId1, NULL, 0);
    if (sharedMatrix1 == (void *)-1) {
        perror("Failed to attach shared memory for matrix1");
        exit(1);
    }

    // Create shared memory for matrix2
    shmMatrixId2 = shmget(5678, sizeof(Matrix), IPC_CREAT | 0666);
    if (shmMatrixId2 == -1) {
        perror("Failed to create shared memory for matrix2");
        exit(1);
    }

    sharedMatrix2 = (Matrix *)shmat(shmMatrixId2, NULL, 0);
    if (sharedMatrix2 == (void *)-1) {
        perror("Failed to attach shared memory for matrix2");
        exit(1);
    }

    // Initialize the semaphore
    initializeSemaphore();
}

// Function to destroy shared memory and semaphore
void cleanupIPC() {
    // Detach and remove shared memory for matrix1
    if (shmdt(sharedMatrix1) == -1) {
        perror("Failed to detach shared memory for matrix1");
        exit(1);
    }

    if (shmctl(shmMatrixId1, IPC_RMID, NULL) == -1) {
        perror("Failed to remove shared memory for matrix1");
        exit(1);
    }

    // Detach and remove shared memory for matrix2
    if (shmdt(sharedMatrix2) == -1) {
        perror("Failed to detach shared memory for matrix2");
        exit(1);
    }

    if (shmctl(shmMatrixId2, IPC_RMID, NULL) == -1) {
        perror("Failed to remove shared memory for matrix2");
        exit(1);
    }

    // Remove the semaphore
    if (semctl(mutexSem, 0, IPC_RMID) == -1) {
        perror("Failed to remove semaphore");
        exit(1);
    }
}

int main() {
    // Initialize the matrices with zeros
    memset(matrix1.matrix, 0, sizeof(matrix1.matrix));
    memset(matrix2.matrix, 0, sizeof(matrix2.matrix));

    pthread_t threads[2];
    int threadIds[2] = {0, 1};

    // Setup IPC (shared memory and semaphore)
    setupIPC();

    // Create two threads to read the CSV files
    for (int i = 0; i < 2; i++) {
        if (pthread_create(&threads[i], NULL, readCSV, &threadIds[i]) != 0) {
            printf("Failed to create thread %d\n", i + 1);
            exit(1);
        }
    }

    // Wait for the threads to finish
    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }

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

    // Cleanup IPC resources
   // cleanupIPC();

    return 0;
}
