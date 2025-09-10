#include <cstddef>
#include "/home/tjoslef/Search engine/linenoise-ng/include/linenoise.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <ostream>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <sqlite3.h>
#include <future>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
struct Posting {
    int file_id;
    int line_number;
    int position;
    bool operator==(const Posting& other) const {
        return file_id == other.file_id && line_number == other.line_number && position == other.position;
    }
};
struct Token {
    std::string Token;
    int line_number;
    int position;
};
class search {
    public:
    bool getStatus(){
        return running;
    }
    void setStatus(bool change){
        start_search = change;
    }
    void updateResult(const std::string& res) {
        std::lock_guard<std::mutex> lock(result_mutex);
        latest_result = res;
    }

    std::string getResult() {
        std::lock_guard<std::mutex> lock(result_mutex);
        return latest_result;
    }
    static std::unordered_map<std::string, std::vector<Posting>> inverted_map;
  const std::pair<std::vector<std::string>, std::unordered_map<int, std::set<std::pair<int,int>>>>&getIntersection(){
       return intersection_map;
   }
  const std::pair<std::vector<std::string>, std::unordered_map<int, std::set<std::pair<int,int>>>>  &getUnion(){
       return result_union;
   }
   const std::pair<std::vector<std::string>, std::unordered_map<int, std::set<std::pair<int,int>>>>  &getWholeSentence(){
        return whole_sentece_map;
    }
   std::vector<Token> tokenizer(const std::string& input);
    std::ofstream fileOpen(std::string filename);
    void TokenizerFile(const std::string &filename,int id,std::promise<bool> resultFile);
    void Matching(std::vector<std::string> query);
    std::vector<std::string> tokenizerInput(std::string input);
    void printMap(const std::pair<std::vector<std::string>, std::unordered_map<int, std::set<std::pair<int,int>>>>& map,
                          const std::map<int, std::string>& id_to_file);

    void Debug(const std::pair<std::vector<std::string>, std::unordered_map<int, std::set<std::pair<int,int>>>>& map);
    int Input();
    int realTime_search();
    void RealMatching(std::string input);
    static void completionCallback(const char* prefix, linenoiseCompletions* lc);
    private:
  bool start_search;
  std::queue<std::string> search_queue;
  std::mutex queue_mutex;
  std::condition_variable queue_cv;
  std::atomic<bool> running{true};
  std::pair<std::vector<std::string>, std::unordered_map<int, std::set<std::pair<int,int>>>>  intersection_map;
  std::pair<std::vector<std::string>, std::unordered_map<int, std::set<std::pair<int,int>>>>  whole_sentece_map;
  std::pair<std::vector<std::string>, std::unordered_map<int, std::set<std::pair<int,int>>>>  result_union;
  std::mutex write_map;
  std::string latest_result;
  std::mutex result_mutex;
};
