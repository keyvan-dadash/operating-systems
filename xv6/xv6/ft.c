#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "x86.h"

struct proc_info {
    int pid;
    int memsize;            // in bytes
};

void loop() 
{
    for (long i = 0; i < 2000000000; i++)
    ;
    
}

void alloc(int i) 
{
    long *g = malloc(100000 * (i + 2) * sizeof(long));
    g[1] = 'c';
    loop();
    loop();
    loop();
    loop();
    loop();
    long *l = malloc(500000 * (i + 2) * sizeof(long));
    l[1] = 'c';
    loop();
    long *z = malloc(1000000 * (i + 2) * sizeof(long));
    z[1] = 'c';
    loop();
    long *k = malloc(8000000 * (i + 2) * sizeof(long));
    k[1] = 'c';
    loop();

    free(g);
    free(l);
    free(z);
    free(k);
}

int main() 
{
    for (int i = 0; i < 5; i++) {
        if (fork() > 0) {
            continue;
        } else {
            alloc(i);
            exit();
        }
    }
    loop();
    loop();
    void *ptr = malloc(sizeof(struct proc_info) * 100);
    int index = idk(ptr);
    printf(2, "%d\n", index);
    struct proc_info *p = (struct proc_info *)ptr;
    for (int i = 0; i < index; i++) {
        printf(2, "%d %d\n", (*(p + i)).pid, (*(p + i)).memsize);
    }

    for (int i = 0; i < 5; i++)
    {
        wait();
    }
    
}