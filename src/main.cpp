#include "../include/search.hpp"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <thread>
#include <vector>
int main(){
    std::vector<std::thread> threads;
    search StartSearch;
    std::map<int, std::string> id_to_file;
    while (true) {
    std::string filename;
    int id = 0;
    while (true) {
          std::cout << "Input filename or type 'continue' to proceed or 'exit' to end:" << std::endl;
          std::getline(std::cin, filename);
          if (filename == "continue") {
              break;
          }else if (filename == "exit") {
              for(auto&t : threads){
                  if(t.joinable()){
                  t.join();
                  }
              }
              return 0;
          }
          std::promise<bool> result;
          auto future = result.get_future();
          threads.emplace_back(&search::TokenizerFile, &StartSearch, filename, id,std::move(result));
          if (future.get()) {
               id_to_file.insert({id, filename});
               id++;
           } else {
               std::cerr << "Skipping invalid file probably due to failed opening(file doesnt exist): " << filename << std::endl;
           }
        }
    for(auto&t : threads){
        if(t.joinable()){
        t.join();
        }
    }
    for(auto& [file_id,filename] : id_to_file){
        std::cout << file_id << " : " << filename << std::endl;

    }
    if(id_to_file.empty()){
        std::cerr << "something went wrong in threads" << std::endl;
    }
    std::cout << "Search words " << std::endl;
    std::thread input(&search::Input,StartSearch);
    std::thread realTimeCompilation(&search::realTime_search,StartSearch);

    std::cout << "Searching ...";
    std::vector<std::string> query = StartSearch.tokenizerInput(input);
    StartSearch.Matching(query);
    const std::pair<std::vector<std::string>, std::unordered_map<int, std::set<std::pair<int,int>>>>& map = StartSearch.getIntersection();
    if(map.second.empty()){
        std::cout << "Doesnt have intersection" << std::endl;
    }else {
        std::cout << "All words are in this files :" << std::endl;
        StartSearch.printMap(map,id_to_file);
    }
     const std::pair<std::vector<std::string>, std::unordered_map<int, std::set<std::pair<int,int>>>>& map1 = StartSearch.getUnion();
    if(map1.second.empty()){
        std::cout << "Doesnt have union :" << std::endl;
    }else {
        std::cout << "Some of the words also are in this files " << std::endl;
        StartSearch.printMap(map1,id_to_file);
    }
    const std::pair<std::vector<std::string>, std::unordered_map<int, std::set<std::pair<int,int>>>>& map2 = StartSearch.getWholeSentence();
   if(map2.second.empty()){
       std::cout << "doesnt have  whole phrase" << std::endl;
   }else {
       std::cout << "Whole phrase is here :" << std::endl;
       StartSearch.printMap(map2,id_to_file);
   }
}
}
