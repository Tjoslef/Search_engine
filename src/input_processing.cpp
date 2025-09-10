#include "../include/search.hpp"
#include <cstdio>
#include <mutex>
#include <shared_mutex>
int search::Input() {
    // Register your member function as callback
    linenoiseSetCompletionCallback(&search::completionCallback);

        char* line = linenoise("search> ");

        std::string input(line);
        free(line);

        this->latest_result = input;
    return 0;
}
