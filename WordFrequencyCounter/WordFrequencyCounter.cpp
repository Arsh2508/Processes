#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <fstream>
#include <future>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>

class QueueWrapper {
public:
    void push(const std::string &line) {
        std::unique_lock<std::mutex> lock(mtx);
        lines.push(line);
        cond.notify_one();
    }
    bool pop(std::string &line) {
        std::unique_lock<std::mutex> lock(mtx);
        cond.wait(lock, [this](){ return !lines.empty() || done; });
        
        if(done && lines.empty()) return false;

        line = lines.front();
        lines.pop();

        return true;
    }

    void set_done() {
        std::unique_lock<std::mutex> lock(mtx);
        done = true;
        cond.notify_all();
    }

private:
    std::queue<std::string> lines;
    std::mutex mtx;
    std::condition_variable cond;
    bool done = false;
};

void producer(QueueWrapper& wrap)
{
    std::ifstream infile("file.txt");
    if(!infile) {
        std::cerr << "Failed to open file\n";
        return;
    }

    std::string line;
    while(std::getline(infile, line)) {
        wrap.push(line);
    }

    wrap.set_done();
}

void consumer(QueueWrapper& wrap, std::promise<std::unordered_map<std::string, int>> prom) {
    std::unordered_map<std::string, int> freq;
    while (true) {
        std::string line;
        if (!wrap.pop(line)) break;

        std::string word;

        std::stringstream ss(line);
        while(ss >> word) {
            word.erase(std::remove_if(word.begin(), word.end(), [](char c){
                return std::ispunct(c);
            }), word.end());

            std::for_each(word.begin(), word.end(), [](char& c){
                c = std::tolower(c);
            });

            ++freq[word];
        }
        
    }
    prom.set_value(std::move(freq));
}

int main()
{
    std::promise<std::unordered_map<std::string, int>> prom1;
    std::promise<std::unordered_map<std::string, int>> prom2;
    std::future<std::unordered_map<std::string, int>> future1 = prom1.get_future();
    std::future<std::unordered_map<std::string, int>> future2 = prom2.get_future();

    QueueWrapper wrap;

    std::thread prod(producer, std::ref(wrap));
    std::thread cons1(consumer, std::ref(wrap), std::move(prom1));
    std::thread cons2(consumer, std::ref(wrap), std::move(prom2));
   
    prod.join();
    cons1.join();
    cons2.join();

    std::unordered_map<std::string, int> res1 = future1.get();
    std::unordered_map<std::string, int> res2 = future2.get();
    
    for(auto &[line, freq] : res2) {
        res1[line] += freq;
    }

    std::vector<std::pair<std::string, int>> vec(res1.begin(), res1.end());

    std::sort(vec.begin(), vec.end(), [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b){
        return a.second > b.second;
    });

    for(const auto &[str, freq] : vec) {
        std::cout << str << ": " << freq << std::endl;
    }
    
}