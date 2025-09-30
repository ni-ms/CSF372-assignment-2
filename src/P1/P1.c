#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define MAX_ROWS 100
#define MAX_COLS 100
#define MAX_THREADS 10

typedef struct {
    int matrix[MAX_ROWS][MAX_COLS];
    int rows;
    int cols;
} Matrix;

typedef union {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
} Semun;

int shmMatrixId1, shmMatrixId2;
int mutexSem;
Matrix *sharedMatrix1, *sharedMatrix2;

// Signal handler for cleanup
void signalHandler(int signal) {
    printf("\nReceived signal %d, cleaning up...\n", signal);
    exit(1);
}

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

// Function to read text file (space-separated values)
void readTextFile(const char *filename, Matrix *matrix) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file %s\n", filename);
        exit(1);
    }

    char buffer[1024];
    int row = 0;
    int cols = 0;

    // Read file line by line
    while (fgets(buffer, sizeof(buffer), file) != NULL && row < MAX_ROWS) {
        // Skip empty lines and comments
        if (buffer[0] == '\n' || buffer[0] == '#') {
            continue;
        }

        char *saveptr;
        // Use space, tab, and newline as delimiters
        char *token = strtok_r(buffer, " \t\n", &saveptr);
        int col = 0;

        while (token != NULL && col < MAX_COLS) {
            matrix->matrix[row][col] = atoi(token);
            token = strtok_r(NULL, " \t\n", &saveptr);
            col++;
        }

        if (col == 0) {
            continue; // Skip empty lines
        }

        if (row == 0) {
            cols = col; // Set number of columns from first row
        } else if (col != cols) {
            printf("Inconsistent number of columns in %s at row %d\n", filename, row + 1);
            fclose(file);
            exit(1);
        }

        row++;
    }

    matrix->rows = row;
    matrix->cols = cols;

    fclose(file);
}

// Function executed by each thread to read the text file
void *readText(void *arg) {
    int threadId = *(int *) arg;

    char filename[20];
    sprintf(filename, "file%d.txt", threadId + 1);

    // Create temporary matrix to read data
    Matrix tempMatrix;
    memset(&tempMatrix, 0, sizeof(Matrix));

    // Read text file into temporary matrix
    readTextFile(filename, &tempMatrix);

    // Use semaphore to safely copy to shared memory
    semWait(mutexSem);

    if (threadId == 0) {
        memcpy(sharedMatrix1, &tempMatrix, sizeof(Matrix));
        printf("Thread %d: Read matrix1 (%d x %d) from %s\n",
               threadId, sharedMatrix1->rows, sharedMatrix1->cols, filename);
    } else if (threadId == 1) {
        memcpy(sharedMatrix2, &tempMatrix, sizeof(Matrix));
        printf("Thread %d: Read matrix2 (%d x %d) from %s\n",
               threadId, sharedMatrix2->rows, sharedMatrix2->cols, filename);
    }

    semSignal(mutexSem);

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
    shmMatrixId1 = shmget(IPC_PRIVATE, sizeof(Matrix), IPC_CREAT | 0666);
    if (shmMatrixId1 == -1) {
        perror("Failed to create shared memory for matrix1");
        exit(1);
    }

    sharedMatrix1 = (Matrix *) shmat(shmMatrixId1, NULL, 0);
    if (sharedMatrix1 == (void *) -1) {
        perror("Failed to attach shared memory for matrix1");
        exit(1);
    }

    // Create shared memory for matrix2
    shmMatrixId2 = shmget(IPC_PRIVATE, sizeof(Matrix), IPC_CREAT | 0666);
    if (shmMatrixId2 == -1) {
        perror("Failed to create shared memory for matrix2");
        exit(1);
    }

    sharedMatrix2 = (Matrix *) shmat(shmMatrixId2, NULL, 0);
    if (sharedMatrix2 == (void *) -1) {
        perror("Failed to attach shared memory for matrix2");
        exit(1);
    }

    // Initialize shared memory to zero
    memset(sharedMatrix1, 0, sizeof(Matrix));
    memset(sharedMatrix2, 0, sizeof(Matrix));

    // Initialize the semaphore
    initializeSemaphore();

    printf("IPC Setup completed:\n");
    printf("Shared Memory ID1: %d\n", shmMatrixId1);
    printf("Shared Memory ID2: %d\n", shmMatrixId2);
    printf("Semaphore ID: %d\n", mutexSem);
}

