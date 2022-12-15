
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

#define N 100
#define UNIT_COUNT 8
#define SHMN "HW41Os"
#define SIZE 4096

int parent_shm_fd;

int c[UNIT_COUNT][2];

int child_shm_fds[UNIT_COUNT];
void *ptr[UNIT_COUNT];
void *parent_ptr;

typedef struct {
    int value;           // value of the prime number
    int counter;         // a counter to decrement (at the start counter = value)
    int first_value;     // this is for debug
    int first_child;     // this is for debug
} prime_t;

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
    // int child_shm_fd = shm_open((const char *)i, O_CREAT | O_RDWR, 0666);
    // ftruncate(child_shm_fd, SIZE);
    // void *child_ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, child_shm_fd, 0);

    sleep(5);
    int n = 0;
    while (1) {
        n++;
        if (n == 100) break;
        prime_t prime;
        memcpy(&prime, ptr[i], sizeof(prime_t));
        ptr[i] += sizeof(prime_t);
        printf("%d %d %d %d\n", prime.value, prime.counter, prime.first_child, prime.first_value);
        fflush(stdout);
        if (prime.value == -1 && prime.counter == -10) //since prime number cannot be -1 i use this condition to terminate child process later
            break;
        
        prime.value += i;
        prime.counter--;
        if (prime.counter == 0) {
            // write(pr[1], &prime, sizeof(prime_t));
            memcpy(parent_ptr, &prime, sizeof(prime_t));
            parent_ptr += sizeof(prime_t);
        } else {
            // int next_child = (i + 1) % UNIT_COUNT;
            // memcpy(ptr[next_child], &prime, sizeof(prime_t));
            // ptr[next_child] += sizeof(prime_t);
            // write(c[next_child][1], &prime, sizeof(prime_t));
        }
    }
    
}

int main() {

    prime_t *primes = first_n_prime(N);

    int sizes[UNIT_COUNT];
    memset(sizes, 0, UNIT_COUNT * sizeof(int));

    parent_shm_fd = shm_open(SHMN, O_CREAT | O_RDWR, 0666);
    ftruncate(parent_shm_fd, SIZE);
    parent_ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, parent_shm_fd, 0);

    for (int i = 0; i < UNIT_COUNT; i++) {
        char ch[2];
        sprintf(ch, "%d", i);
        child_shm_fds[i] = shm_open(ch, O_CREAT | O_RDWR, 0666);
        ftruncate(child_shm_fds[i], SIZE);
        ptr[i] = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, child_shm_fds[i], 0);
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
        memcpy((ptr[child] + (sizes[child]*sizeof(prime_t))), &primes[i], sizeof(prime_t));
        printf("%d %d %d %d\n", primes[i].value, primes[i].counter, primes[i].first_child, primes[i].first_value);
        fflush(stdout);
        sizes[child] += 1;
    }
    
    // for (int i = 0; i < N; i++) {
    //     prime_t prime;
    //     memcpy(&prime, parent_ptr, sizeof(prime_t));
    //     parent_ptr += sizeof(prime_t);
    //     printf("got value:%d and first child is:%d and first value is:%d\n", prime.value, prime.first_child, prime.first_value);
    // }

    // fflush(stdout);

    // for (int i = 0; i < UNIT_COUNT; i++) {
    //     prime_t terminate;
    //     terminate.value = -1;
    //     terminate.counter = -10;
    //     write(c[i][1], &terminate, sizeof(prime_t));
    // }
    
    for (int i = 0; i < UNIT_COUNT; i++) {
        wait(NULL);
    }
    
    return 0;
}
