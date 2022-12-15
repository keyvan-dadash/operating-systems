
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

#define N 100
#define UNIT_COUNT 8
#define PARENT_KEY 2222

#define PARENT_SEM "parent"


typedef struct {
    int value;           // value of the prime number
    int counter;         // a counter to decrement (at the start counter = value)
    int first_value;     // this is for debug
    int first_child;     // this is for debug
} prime_t;

typedef struct {
    int current_index;
    int number_of_tasks;
    prime_t tasks[N];
} shm_t;


int parent_shmid;
int child_shmid[UNIT_COUNT];

shm_t *child_shm[UNIT_COUNT];


int is_prime(int value)
{
    int sqr = (int)sqrt((double) value);
    for (int i = 2; i <= sqr; i++)
    {
        if (value % i == 0)
            return 0;
    }
    return 1;
}

prime_t *first_n_prime(int n) 
{
    int count = 0;
    int i = 1;
    prime_t *primes = malloc(sizeof(prime_t) * n);

    while (count != n) {
        if (is_prime(i)) {
            primes[count].value = i;
            primes[count].counter = i;
            primes[count].first_value = i;
            count++;
        }
        i++;
    }

    return primes;
}

void child_worker(int i)
{
    sleep(2);

    char child_sem_char[2];
    sprintf(child_sem_char, "%d", i * 10);

    key_t key = ftok("/tmp", i);

    int child_shmid_l = shmget(key, sizeof(shm_t), 0666);
    shm_t *child_shm_l = shmat(child_shmid_l, NULL, 0);

    sem_t *child = sem_open(child_sem_char, 0);

    //////////////////
    key_t parent_key = ftok("/tmp", PARENT_KEY);

    int parent_shmid = shmget(parent_key, sizeof(shm_t), 0666);
    shm_t *parent_shm = shmat(parent_shmid, NULL, 0);

    sem_t *parent_sem = sem_open(PARENT_SEM, 0);


    while (1) {
        if (child_shm_l->number_of_tasks == 0) {
            sleep(1);
            // printf("%d %d %d %d\n", child_shm_l->number_of_tasks, i, child_shmid_l, child_shmid[i]);
            // fflush(stdout);
            continue;
        }
        
        prime_t prime = child_shm_l->tasks[0];

        int semval;
        sem_getvalue(child, &semval);
        // printf("%d %d %d %d %d\n", prime.value, prime.counter, prime.first_child, prime.first_value, semval);
        fflush(stdout);

        if (!sem_wait(child)) {
            for (int i = 0; i < (child_shm_l->number_of_tasks - 1); i++) {
                child_shm_l->tasks[i] = child_shm_l->tasks[i + 1];
            }
            
            child_shm_l->current_index--;
            child_shm_l->number_of_tasks--;

            sem_post(child);
        }

        if (prime.value == -1 && prime.counter == -10) //since prime number cannot be -1 i use this condition to terminate child process later
            break;
        
        prime.value += i;
        prime.counter--;
        if (prime.counter == 0) {
            // printf("fuck22\n");
            fflush(stdout);
            if (!sem_wait(parent_sem)) {
                // printf("------------------------------------------------------ %d %d %d\n", prime.value, prime.first_value, prime.first_child);
                fflush(stdout);
                parent_shm->current_index++;
                parent_shm->number_of_tasks++;
                int cur = parent_shm->current_index;
                parent_shm->tasks[cur] = prime;
                sem_post(parent_sem);
            }

        } else {
            int next_child = (i + 1) % UNIT_COUNT;

            char next_child_sem_char[2];
            sprintf(next_child_sem_char, "%d", next_child * 10);

            key_t key = ftok("/tmp", next_child);

            int next_child_shm_id = shmget(key, sizeof(shm_t), 0666);
            shm_t *next_child_shm = shmat(next_child_shm_id, NULL, 0);

            sem_t *next_child_sem = sem_open(next_child_sem_char, 0);

            if (!sem_wait(next_child_sem)) {
                next_child_shm->current_index++;
                next_child_shm->number_of_tasks++;
                int cur = next_child_shm->current_index;
                next_child_shm->tasks[cur] = prime;
                // printf("%d next child with %d task and parent has %d task\n",next_child, next_child_shm->number_of_tasks, parent_shm->number_of_tasks);
                fflush(stdout);
                sem_post(next_child_sem);
            }

            shmdt(next_child_shm);
        }
    }
    
}

