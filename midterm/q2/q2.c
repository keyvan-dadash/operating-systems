
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
#include <errno.h>

sem_t mutexs[32];

typedef struct {
    int id;
    void (*func)();
} args_t;

void *function(void *args) 
{
    args_t *arg = (args_t *)args;
    int id = arg->id;
    void (*fun_ptr)() = arg->func;

    sem_wait(&mutexs[id]);

    (*fun_ptr)();  

    sem_post(&mutexs[id]);


}

void func1() {
    printf("func1 stared\n");
    sleep(1);
    printf("func1 finished\n");
    fflush(stdout);
}

void func2() {
    printf("func2 stared\n");
    sleep(2);
    printf("func2 finished\n");
    fflush(stdout);
}

void func3() {
    printf("func3 stared\n");
    sleep(3);
    printf("func3 finished\n");
    fflush(stdout);
}

int main() {

    for (int i = 0; i < 32; i++) {
        sem_init(&mutexs[i], 0, 1); 
    }
    

    pthread_t threads[3];

    args_t args[3];

    args[0].id = 1;
    args[0].func = &func1;

    args[1].id = 2;
    args[1].func = &func2;

    args[2].id = 1;
    args[2].func = &func3;

    pthread_create(&threads[0], NULL, function, (void *)&args[0]);
    pthread_create(&threads[1], NULL, function, (void *)&args[1]);
    pthread_create(&threads[2], NULL, function, (void *)&args[2]);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);
}