#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

struct data
{
    int flag;
    int arr[100];
    int size;
};

int main()
{
    struct data *shared_data = mmap(NULL, sizeof(struct data), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    shared_data->flag = 0;

    pid_t pid = fork();

    if(pid < 0) {
        perror("fork failed!\n");
        exit(1);
    }

    if(pid == 0) {
        while(!shared_data->flag);

        printf("Child prints data: \n");
        for(int i = 0; i < shared_data->size; ++i) {
            printf("%d ", shared_data->arr[i]);
        }
        printf("\n");
        exit(0);
    }
    else {
        printf("Parent Enters data: \n");
        shared_data->size = 10;
        for(int i = 0; i < shared_data->size; ++i) {
            shared_data->arr[i] = i + 1;
        }
        shared_data->flag = 1;

        wait(NULL);
    }

    munmap(shared_data, sizeof(struct data));

    return 0;
}