#include "../include/search.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <sqlite3.h>
#include <string>
#include <unordered_map>
#include <vector>
int levenshteinDistance(const std::string& s1, const std::string& s2) {
    size_t n = s1.size();
    size_t m = s2.size();

    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1));

    for (size_t i = 0; i <= n; i++) dp[i][0] = i;
    for (size_t j = 0; j <= m; j++) dp[0][j] = j;

    for (size_t i = 1; i <= n; i++) {
        for (size_t j = 1; j <= m; j++) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({
                dp[i - 1][j] + 1,
                dp[i][j - 1] + 1,
                dp[i - 1][j - 1] + cost
            });
        }
    }

    return dp[n][m];
}
void search::Matching(std::vector<std::string> query){
    std::vector<std::vector<Posting>> docID;
    std::vector<std::pair<std::string, std::unordered_map<int, std::set<std::pair<int,int>>>>> result_maps;

    for (auto& token : query) {
        for (auto&[name,map]: inverted_map){
        if (abs((int)token.size() - (int)name.size()) > 1) continue;
        std::unordered_map<int, std::set<std::pair<int,int>>>file_to_lines;
        int lenght = levenshteinDistance(token, name);
        for (auto &post : map) {
            if(lenght <= 2){
                file_to_lines[post.file_id].insert(std::make_pair(post.line_number,post.position));
            }
        }
        if (!file_to_lines.empty()) {
            result_maps.emplace_back(name, std::move(file_to_lines));
        }
    }
}
for (const auto& par : result_maps) {
    std::cout << "Token: " << par.first << std::endl;

    for (const auto& [file_id, positions] : par.second) {
        std::cout << "  File ID: " << file_id << std::endl;

        for (const auto& [line, pos] : positions) {
            std::cout << "    Line: " << line << "  Position: " << pos << std::endl;
        }
    }
}
   if(!result_maps.empty()){
       intersection_map.second = result_maps[0].second;
       intersection_map.first.push_back(result_maps[0].first);
       for(int i = 1; i < result_maps.size(); ++i){
           std::pair<std::vector<std::string>, std::unordered_map<int, std::set<std::pair<int,int>>>> temp;
           for(auto& [file_id,lines_number] : intersection_map.second){
               auto it = result_maps[i].second.find(file_id);
               if(it != result_maps[i].second.end()){
                std::set<std::pair<int,int>> merged = lines_number;
                  merged.insert(it->second.begin(), it->second.end());
                  for(auto& ex_line : merged){
                  std::cout << ex_line.first << std::endl;
                  }
                  temp.second[file_id] = std::move(merged);
               }
           }
           temp.first = intersection_map.first;
           temp.first.push_back(result_maps[i].first);
           intersection_map = std::move(temp);
        if (intersection_map.second.empty()) break;
       }
   }else {
       std::cout << "nothing in result_map" << std::endl;
   }
   for (const auto &[file_id,lines_number] : intersection_map.second) {
       std::vector<int> token_position;
       for (const auto&temp : lines_number) {
           token_position.push_back(temp.second);
       }
        if (token_position.size() < intersection_map.first.size()){
            continue;
        }

       std::sort(token_position.begin(),token_position.end());
       int first = token_position.front();
       int last  = token_position.back();
       int len_first = intersection_map.first.front().size();
       int len_last  = intersection_map.first.back().size();

       int expected = 0;
       for (const auto& word : intersection_map.first)
           expected += word.size();
       expected += (int)intersection_map.first.size() - 1;
       int actual = (last + len_last) - first;
        if (expected == actual) {
            whole_sentece_map.second.insert({file_id,lines_number});
            whole_sentece_map.first = std::move(intersection_map.first);
        }
   }
  std::pair<std::vector<std::string>, std::unordered_map<int, std::set<std::pair<int,int>>>> union_result;
   if (!result_maps.empty()) {
       union_result.second = result_maps[0].second;
       union_result.first.push_back(result_maps[0].first);
       for (size_t i = 1; i < result_maps.size(); ++i) {
           for (const auto& [file_id, lines] : result_maps[i].second) {
               auto& existing_lines = union_result.second[file_id];
               existing_lines.insert(lines.begin(), lines.end());
           }
           union_result.first.push_back(result_maps[i].first);
       }
   }

   for(auto &[file_id, lines] : union_result.second){
       if(intersection_map.second.find(file_id) == intersection_map.second.end()){
           result_union.second.insert({file_id,lines});
       }
   }

}
