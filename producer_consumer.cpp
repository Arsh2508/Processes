#include <pthread.h>
#include <queue>
#include <string>
#include <iostream>

void *consumer(void *);
void *producer1(void *);
void *producer2(void *);

pthread_mutex_t mutex;
pthread_cond_t cond;
const int size = 5;

std::queue<std::string> data;


int main()
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_t th1, th2, th3;

    pthread_create(&th1, NULL, consumer, NULL);
    pthread_create(&th2, NULL, producer1, NULL);
    pthread_create(&th3, NULL, producer2, NULL);

    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    pthread_join(th3, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

void *producer1(void *)
{
    for(int i = 0; i < 10; ++i) {

        pthread_mutex_lock(&mutex);
        
        while(data.size() == size) {
            pthread_cond_wait(&cond, &mutex);
        }

        data.push("Producer 1");

        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
    }

    return nullptr;
}

void *producer2(void *)
{
    for(int i = 0; i < 10; ++i) {
        pthread_mutex_lock(&mutex);
        
        while(data.size() == size) {
            pthread_cond_wait(&cond, &mutex);
        }

        data.push("Producer 2");

        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
    }

    return nullptr;
}

void* consumer(void *){
    for(int i = 0; i < 20; ++i) {
        pthread_mutex_lock(&mutex);

        while(data.empty()) {
            pthread_cond_wait(&cond, &mutex);
        }

        std::cout << data.front() << "\n";
        data.pop();

        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
    }

    return nullptr;
}
