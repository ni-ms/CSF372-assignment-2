//IPC
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

// Program to multiply the two elements

typedef struct {
    int row;
    int col;
    int* list1;
    int* list2;
} RAS_t;


void* multiplyFun(void* args){
    //get row and column
    //multiply
}
int main(){
    key_t key = ftok("./cmake-build-debug/shm/shmfile1.txt", 65);
    int shmid = shmget(key,024,0666|IPC_CREAT);
    int*shmseg = (int*) shmat(shmid,(void*)0,0);
    printf("Data read from memory\n");









    for (int i = 0; i < 1024; ++i) {
        printf("%d ", shmseg[i]);
    }
    shmdt(shmseg);
    shmctl(shmid,IPC_RMID,NULL);
    return 0;
}

