#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fstream>


void process_file(const std::string& fn) {
    std::ifstream infile(fn);
    if(!infile) {
        std::cerr << "Failed to open the file\n";
        exit(-1);
    }
    
    std::string res = fn.substr(0, fn.size() - 4) + "_result.json";
    std::ofstream outfile(res);
    if(!outfile) {
        std::cerr << "Failed to open the file\n";
        exit(-1);
    }

    int first, second;
    int total = 0;
    int errors = 0;
    int time_sum = 0;

    while(infile >> first >> second) {
        ++total;
        if(first == 500) {
            ++errors;
        }
        time_sum += second;
    }

    double avg_time = static_cast<double>(time_sum) / total;

    outfile << "{\n\t\"file\": \"" << fn 
            << "\",\n\t\"total_requests\": " << total << ",\n"
            << "\t\"http_500_errors\": " << errors << ",\n"
            << "\t\"avg_response_time\": " << avg_time << "\n}";
}

int main()
{
    std::string path = "./LogFiles";
   
    std::vector<std::string> files;
    
    for(const auto &entity : std::filesystem::directory_iterator(path)) {
        if(entity.is_regular_file()) {
            std::string fn = entity.path().filename().string();
            if(fn.substr(0, 3) == "log" && entity.path().extension() == ".txt") {
                files.push_back(path + "/" + fn);
            }
        }
    }

    for(int i = 0; i < files.size(); ++i) {
        pid_t pid = fork();
        if(pid < 0) {
            std::cerr << "Fork failed\n";
            return -1;
        }
        if(pid == 0) {
            process_file(files[i]);
            exit(0);
        }
        else {
            wait(nullptr);
        }
    }

    std::ofstream res_file("result.txt");
    if(!res_file) {
        std::cerr << "Failed to open the file\n";
        return -1;
    }

    int total_sum = 0;
    int error_sum = 0;
    double global_time = 0;

    for(int i = 0; i < files.size(); ++i) {
        std::string res = files[i].substr(0, files[i].size() - 4) + "_result.json";

        std::ifstream infile(res);
        if(!infile) {
            std::cerr << "Failed to open the file\n";
            return -1;
        }

        std::string token;
        int total, errors;
        double avg_time;
        
        while (infile >> token) {
            if (token.find("total_requests\":") != std::string::npos) {
                infile >> total;

                total_sum += total;
            }
            else if (token.find("http_500_errors\":") != std::string::npos) {
                infile >> errors;

                error_sum += errors;
            }
            else if (token.find("avg_response_time\":") != std::string::npos) {
                infile >> avg_time;

                global_time += avg_time;
            }
        }

    }


    res_file << "Aggregated Results:\n"
             << "Total requests:" << total_sum << "\n"
             << "Total 500 errors:" << error_sum << "\n"
             << "Global average response time:" << global_time/files.size() << "\n";

    return 0;

}