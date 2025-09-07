#include "../include/search.hpp"
#include <cstdio>
#include <mutex>
#include <shared_mutex>
int search::Input(){
    std::string input;
    while (start_search == false) {
        char inputC;
        inputC = std::getchar();
        input.push_back(inputC);
        if(input.size() >= 2){
        std::lock_guard<std::mutex> lock(queue_mutex);
        search_queue.push(input);
        queue_cv.notify_one();
        }
    }
    return 0;
}
