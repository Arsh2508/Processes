#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main()
{
    char *shared = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(shared == MAP_FAILED) {
        perror("mmap failed!\n");
        exit(1);
    }

    pid_t pid = fork();

    if(pid < 0) {
        perror("fork failed!\n");
        exit(1);
    }

    if(pid == 0) {
        sleep(1);
        printf("Child reads: %s", shared);
        strcpy(shared, "Child changes text: \n");
    }

    else {
        strcpy(shared, "Hello from parent: \n");
        sleep(2);
        printf("Parent reads: %s", shared);
        wait(NULL);
    }

    munmap(shared, 4096);

    return 0;

}