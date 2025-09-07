#include "../include/search.hpp"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <ostream>
#include <string>
#include <thread>
#include <vector>
int search::realTime_search(){
while (!start_search) {
    std::unique_lock<std::mutex> lock(queue_mutex);
    queue_cv.wait(lock,[&]{return !search_queue.empty();});
    std::string input = search_queue.front();
    search_queue.pop();
    lock.unlock();
    this->RealMatching(input);

    }
return 0;
}
