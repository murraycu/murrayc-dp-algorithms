#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>

using uint = unsigned int;

enum class operations {
  MATCH,
  INSERT,
  DELETE
};

uint match(const char ch_str, const char ch_pattern) {
  if (ch_str == ch_pattern) {
    return 0;
  } else {
    return 1;
  }
}

uint indel(char /* ch */) {
  return 1;
}

template <typename T>
class circular_vector {
  public:
    using size_type = typename std::vector<T>::size_type;

    /**
     * Create a circular vector of size @a size,
     * with each item having value @a value.
     */
    explicit circular_vector(size_type size, const T& value = T())
    : pos_zero_(0),
      size_(size),
      vec_(size, value)
    {}
    
    T& get(int offset) {
      const auto pos = pos_for_offset(offset);
      return vec_[pos];
    }
    
    const T& get(int offset) const {
      const auto pos = pos_for_offset(offset);
      return vec_[pos];
    }

    /** Cause get(-1) to return whatever get(0) currently returns.
     */
    void step() {
      ++pos_zero_;
      if(pos_zero_ >= (int)size_)
        pos_zero_ = 0; 
    }
    
  private:
    /** Get a position relative to pos_zero_,
     * wrapping around if necessary.
     */    
    size_type pos_for_offset(int offset) const {
      if (offset >= (int)size_) {
        std::cerr << "offset too large: " << offset << std::endl;
      }

      int pos = pos_zero_ + offset;
      if(pos >= (int)size_)
        return 0 + (size_ - pos);
      else if (pos < 0)
        return size_ + pos;
      else
        return pos;
    }

    int pos_zero_;
    unsigned int size_;
    std::vector<T> vec_;
};

class DpEditDistance {
private:
  const uint COUNT_COSTS_TO_KEEP = 2;
  using type_costs = std::vector<uint>;
    
public:
  explicit DpEditDistance(const std::string& str, const std::string& pattern)
  : str_(str),
    pattern_(pattern),
    costs_(COUNT_COSTS_TO_KEEP, type_costs(pattern.size(), 0))
  {}

  uint calc_cost(uint i, uint j) const {
    const type_costs& costs_i = costs_.get(0);
    const type_costs& costs_i_minus_1 = costs_.get(-1);

    //Get the cost of the possible operations, and choose the least costly:
    const uint cost_match = costs_i_minus_1[j - 1] + match(str_[i], pattern_[j]);
    const uint cost_insert = costs_i[j - 1] + indel(pattern_[j]);
    const uint cost_delete = costs_i_minus_1[j] + indel(str_[j]);
    
    auto min = std::min(cost_match, cost_insert);
    min = std::min(min, cost_delete);
    return min;
  }

  uint calc_edit_distance() {
    const auto str_size = str_.size();
    const auto pattern_size = pattern_.size();
    
    for (uint i = 1; i < str_size; ++i) {
      costs_.step(); //Swap costs_i and costs_i_minus_1.
      type_costs& costs_i = costs_.get(0);

      for (uint j = 1; j < pattern_size; ++j) {
        costs_i[j] = calc_cost(i, j);
      }

      //costs_i will then be read as costs_i_minus_1;
      //and costs_i_minus_1 will be filled as costs_i;
    }

    const type_costs& costs_i = costs_.get(0);
    return costs_i[pattern_size-1];
  }
  
private:
  const std::string str_;
  const std::string pattern_;

  using type_vec_costs = circular_vector<type_costs>;
  type_vec_costs costs_;
};

int main() {
  const auto str = "you should not";
  const auto pattern = "though shalt not";
  
  DpEditDistance dp(str, pattern);
  const auto distance = dp.calc_edit_distance();
  std::cout << "string: " << str << std::endl
    << "pattern: " << pattern << std::endl
    << "distance: " << distance << std::endl;
  
  return EXIT_SUCCESS;
}
