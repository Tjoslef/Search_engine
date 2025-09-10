#include "../include/search.hpp"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>
bool isTextFile(const std::string& filename) {
    if (filename.size() < 4) return false; // ".txt" is 4 chars including the dot
    std::string ext = filename.substr(filename.size() - 4);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".txt";
}
bool isPDF(const std::string& filename) {
    if (filename.size() < 4) return false; // ".txt" is 4 chars including the dot
    std::string ext = filename.substr(filename.size() - 4);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".pdf";
}
std::vector<Token> search::tokenizer(const std::string& input) {
    if (input.empty()) {
        std::cerr << "zero size string in tokenizer" << std::endl;
        return {};
    }

    size_t line_counter = 1;
    std::vector<Token> tokens;
    std::string word;
    size_t i = 0;

    while (i < input.size()) {
        char c = input[i];
        if (c == '\n') {
            line_counter++;
            i++;
            continue;
        }
        if (c == ' ' || c == ',') {
            i++;
            continue;
        }
        size_t start = i;
        while (i < input.size() && input[i] != ' ' && input[i] != ',' && input[i] != '\n') {
            i++;
        }

        word = input.substr(start, i - start);
        std::transform(word.begin(), word.end(), word.begin(),
                       [](unsigned char ch){ return std::tolower(ch); });

        if (word != "a" && word != "the" && word != "in") {
            tokens.push_back({word, static_cast<int>(line_counter), static_cast<int>(start)});
        }
    }

    return tokens;
}
std::vector<std::string> search::tokenizerInput(std::string input){
if(input.size() == 0){
    std::cerr << "zero size string in tokenizer" << std::endl;
}
bool inWord =true;
std::vector<std::string> tokens;
int i= 0;
std::string word;
while (i < input.size()) {
    while (i < input.size() && (input[i] == ' ' || input[i] == ',')) ++i;
    size_t start = i;
    while (i < input.size() && (input[i] != ' ' && input[i] != ',')) ++i;
    if (start < i){
        word = input.substr(start, i - start);
        std::transform(word.begin(),word.end(),word.begin(),[](unsigned char c){return std::tolower(c);});
        if (word != "a" && word != "the" && word != "in") {
                        tokens.push_back(word);
        }
    }
}
return tokens;
}
std::unordered_map<std::string,std::vector<Posting>> search::inverted_map;
void search::TokenizerFile(const std::string &filename, int id, std::promise<bool> resultFile) {
    std::vector<Token> result;
     std::string content ;
    if (isTextFile(filename)) {
    std::ifstream myfile(filename);
    if (!myfile.is_open()) {
          std::cerr << "Failed to open " << filename << std::endl;
          resultFile.set_value(false);
          return;
      }else {
          resultFile.set_value(true);
      }

    std::stringstream buffer;
    buffer << myfile.rdbuf();
    content = buffer.str();
    }else if (isPDF(filename)) {
        std::unique_ptr<poppler::document> doc(
            poppler::document::load_from_file(filename)
        );
        if (!doc) {
            std::cerr << "Failed to open " << filename << std::endl;
            resultFile.set_value(false);
            return;
        }else {
            resultFile.set_value(true);
        }

        for (int i = 0; i < doc->pages(); ++i) {
            std::unique_ptr<poppler::page> p(doc->create_page(i));
            if (p) {
                content += p->text().to_latin1();
            }
        }
    }else {
        std::cerr << "Failed to open " << filename << std::endl;
        resultFile.set_value(false);
        return;
    }
    result = tokenizer(content);
    {
    std::lock_guard<std::mutex> lock(write_map);
    for (const auto& [word, line,position] : result) {
        auto& id_list = inverted_map[word];
        if (std::find_if(id_list.begin(), id_list.end(),
                         [id](const Posting& p) { return p.file_id == id; }) == id_list.end()) {
            id_list.push_back({id, line,position});
        }
    }
    }
}
void search::printMap(const std::pair<std::vector<std::string>, std::unordered_map<int, std::set<std::pair<int,int>>>>& map,
                   const std::map<int, std::string>& id_to_file) {
    for (const auto& [file_id, lines] : map.second) {
        auto it = id_to_file.find(file_id);
        std::string filename = (it != id_to_file.end()) ? it->second : "<unknown>";
        std::cout << "File: " << filename << " (ID " << file_id << ")" << std::endl;
        std::cout << " Words: ";
        for (const auto& word : map.first) {
            std::cout << word << " ";
        }
        std::cout << std::endl;
        for (const auto& [line, pos] : lines) {
            std::cout << "   Line: " << line << " Pos: " << pos << std::endl;
        }
    }
   }
