
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>
#include <math.h>

#define DOWN 1000000000
#define UP   1100000000
// #define DOWN 490
// #define UP   3000
#define UNIT_COUNT 8

int pw[2];
int pr[2];

int c[UNIT_COUNT][2];

typedef struct {
    long long int value;           // value of the prime number
} perfect_t;

typedef struct {
    int isEmpty;
} child_t;

child_t child[UNIT_COUNT];

int isPerfect(long long int n) 
{ 
    long long int sum = 1; 
   
    for (long long int i=2; i*i<=n; i++) 
    { 
        if (n%i==0) 
        { 
            if(i*i!=n) 
                sum = sum + i + n/i; 
            else
                sum=sum+i; 
        } 
    }  
     if (sum == n && n != 1) 
          return 1; 
   
     return 0; 
} 

perfect_t *first_n() 
{
    long long int i = 1;
    perfect_t *perfect = malloc(sizeof(perfect_t) * (UP - DOWN));

    for (i = DOWN; i < UP; i++) {
        perfect[i - DOWN].value = i;
    }

    return perfect;
}

void child_worker(int i)
{
    child_t child;
    read(c[i][0], &child, sizeof(child_t));
    while (1) {
        perfect_t perfect;
        read(c[i][0], &perfect, sizeof(perfect_t));
        child.isEmpty = 0;
        if (perfect.value == -1) { //since prime number cannot be -1 i use this condition to terminate child process later
            child.isEmpty = 1;
            break;
        }


        if (isPerfect(perfect.value)) {
            write(pr[1], &perfect, sizeof(perfect_t));
            child.isEmpty = 1;
        } 
    }
}

int main() {

    perfect_t *perfects = first_n();

    if (pipe(pw) < 0) exit(1);
    if (pipe(pr) < 0) exit(1);

    for (int i = 0; i < UNIT_COUNT; i++) {
        child[i].isEmpty = 1;
    }
    


    for (int i = 0; i < UNIT_COUNT; i++) {
        if (pipe(c[i]) < 0) exit(1);
    }
    

    for (int i = 0; i < UNIT_COUNT; i++) {
        if (fork() == 0) {
            child_worker(i);
            exit(1);
        }
    }

    for (int i = 0; i < UNIT_COUNT; i++) {
        write(c[i][1], &child[i], sizeof(child_t));
    }
    

    for (int i = 0; i < (UP - DOWN); i++) {
        int childC = -1;
        for (int k = 0; k < UNIT_COUNT; k++) {
            if (child[k].isEmpty == 1) {
                childC = k;
                break;
            }
        }
        if (childC == -1) {
            sleep(1);
            i--;
            continue;
        }       
        write(c[childC][1], &perfects[i], sizeof(perfect_t));
    }

    printf("finished\n");
    fflush(stdout);
    
    fcntl (pr[0], F_SETFL, O_ASYNC | O_NONBLOCK);
    while (1) {
        perfect_t perfect;
        int nread = read(pr[0], &perfect, sizeof(perfect_t));
        if (nread == -1) {
            int finish = 1;
            for (int i = 0; i < UNIT_COUNT; i++) {
                if (child[i].isEmpty == 0) {
                    finish = 0;
                    break;
                }
            }
            if (finish) {
                break;
            } else {
                continue;
            }
        }
        printf("got value:%lld\n", perfect.value);
        fflush(stdout);
    }

    for (int i = 0; i < UNIT_COUNT; i++) {
        perfect_t terminate;
        terminate.value = -1;
        write(c[i][1], &terminate, sizeof(perfect_t));
    }
    
    for (int i = 0; i < UNIT_COUNT; i++) {
        wait(NULL);
    }
    
    return 0;
}