
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

// #define DOWN 1000000000
// #define UP   1100000000
#define DOWN 1
#define UP   20
#define UNIT_COUNT 8

#define PARENT_KEY 2222

#define PARENT_SEM "parent"

int pw[2];
int pr[2];

int c[UNIT_COUNT][2];

typedef struct {
    long long int value;           // value of the prime number
} perfect_t;

typedef struct {
    int isEmpty;
} child_t;

typedef struct {
    int isEmpty;
    int filled;
    perfect_t tasks;
} shm_t;

typedef struct  {
    int current;
    perfect_t tasks[10000];
} parent_t;

int parent_shmid;
int child_shmid[UNIT_COUNT];

shm_t *child_shm[UNIT_COUNT];

shm_t child[UNIT_COUNT];

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
    char child_sem_char[2];
    sprintf(child_sem_char, "%d", i * 10);

    key_t key = ftok("/tmp", i);

    int child_shmid_l = shmget(key, sizeof(shm_t), 0666);
    shm_t *child_shm_l = shmat(child_shmid_l, NULL, 0);

    sem_t *child = sem_open(child_sem_char, 0);

    //////////////////
    key_t parent_key = ftok("/tmp", PARENT_KEY);

    int parent_shmid = shmget(parent_key, sizeof(parent_t), 0666);
    parent_t *parent_shm = shmat(parent_shmid, NULL, 0);

    sem_t *parent_sem = sem_open(PARENT_SEM, 0);


    while (1) {
        if (child_shm_l->filled == 0) {
            sleep(1);
            // printf("%d %d %d %d\n", child_shm_l->isEmpty, child_shm_l->filled, child_shmid_l, child_shmid[i]);
            // fflush(stdout);
            continue;
        }
        
        perfect_t perfect = child_shm_l->tasks;

        int semval;
        sem_getvalue(child, &semval);
        // printf("%lld %d\n", perfect.value, semval);
        fflush(stdout);

        if (perfect.value == -1) //since prime number cannot be -1 i use this condition to terminate child process later
            break;
        
        if (isPerfect(perfect.value)) {
            if (!sem_wait(parent_sem)) {
                // printf("------------------------------------------------------ %d %d %d\n", prime.value, prime.first_value, prime.first_child);
                fflush(stdout);
                parent_shm->current++;
                int cur = parent_shm->current;
                parent_shm->tasks[cur] = perfect;
                sem_post(parent_sem);
            }
        }
        if (!sem_wait(child)) {           
            child_shm_l->isEmpty = 1;
            child_shm_l->filled = 0;
            sem_post(child);
        }
    }
}

int main() {

    perfect_t *perfects = first_n();

    for (int i = 0; i < UNIT_COUNT; i++) {
        child[i].isEmpty = 1;
    }
    


    for (int i = 0; i < UNIT_COUNT; i++) {
        if (pipe(c[i]) < 0) exit(1);
    }

    key_t parent_key = ftok("/tmp", PARENT_KEY);

    if (parent_key == -1) {
        printf("Oh dear, something went wrong with ftok()! %s\n", strerror(errno));
        fflush(stdout);
    }

    parent_shmid = shmget(parent_key, sizeof(parent_t), 0666|IPC_CREAT);

    if (parent_shmid == -1) {
        printf("Oh dear, something went wrong with shmget()! %s\n", strerror(errno));
        fflush(stdout);
    }

    parent_t *smh = shmat(parent_shmid, NULL, 0);

    sem_t *parent_sem = sem_open(PARENT_SEM, O_CREAT, ACCESSPERMS, 1);

    sem_t *child_sem[UNIT_COUNT];

    smh->current = -1;

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
        child_shm[i]->filled = 0;
        child_shm[i]->isEmpty = 1;

        char child_sem_char[2];
        sprintf(child_sem_char, "%d", i * 10);

        child_sem[i] = sem_open(child_sem_char, O_CREAT, ACCESSPERMS, 0);

        sem_post(child_sem[i]);
        int semval;
        sem_getvalue(child_sem[i], &semval);
        fflush(stdout);
    }


    for (int i = 0; i < UNIT_COUNT; i++) {
        if (fork() == 0) {
            child_worker(i);
            exit(1);
        }
    }

    sleep(3);
    

    for (int i = 0; i < (UP - DOWN); i++) {
        int childC = -1;
        for (int k = 0; k < UNIT_COUNT; k++) {
            if (child_shm[k]->isEmpty == 1) {
                childC = k;
                break;
            }
        }
        if (childC == -1) {
            sleep(1);
            i--;
            continue;
        }

        // printf("%d\n", childC);
        fflush(stdout);

        if (!sem_wait(child_sem[childC])) {      
            child_shm[childC]->isEmpty = 0;
            child_shm[childC]->filled = 1;
            child_shm[childC]->tasks = perfects[i];
            sem_post(child_sem[childC]);
        }
    }

    printf("finished\n");
    fflush(stdout);

    while (1) {
        perfect_t perfect;
        if (!sem_wait(parent_sem)) {

            int cur = smh->current;
            if (cur <= 0) {
                sem_post(parent_sem);
                sleep(1);
                continue;
            }
            perfect = smh->tasks[0];
            for (int i = 0; i < smh->current; i++) {
                smh->tasks[i] = smh->tasks[i + 1];
            }
            printf("got value:%lld\n", perfect.value);
            smh->current--;
            int finish = 1;
            for (int i = 0; i < UNIT_COUNT; i++) {
                if (child_shm[i]->isEmpty == 0) {
                    finish = 0;
                    break;
                }
            }
            if (finish) {
                sem_post(parent_sem);
                break;
            } else {
                sem_post(parent_sem);
                continue;
            }
        }
        printf("got value:%lld\n", perfect.value);
        fflush(stdout);
    }

    for (int i = 0; i < UNIT_COUNT; i++) {
        perfect_t terminate;
        terminate.value = -1;
        if (!sem_wait(child_sem[i])) {      
            child_shm[i]->isEmpty = 0;
            child_shm[i]->filled = 1;
            child_shm[i]->tasks = perfects[i];
            sem_post(child_sem[i]);
        }
    }
    
    for (int i = 0; i < UNIT_COUNT; i++) {
        wait(NULL);
    }
    
    return 0;
}