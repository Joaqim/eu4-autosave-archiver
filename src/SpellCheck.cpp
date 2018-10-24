
#include "impl/bk_tree.hpp"
#include "impl/levenshtein_distance.hpp"

#include <locale>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <cctype> // For isdigit()
#include <vector>
#include <assert.h>

#include <iostream>
#include <fstream>


std::string joinStringVector(const std::vector<std::string> & v, const std::string & delimiter = ",") {
	//std::vector<std::string>::iterator itr;
  std::string out;
  //itr = v.begin(); 
  auto itr = v.begin(); 
  auto e = v.end(); 
  if(itr != e) {
    out += *itr++;
    for (; itr != e; ++itr) out.append(delimiter).append(*itr);
  }
  return out;
}

std::string extract_ints(std::ctype_base::mask category, std::string str, std::ctype<char> const& facet) {
  using std::strlen;

  char const *begin = &str.front();
  char const *end   = &str.back();
  int i;
  for (i = 0; i < str.length(); i++) {
    if (! isdigit(str[i])) {
      if(i != 0) {
        //str = str.substr(0,i);
        //end = &str.back();
        break;
      }
    }
  }
  //printf("End: %c \n", *end);

#if 1 // Insert whitespace between int and string
  str.insert(i, 1, ' ');

#else // Remove everything after int
  auto res = facet.scan_is(category, begin, end);
  assert(strlen(res) <= str.length());

  begin = &res[0];
  end   = &res[strlen(res)];

  //std::cout << res << std::endl;
#endif

  return std::string(begin, end);
}

std::string extract_ints(std::string str) {
  return extract_ints(std::ctype_base::digit, str,
                      std::use_facet<std::ctype<char>>(std::locale("")));
}


int levDistance(const std::string source, const std::string target) {
  // Step 1
  const int n = source.length();
  const int m = target.length();
  if (n == 0) {
    return m;
  }
  if (m == 0) {
    return n;
  }
  // Good form to declare a TYPEDEF
  typedef std::vector< std::vector<int> > Tmatrix;
  Tmatrix matrix(n+1);
  // Size the vectors in the 2.nd dimension. Unfortunately C++ doesn't
  // allow for allocation on declaration of 2.nd dimension of vec of vec
  for (int i = 0; i <= n; i++) {
    matrix[i].resize(m+1);
  }
  // Step 2
  for (int i = 0; i <= n; i++) {
    matrix[i][0]=i;
  }
  for (int j = 0; j <= m; j++) {
    matrix[0][j]=j;
  }
  // Step 3
  for (int i = 1; i <= n; i++) {
    const char s_i = source[i-1];
    // Step 4
    for (int j = 1; j <= m; j++) {
      const char t_j = target[j-1];
      // Step 5
      int cost;
      if (s_i == t_j) {
        cost = 0;
      }
      else {
        cost = 1;
      }
      // Step 6
      const int above = matrix[i-1][j];
      const int left = matrix[i][j-1];
      const int diag = matrix[i-1][j-1];
      int cell = std::min( above + 1, std::min(left + 1, diag + cost));
      // Step 6A: Cover transposition, in addition to deletion,
      // insertion and substitution. This step is taken from:
      // Berghel, Hal ; Roach, David : "An Extension of Ukkonen's
      // Enhanced Dynamic Programming ASM Algorithm"
      // (http://www.acm.org/~hlb/publications/asm/asm.html)
      if (i>2 && j>2) {
        int trans=matrix[i-2][j-2]+1;
        if (source[i-2]!=t_j) trans++;
        if (s_i!=target[j-2]) trans++;
        if (cell>trans) cell=trans;
      }
      matrix[i][j]=cell;
    }
  }
  // Step 7
  return matrix[n][m];
}


struct SpellCheck {
  MB::bktree<std::string, int, MB::levenshtein_distance> tree;
  std::vector<std::pair<std::string, int> > results;

  SpellCheck(char const filename[] = "../data/dictionary.txt") {
    //const char filename[] = "../data/dictionary.txt";

    std::ifstream ifs(filename);
    if (!ifs) {
      std::cerr << "Couldn't open " << filename << " for reading\n";
    }

    std::string word;
    while (ifs >> word) {
      tree.insert(word);
    }

    //std::cout << "Inserted " << tree.size() << " words\n";
    if(tree.size() >= 12) {
      std::cout << "dictionary.txt successfully loaded" << std::endl;
    }
    //TODO: Load months from dictionary.txt
    // change dictionary.txt format to MONTH #
    // i.e: january 1
    //      march 3

    months["january"] = 1;
    months["february"] = 2;
    months["march"] = 3;
    months["april"] = 4;
    months["may"] = 5;
    months["june"] = 6;
    months["july"] = 7;
    months["august"] = 8;
    months["september"] = 9;
    months["october"] = 10;
    months["november"] = 11;
    months["december"] = 12;

  }

  std::vector<std::string> operator()(std::string str) {
    std::vector<std::string> result;
    if(str.size() <= 1)
      return result;

    // remove "newline"
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());

    std::pair<std::string, int> correct_word("",8);
    const uint limit = 4;
    //t1 = "aj";
    //str = "31DecenberES";
    //std::stringstream ss(extract_ints(str));
    //std::cout << ss.str() << std::endl;

    std::stringstream ss(str);
    std::string word;
    while(std::getline(ss, word, ' ' )) {
      if(std::isdigit(word[0])) {
        if(word.size() <= 4) {
          //TODO: Check that the number is accurate/plausible
          //std::cout << std::atoi(word.c_str()) << std::endl;
          result.push_back(word);
          continue;
        }
        result.push_back("00");
        continue;
      }
      tree.find(word, limit, std::back_inserter(results));
      //for (const auto &it = results.begin(); it != results.end(); ++it) {
      for (const auto &it : results) {
        //std::cout << it.first << "(distance " << it.second << ")\n";
        if (it.second <= correct_word.second) {
          correct_word = it;
        }
      }
      if(correct_word.second >= 6) {
        //printf("Failed to find correct word.");
        result.push_back(word);
      } else {
        //printf("\nCorrect word: %s\n", correct_word.first.c_str());
        result.push_back(correct_word.first);
      }
    }

    results.clear();

    // Prints out result
    for (auto &w: result) std::cout << w << std::endl;

    if (result.size()> 3) {
      // Combine any stray characters at the end with year "1 444" -> "1444"
      std::string tmp = result.back(); // Get last string
      result.pop_back(); // remove it
      if(tmp.size() == 1 && tmp != " ") { // check if it's a single character (digit in our case)
        result.back() = tmp + result.back(); // combine it with the new last string
      } else if(tmp.size() == 2 && result.back().size() != 4) { // If last digit is 2 size and year is only 2 maybe combine?
        //TODO: Make sure the year date and month is accurate. . . (relative to previous dates)
        result.back() = tmp + result.back();

      }
    }

    if(result.back().size() != 4) {
      return std::vector<std::string>{""};
    }
    std::reverse(result.begin(), result.end());

#if 1 // If we want Numerical months
    for (auto &w : result) {
      if(w.size() >= 3) {
        if(!std::isdigit(w[0])) {
          w = std::to_string(months[w]);
        }
      }
    }
#endif

    return result;
  }

  std::map<std::string, int> months;
};