// Function to destroy shared memory and semaphore
void cleanupIPC() {
    printf("Cleaning up IPC resources...\n");

    // Detach and remove shared memory for matrix1
    if (sharedMatrix1 != NULL) {
        if (shmdt(sharedMatrix1) == -1) {
            perror("Failed to detach shared memory for matrix1");
        }
    }

    if (shmctl(shmMatrixId1, IPC_RMID, NULL) == -1) {
        perror("Failed to remove shared memory for matrix1");
    }

    // Detach and remove shared memory for matrix2
    if (sharedMatrix2 != NULL) {
        if (shmdt(sharedMatrix2) == -1) {
            perror("Failed to detach shared memory for matrix2");
        }
    }

    if (shmctl(shmMatrixId2, IPC_RMID, NULL) == -1) {
        perror("Failed to remove shared memory for matrix2");
    }

    // Remove the semaphore
    if (semctl(mutexSem, 0, IPC_RMID) == -1) {
        perror("Failed to remove semaphore");
    }

    printf("IPC cleanup completed\n");
}

// Function to validate matrix compatibility for multiplication
int validateMatrices() {
    if (sharedMatrix1->cols != sharedMatrix2->rows) {
        printf("Error: Matrix dimensions incompatible for multiplication\n");
        printf("Matrix1: %d x %d, Matrix2: %d x %d\n",
               sharedMatrix1->rows, sharedMatrix1->cols,
               sharedMatrix2->rows, sharedMatrix2->cols);
        printf("For multiplication: Matrix1 columns (%d) must equal Matrix2 rows (%d)\n",
               sharedMatrix1->cols, sharedMatrix2->rows);
        return 0;
    }

    printf("Matrix dimensions are compatible for multiplication\n");
    printf("Result will be %d x %d\n", sharedMatrix1->rows, sharedMatrix2->cols);
    return 1;
}

// Function to print matrix
void printMatrix(const char *name, Matrix *matrix) {
    printf("\n%s (%d x %d):\n", name, matrix->rows, matrix->cols);
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            printf("%4d ", matrix->matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    // Setup signal handlers for graceful cleanup
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    pthread_t threads[2];
    int threadIds[2] = {0, 1};

    printf("Matrix Reader Program - Part 1 of Matrix Multiplication System\n");
    printf("Reading from text files (file1.txt and file2.txt)\n");
    printf("==============================================================\n");

    // Setup IPC (shared memory and semaphore)
    setupIPC();

    // Create two threads to read the text files
    printf("\nCreating threads to read text files...\n");
    for (int i = 0; i < 2; i++) {
        if (pthread_create(&threads[i], NULL, readText, &threadIds[i]) != 0) {
            printf("Failed to create thread %d\n", i + 1);
            cleanupIPC();
            exit(1);
        }
    }

    // Wait for the threads to finish
    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nBoth text files read successfully\n");

    // Validate matrix compatibility for multiplication
    if (!validateMatrices()) {
        cleanupIPC();
        exit(1);
    }

    // Print the matrices
    printMatrix("Matrix 1", sharedMatrix1);
    printMatrix("Matrix 2", sharedMatrix2);

    printf("\n==============================================================\n");
    printf("Matrices are ready for multiplication by the next program\n");
    printf("Shared Memory IDs: %d (Matrix1), %d (Matrix2)\n", shmMatrixId1, shmMatrixId2);
    printf("Semaphore ID: %d\n", mutexSem);
    printf("Note: IPC resources remain active for the next program\n");

    // For this first program, we'll keep IPC resources active
    // They will be cleaned up by the final program in the pipeline
    // Uncomment the line below if you want to clean up immediately:
    // cleanupIPC();

    return 0;
}
