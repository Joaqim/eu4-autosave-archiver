#include <string>
#include <vector>
#include <algorithm>
 
namespace MB
{
 
namespace detail
{
 
template <typename T>
T min3(const T& a, const T& b, const T& c)
{
  return std::min(std::min(a, b), c);
}
 
}; // namespace detail
 
class levenshtein_distance 
{
  mutable std::vector<std::vector<unsigned int> > matrix_;
 
 public:
  explicit levenshtein_distance(size_t initial_size = 8)
      : matrix_(initial_size, std::vector<unsigned int>(initial_size))
  {
  }
 
  unsigned int operator()(const std::string& s, const std::string& t) const
  {
    const size_t m = s.size();
    const size_t n = t.size();
    // The distance between a string and the empty string is the string's length
    if (m == 0) {
      return n;
    }
    if (n == 0) {
      return m;
    }
    // Size the matrix as necessary
    if (matrix_.size() < m + 1) {
      matrix_.resize(m + 1, matrix_[0]);
    }
    if (matrix_[0].size() < n + 1) {
      for (auto& mat : matrix_) {
        mat.resize(n + 1);
      }
    }
    // The top row and left column are prefixes that can be reached by
    // insertions and deletions alone
    unsigned int i, j;
    for (i = 1;  i <= m; ++i) {
      matrix_[i][0] = i;
    }
    for (j = 1; j <= n; ++j) {
      matrix_[0][j] = j;
    }
    // Fill in the rest of the matrix
    for (j = 1; j <= n; ++j) {
      for (i = 1; i <= m; ++i) {
        unsigned int substitution_cost = s[i - 1] == t[j - 1] ? 0 : 1;
        matrix_[i][j] =
            detail::min3(matrix_[i - 1][j] + 1,         // Deletion
                         matrix_[i][j - 1] + 1,                      // Insertion
                         matrix_[i - 1][j - 1] + substitution_cost); // Substitution
      }
    }
    return matrix_[m][n];
  }
};
 
}; // namespace MB
