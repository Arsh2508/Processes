#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct array
{
    int *data;
    unsigned int size;
};

void * ThreadFunc(void * arg) {
    struct array *arr = (struct array*)arg;
  
    int sum = 0;
    for(int i = 0; i < arr->size; ++i) {
        sum += arr->data[i];
    }
    int * result = malloc(sizeof(int));
    *result = sum;
    return result;
}

int main()
{
    pthread_t th;

    int arr[] = {1, 2, 3, 4, 5, 6};
    struct array arg;
    arg.data = arr;
    arg.size = 6;
    
    pthread_create(&th, NULL, ThreadFunc, &arg);

    int *sum;

    pthread_join(th, (void **)&sum);

    printf("sum of array = %d\n", *sum);

    free(sum);
    return 0;

}