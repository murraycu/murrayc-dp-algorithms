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
    size_type pos_for_offset(int offset) {
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

uint dp_calc_edit_distance(const std::string& str, const std::string& pattern) {
  const auto str_size = str.size();
  const auto pattern_size = pattern.size();
  
  const uint count_costs_to_keep = 2;
  using type_costs = std::vector<uint>;
  circular_vector<type_costs> costs(count_costs_to_keep,
    type_costs(pattern_size, 0));

  for (uint i = 1; i < str_size; ++i) {
    costs.step(); //Swap costs_i and costs_i_minus_1.
    type_costs& costs_i = costs.get(0);
    const type_costs& costs_i_minus_1 = costs.get(-1);

    for (uint j = 1; j < pattern_size; ++j) {
      //Get the cost of the possible operations, and choose the least costly:
      const uint cost_match = costs_i_minus_1[j - 1] + match(str[i], pattern[j]);
      const uint cost_insert = costs_i[j - 1] + indel(pattern[j]);
      const uint cost_delete = costs_i_minus_1[j] + indel(str[j]);
      
      auto min = std::min(cost_match, cost_insert);
      min = std::min(min, cost_delete);
      costs_i[j] = min;
    }

    //costs_i will then be read as costs_i_minus_1;
    //and costs_i_minus_1 will be filled as costs_i;
  }
  
  const type_costs& costs_i = costs.get(0);
  return costs_i[pattern_size-1];
}

int main() {
  const auto str = "you should not";
  const auto pattern = "though shalt not";
  
  const auto distance = dp_calc_edit_distance(str, pattern);
  std::cout << "string: " << str << std::endl
    << "pattern: " << pattern << std::endl
    << "distance: " << distance << std::endl;
  
  return EXIT_SUCCESS;
}
