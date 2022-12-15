
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

int pw[2];
int pr[2];

int c[UNIT_COUNT][2];

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
    while (1) {
        prime_t prime;
        read(c[i][0], &prime, sizeof(prime_t));
        if (prime.value == -1 && prime.counter == -10) //since prime number cannot be -1 i use this condition to terminate child process later
            break;
        
        prime.value += i;
        prime.counter--;
        if (prime.counter == 0) {
            write(pr[1], &prime, sizeof(prime_t));
        } else {
            int next_child = (i + 1) % UNIT_COUNT;
            write(c[next_child][1], &prime, sizeof(prime_t));
        }
    }
    
}

int main() {

    prime_t *primes = first_n_prime(N);

    if (pipe(pw) < 0) exit(1);
    if (pipe(pr) < 0) exit(1);


    for (int i = 0; i < UNIT_COUNT; i++) {
        if (pipe(c[i]) < 0) exit(1);
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
        write(c[child][1], &primes[i], sizeof(prime_t));
    }
    
    for (int i = 0; i < N; i++) {
        prime_t prime;
        read(pr[0], &prime, sizeof(prime_t));
        printf("got value:%d and first child is:%d and first value is:%d\n", prime.value, prime.first_child, prime.first_value);
    }
    fflush(stdout);

    for (int i = 0; i < UNIT_COUNT; i++) {
        prime_t terminate;
        terminate.value = -1;
        terminate.counter = -10;
        write(c[i][1], &terminate, sizeof(prime_t));
    }
    
    for (int i = 0; i < UNIT_COUNT; i++) {
        wait(NULL);
    }
    
    return 0;
}
