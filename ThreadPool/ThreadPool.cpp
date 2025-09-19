#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool{
public:
    ThreadPool(size_t n)
    {
        for(int i = 0; i < n; ++i) {
            threads.emplace_back([this](){
                while(true) {
                    std::function<void()> func;
                    {
                        std::unique_lock<std::mutex> lock(mtx);

                        cond.wait(lock, [this]{return !functions.empty() || stopped; } );
                        
                        if(stopped && functions.empty()) {
                            return;
                        }

                        func = functions.front();
                        functions.pop();
                    }
                    func();
                }
            });
            
        }
    }

    void submit(std::function<void(void)> f) {
        {
            std::unique_lock<std::mutex> lock(mtx);

            functions.push(f);
        }
        cond.notify_one();
    }

    void shutdown()
    {
        {
            std::unique_lock<std::mutex> lock(mtx);
            stopped = true;
        }
        cond.notify_all();

        for (int i = 0 ; i < threads.size(); ++i) {
            threads[i].join();
        }
    }

    ~ThreadPool() {
        shutdown();
    }

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void(void)>> functions;
    std::mutex mtx;
    std::condition_variable cond;
    bool stopped = false;
};

int main()
{
    ThreadPool pool(4);

    pool.submit([](){
        std::cout << "function 1" << std::endl;
    });

    pool.submit([](){
        std::cout << "function 2" << std::endl;
    });
    pool.submit([](){
        std::cout << "function 3" << std::endl;
    });
    pool.submit([](){
        std::cout << "function 4" << std::endl;
    });
    pool.submit([](){
        std::cout << "function 5" << std::endl;
    });
    

}