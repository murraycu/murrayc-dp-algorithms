#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>

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

/** A base class for a 2D bottom-up dynamic programming algorithm.
 *
 * Override this, implementing calc_cost(), and then call calc() to get the
 * overall cost.
 * @tparam T_COUNT_COSTS_TO_KEEP The number of previous i values that calc_cost() needs to use.
 * @tparam T_cost The type of the cost, such as unsigned int.
 */
template <unsigned int T_COUNT_COSTS_TO_KEEP, typename T_cost>
class DpBase {
public:
  using type_costs = std::vector<T_cost>;

  /**
   * @param The number of i values to calculate the cost for.
   * @pram The number of j values to calculate the cost for.
   */
  DpBase(unsigned int i_count, unsigned int j_count)
  : costs_(T_COUNT_COSTS_TO_KEEP, type_costs(j_count)),
    i_count_(i_count),
    j_count_(j_count)
  {}
  
  T_cost calc() {
    for (unsigned int i = 1; i < i_count_; ++i) {
      costs_.step(); //Swap costs_i and costs_i_minus_1.
      type_costs& costs_i = costs_.get(0);

      for (unsigned int j = 1; j < j_count_; ++j) {
        costs_i[j] = calc_cost(i, j);
      }

      //costs_i will then be read as costs_i_minus_1;
      //and costs_i_minus_1 will be filled as costs_i;
    }

    const type_costs& costs_i = costs_.get(0);
    return costs_i[j_count_ - 1];
  }
 
private:
  virtual T_cost calc_cost(unsigned int i, unsigned int j) const = 0;

protected:
  using type_vec_costs = circular_vector<type_costs>;
  type_vec_costs costs_;
  uint i_count_;
  uint j_count_;
};

class DpEditDistance
  : public DpBase<2 /* cost to keep, used in calc_cost() */, uint> {
public:
  explicit DpEditDistance(const std::string& str, const std::string& pattern)
  : DpBase(str.size(), pattern.size()),
    str_(str),
    pattern_(pattern)
  {}

private:
  uint calc_cost(uint i, uint j) const override {
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

  static uint match(const char ch_str, const char ch_pattern) {
    if (ch_str == ch_pattern) {
      return 0;
    } else {
      return 1;
    }
  }

  static uint indel(char /* ch */) {
    return 1;
  }

  
  const std::string str_;
  const std::string pattern_;
};

int main() {
  const auto str = "you should not";
  const auto pattern = "though shalt not";
  
  DpEditDistance dp(str, pattern);
  const auto distance = dp.calc();
  std::cout << "string: " << str << std::endl
    << "pattern: " << pattern << std::endl
    << "distance: " << distance << std::endl;
  
  return EXIT_SUCCESS;
}
