//SCHEDULING
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#define P_NUM 2
#define Quant1 1
#define Quant2 2

/*int sum = 0, count = 0;
float avg_wt, avg_tat;
int main(int argc, char * argv[]) {

    for (int i = 0; i < P_NUM ; ++i) {
        fork
    }

} */
//Argument 20 50 20 inp1.txt inp2.txt out.txt

int main(){
    pid_t p1;
    pid_t p2;
    p1  = fork();
    
    int status;

    if (p1 == 0){
        execlp("./cmake-build-debug/main2.out", "20", "50","20","inp1.txt", "inp2.txt","out.txt" NULL);
    }

    else if (p2 > 0){

        p2 = fork();
        
        if (p2 == 0){

        }

        else if(p2 > 0){
            nanosleep(1000000);
            execlp("./cmake-build-debug/main3.out", "P2", "",NULL);
        }


    }
}