int main() {

    prime_t *primes = first_n_prime(N);

    key_t parent_key = ftok("/tmp", PARENT_KEY);

    if (parent_key == -1) {
        printf("Oh dear, something went wrong with ftok()! %s\n", strerror(errno));
        fflush(stdout);
    }

    parent_shmid = shmget(parent_key, sizeof(shm_t), 0666|IPC_CREAT);

    if (parent_shmid == -1) {
        printf("Oh dear, something went wrong with shmget()! %s\n", strerror(errno));
        fflush(stdout);
    }

    shm_t *smh = shmat(parent_shmid, NULL, 0);

    sem_t *parent_sem = sem_open(PARENT_SEM, O_CREAT, ACCESSPERMS, 1);

    sem_t *child_sem[UNIT_COUNT];

    smh->current_index = -1;
    smh->number_of_tasks = 0;

    for (int i = 0; i < UNIT_COUNT; i++) {

        key_t key = ftok("/tmp", i);

        if (key == -1) {
            printf("Oh dear, something went wrong with ftok()! %s\n", strerror(errno));
            fflush(stdout);
        }

        child_shmid[i] = shmget(key, sizeof(shm_t), 0666|IPC_CREAT);

        if (child_shmid[i] == -1) {
            printf("Oh dear, something went wrong with shmget()! %s %d\n", strerror(errno), i);
            fflush(stdout);
        }

        child_shm[i] = shmat(child_shmid[i], NULL, 0);
        child_shm[i]->current_index = -1;
        child_shm[i]->number_of_tasks = 0;

        char child_sem_char[2];
        sprintf(child_sem_char, "%d", i * 10);

        child_sem[i] = sem_open(child_sem_char, O_CREAT, ACCESSPERMS, 0);

        sem_post(child_sem[i]);
        int semval;
        sem_getvalue(child_sem[i], &semval);
        printf("%d\n", semval);
        fflush(stdout);
    }
    

    for (int i = 0; i < UNIT_COUNT; i++) {
        if (fork() == 0) {
            child_worker(i);
            exit(1);
        }
    }

    for (int i = 0; i < N; i++) {
        int child = primes[i].value % UNIT_COUNT;
        primes[i].first_child = child;


        child_shm[child]->current_index++;
        child_shm[child]->number_of_tasks++;
        int current_index = child_shm[child]->current_index;
        child_shm[child]->tasks[current_index] = primes[i];
    }

    // for (int i = 0; i < UNIT_COUNT; i++) {
    //     printf("%d\n", child_shm[i]->number_of_tasks);
    //     fflush(stdout);
    // }
    
    
    for (int i = 0; i < N; i++) {
        prime_t prime;
        fflush(stdout);
        if (smh->number_of_tasks == 0) {
            i--;
            continue;
        }
        if (!sem_wait(parent_sem)) {
            int cur = smh->current_index;
            prime = smh->tasks[0];
            for (int i = 0; i < (smh->number_of_tasks - 1); i++) {
                smh->tasks[i] = smh->tasks[i + 1];
            }
            smh->current_index--;
            smh->number_of_tasks--;
            sem_post(parent_sem);
        }
        printf("got value:%d and first child is:%d and first value is:%d\n", prime.value, prime.first_child, prime.first_value);
    }
    fflush(stdout);

    for (int i = 0; i < UNIT_COUNT; i++) {
        prime_t terminate;
        terminate.value = -1;
        terminate.counter = -10;
    }
    
    sleep(15);
    for (int i = 0; i < UNIT_COUNT; i++) {
        wait(NULL);
    }
    
    return 0;
}
