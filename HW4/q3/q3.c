#define _GNU_SOURCE
#include <stdio.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <wait.h>
#include <stdlib.h>

#define GIG3 3221225472

int main() 
{
    int fd[2];
    pipe(fd);
    if (fork() > 0) {


        void *ptr = malloc(sizeof(char) * GIG3);

        printf("%p\n", (void *)ptr);
        fflush(stdout);

        write(fd[1], &ptr, sizeof(void *));

        wait(NULL);
    } else {
        ssize_t nwrite;
        pid_t parent_pid = getppid();

        void *ptr = malloc(GIG3 * sizeof(char));

        struct iovec *local = malloc(sizeof(struct iovec) * 1024);
        struct iovec *remote = malloc(sizeof(struct iovec) * 1024);

        for (int i = 0; i < 1024; i++) {
            memset(&ptr[i * (GIG3 / 1024)], '0' + i, (GIG3 / 1024));
            local[i].iov_base = &ptr[i * (GIG3 / 1024)];
            local[i].iov_len = (GIG3 / 1024) * sizeof(char);
        }

        void *address;
        int s = read(fd[0], &address, sizeof(char *));

        for (size_t i = 0; i < 1024; i++) {
            fflush(stdout);
            remote[i].iov_base = (char *)address + (i * (GIG3 / 1024));
            remote[i].iov_len = (GIG3 / 1024) * sizeof(char);
        }

        if (s == -1) {
            printf("bad! %s\n", strerror(errno));
        }

        printf("%p\n", address);
        fflush(stdout);

        for (size_t i = 0; i < 1024; i++) {
            nwrite += process_vm_writev(parent_pid, &local[i], 1, &remote[i], 1, 0);
            if (errno == 1) {
                printf("bad! %ld %s %d\n", nwrite, strerror(errno), errno);
                fflush(stdout);
                exit(-1);
            }
        }

        printf("success! and %ld byte writed", nwrite);
        fflush(stdout);
    }
}