#define _GNU_SOURCE 
#include <fcntl.h>
#include <stdint.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sched.h>


double calculate_trials(long page_size, int num_pages, int trials)
{

    int *arr = (int*) calloc((int)((page_size * num_pages) / sizeof(int)), sizeof(int));

    long jump = page_size / (long)sizeof(int);

    struct timeval start,end;
    int time1 = gettimeofday(&start, NULL);

    for (int j = 0; j < trials; j++) {
        for (int i = 0; i < num_pages * jump; i += jump) {
            arr[i] += 1;
        }
    }
    
    int time2 = gettimeofday(&end, NULL); 

    if (time1 == -1 || time2 == -1) {
        exit(EXIT_FAILURE);
    }
    uint64_t trial_time = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
    free(arr);
    return trial_time / (double) (num_pages * trials);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        exit(EXIT_FAILURE);
    }

    // cpu_set_t cpuset;

    // CPU_ZERO(&cpuset);
    // CPU_SET(0, &cpuset);
    // if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset)) {
    //     printf("ops");
    //     fflush(stdout);
    //     exit(EXIT_FAILURE);
    // }

    // pthread_t thread = pthread_self();
    // CPU_ZERO(&cpuset);
    // CPU_SET(0, &cpuset);
    // if (pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset)) {
    //     printf("ops");
    //     fflush(stdout);
    //     exit(EXIT_FAILURE);
    // }
    
    int page_size = getpagesize();
    long pages = strtol(argv[1], NULL, 10);
    long trials = strtol(argv[2], NULL, 10);

    if (!pages || !trials) {
        printf("ops");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    double total = calculate_trials(page_size, pages, trials);
    printf("%ld  %ld  %f\n", pages, trials, total);
    return 0;
}