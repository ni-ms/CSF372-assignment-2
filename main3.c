//IPC
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

int main(){
    key_t key = ftok("shmfile",65);
    int shmid = shmget(key,1024,0666|IPC_CREAT);
    int*shmseg = (int*) shmat(shmid,(void*)0,0);
    printf("Data read from memory\n");
    for (int i = 0; i < 1024; ++i) {
        printf("%d ", shmseg[i]);
    }
    shmdt(shmseg);
    shmctl(shmid,IPC_RMID,NULL);
    return 0;
}

