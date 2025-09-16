#include <iostream>
#include <pthread.h>
#include <string>
#include <queue>

void *consumer(void *arg);
void *producer1(void *arg);
void *producer2(void *);

pthread_mutex_t mutex;
pthread_cond_t cond;

const int size = 5;

std::queue<std::string> data;


int main() 
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_t t1, t2, t3;

    int a, b;
    std::cout << "Enter producer1 and producer2 loop count: \n";
    std::cin >> a >> b;
    int c = a + b;

    pthread_create(&t1, NULL, producer1, &a);
    pthread_create(&t2, NULL, producer2, &b);
    pthread_create(&t3, NULL, consumer, &c);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

void *producer1(void *arg)
{
    int m = *(int *)arg;
    for(int i = 0; i < m; ++i) {
        pthread_mutex_lock(&mutex);

        while(data.size() == size) {
            pthread_cond_wait(&cond, &mutex);
        }

        std::string msg = "Producer 1: message " + std::to_string(i);
        data.push(msg);

        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
    }
    return nullptr;
}

void *producer2(void *arg) 
{
    int m = *(int *)arg;
    for(int i = 0; i < m; ++i) {
        pthread_mutex_lock(&mutex);

        while(data.size() == size) {
            pthread_cond_wait(&cond, &mutex);
        }

        std::string msg = "Producer 2: message " + std::to_string(i);
        data.push(msg);

        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
    }
    return nullptr;
}

void *consumer(void * arg) 
{
    int m = *(int *)arg;

    for(int i = 0; i < m; ++i) {
        pthread_mutex_lock(&mutex);

        while(data.empty()) {
            pthread_cond_wait(&cond, &mutex);
        }

        std::cout << data.front() << std::endl;
        data.pop();

        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);

    }
    return nullptr;
}