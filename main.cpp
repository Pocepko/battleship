#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "trie.hpp"

bool is_letter(char ch) {
  return (('A' <= ch) and (ch <= 'Z')) or (('a' <= ch) and (ch <= 'z'));
}

std::vector<std::string> line_to_words(std::string &line) {
  std::vector<std::string> words;

  std::string word;
  for (int i = 0; i < line.size(); i++) {
    if (is_letter(line[i])) {
      word += line[i];
    } else if (word.size() > 0) {
      words.push_back(word);
      word = "";
    }
  }

  if (word.size() > 0) {
    words.push_back(word);
  }

  return words;
}

int main() {
  std::cout << "Print \"help\" to learn how to use this program" << std::endl;

  Trie trie;

  std::pair<bool, int> last_word_inf;
  std::string last_word;
  std::string input;
  while (getline(std::cin, input)) {
    if (input == "help") {
      std::string line;
      std::ifstream in("help.txt");
      if (in.is_open()) {
          while (std::getline(in, line)) {
              std::cout << line << std::endl;
          }
      }
      in.close(); 
    } else if (input == "add text") {
      std::string line;
      getline(std::cin, line);

      std::vector<std::string> words = line_to_words(line);

      for (int i = 0; i < words.size(); i++) {
        trie.add_word(words[i]);
      }
    } else if (input == "complete") {
      getline(std::cin, input);
      //std::cin >> input;
      last_word = input;
      last_word_inf = trie.complete_prefix(input);
      std::cout << input << std::endl;
      if (last_word_inf.first) {
        std::cout << "there is ambiguity" << std::endl;
      }
    } else if (input == "add letters") {
      getline(std::cin, input);
      last_word += input;
      last_word_inf = trie.complete_prefix(last_word_inf.second, last_word, input);
      std::cout << last_word << std::endl;
    } else {
      std::cout << "unknown command" << input << std::endl;
    }
  }

  return 0;
}
