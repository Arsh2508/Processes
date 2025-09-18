#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int is_prime(int n) 
{
    if (n <= 1) {
        return 0;
    }

    for(int i = 2; i <= n/2; ++i) {
        if(n % i == 0)
        {
            return 0;    
        }
    }
    return 1;
}

struct PrimeData{
    int *arr;
    int size;
};

void * primes(void *arg)
{
    int n = *(int *)arg;

    struct PrimeData *data = malloc(sizeof(struct PrimeData));
    data->arr = malloc(sizeof(int) * n);
    data->size = 0;

    for(int i = 2; i < n; ++i) {
        if(is_prime(i)) {
            data->arr[data->size++] = i;
        }
    }
    
    return data;
}

void * print(void *arg)
{
    struct PrimeData *data = (struct PrimeData *) arg;

    printf("Printing primes: \n");
    for(int i = 0; i < data->size; ++i) 
    {
        printf("%d ", data->arr[i]);
    }   
    printf("\n");

    free(data->arr);
    free(data);

    return NULL;
}

int main()
{
    pthread_t t1, t2;
    int N = 50;

    pthread_create(&t1, NULL, primes, &N);

    struct PrimeData *result;
    pthread_join(t1, (void **)&result);

    pthread_create(&t2, NULL, print, result);

    pthread_join(t2, NULL);

    return 0;

}