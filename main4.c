//SCHEDULING
#include <stdio.h>
#include <stdlib.h>
//#include <

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

int main(){
    pid_t p1;
    pid_t p2;
    p1  = fork();
    
    int status;

    if (p1 == 0){
        execlp("main2.out", "P1", "", NULL);
    }

    else if (p2 > 0){

        p2 = fork();
        
        if (p2 == 0){

        }

        else if(p2 > 0){
            nanosleep(1000000);
            execlp("main3.out", "P2", "",NULL);
        }


    }
}
