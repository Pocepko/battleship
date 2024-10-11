const int alphabet_size = 26;

class Node {
 public:
  bool contains_char(char ch) {
    return is_neighbour[char_to_index(ch)];
  }

  int get_neighbour_index(char ch) {
    return neighbour_index[char_to_index(ch)];
  }

  void add_char(char ch, int ind) {
    neighbours_list.push_back(ch);
    is_neighbour[char_to_index(ch)] = true;
    neighbour_index[char_to_index(ch)] = ind;
  }

  int get_words_at_node() {
    return words_at_node;
  }

  char get_max_char() {
    return max_char_in_subtree;
  }

  char get_max_char_index() {
    return neighbour_index[char_to_index(max_char_in_subtree)];
  }

  int get_max() {
    return max_in_subtree;
  }
  
  bool get_is_max_in_subtree() {
    return is_max_in_subtree;
  }

  bool get_many_maximums() {
    return many_maximums;
  }

  void max_char_update(int count, char ch) {
    if (count > max_in_subtree) {
      max_in_subtree = count;
      max_char_in_subtree = ch;
      many_maximums = false;
      is_max_in_subtree = false;
    } else if (count == max_in_subtree) {
      many_maximums = true;
    }
  }

  int add_word() {
    words_at_node++;
    if (words_at_node > max_in_subtree) {
      max_in_subtree = words_at_node;
      is_max_in_subtree = true;
    }
    return words_at_node;
  }

 private:
  int char_to_index(char ch) {
    if (('A' <= ch) and (ch <= 'Z')) {
      return ch - 'A';
    } else if (('a' <= ch) and (ch <= 'z')) {
      return ch - 'a' + ('Z' - 'A') + 1;
    } else {
      return -1;
    }
  }

  std::vector<char> neighbours_list;
  bool is_neighbour[alphabet_size * 2];
  int neighbour_index[alphabet_size * 2];
  int words_at_node = 0;
  int max_in_subtree = 0;
  char max_char_in_subtree;
  bool many_maximums = false;
  bool is_max_in_subtree = true;
};

class Trie {
 public:
   Trie() { nodes.push_back(Node()); }

  void add_word(std::string &str) {
    // add_word_rek(str, 0, 0);
    find_pref(str, 0, 0, true);
  }

  std::pair<bool, int> complete_prefix(std::string &str) {
    std::string res = str;

    int cur_node = find_pref(str, 0, 0, false);

    complete_pref_rek(str, cur_node);

    return {nodes[cur_node].get_many_maximums(), cur_node};
  }

  std::pair<bool, int> complete_prefix(int cur_node, std::string &str_last, std::string &str) {
    std::string res = str;

    cur_node = find_pref(str, 0, cur_node);
    complete_pref_rek(str_last, cur_node);

    return {nodes[cur_node].get_many_maximums(), cur_node};
  }

 private:
  int find_pref(std::string &str, int ch_ind, int cur_node, bool create = true) {
    int next_node;
    if (nodes[cur_node].contains_char(str[ch_ind])) {
      next_node = nodes[cur_node].get_neighbour_index(str[ch_ind]);
    } else if (create){
      nodes.push_back(Node());
      nodes[cur_node].add_char(str[ch_ind], nodes.size() - 1);
      next_node = nodes.size() - 1;
    } else {
      return -1;
    }

    int count;
    if (ch_ind + 1 < str.size()) {
      count = find_pref(str, ch_ind + 1, next_node, create);
      if (create) {
        int val = nodes[next_node].get_max();
        nodes[cur_node].max_char_update(val, str[ch_ind]);
      }
    } else {
      if (create) {
        int val = nodes[next_node].add_word();
        nodes[cur_node].max_char_update(val, str[ch_ind]);
      }
      count = next_node;
    }
    return count;
  }

  bool complete_pref_rek(std::string &str, int cur_node) {
    if (nodes[cur_node].get_is_max_in_subtree()) {
     return nodes[cur_node].get_many_maximums();
    }
    int next_node = nodes[cur_node].get_max_char_index();
    str += nodes[cur_node].get_max_char();

    return complete_pref_rek(str, next_node);
  }

  std::vector<Node> nodes;
};
