#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>

#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>

#define MURRAYC_DP_DEBUG_OUTPUT = 1;

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
      steps_count_(0),
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

    T& get_at_offset_from_start(int offset) {
      return get(get_offset_from_start(offset));
    }

    const T& get_at_offset_from_start(int offset) const {
      return get(get_offset_from_start(offset));
    }

    /** Cause get(-1) to return whatever get(0) currently returns.
     */
    void step() {
      ++steps_count_;

      ++pos_zero_;
      if(pos_zero_ >= (int)size_)
        pos_zero_ = 0; 
    }

    size_type size() const {
      return size_;
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

    /** For instance, get the 5th item,
     * regardless of how many times we have called step().
     * This can fail if the @a offset is greater than size().
     */
    int get_offset_from_start(int offset) const {
     const int current_offset = offset - steps_count();
      if (std::abs(current_offset) > (int)size()) {
        std::cerr << "Item has already been discarded. size=" << size() << ", current_offset=" << current_offset << std::endl;
        return 0;
      }

      return current_offset;
    }

    /** Returns which index get(0) now represents.
     */
    int steps_count() const {
      return steps_count_;
    }

    int pos_zero_;
    int steps_count_;
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
  using type_cost = T_cost;
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
  
  type_cost calc() {
    for (unsigned int i = 0; i < i_count_; ++i) {
      type_costs& costs_i = costs_.get(0);

#if defined(MURRAYC_DP_DEBUG_OUTPUT)
      std::cout << "i=" << std::setw(2) << i << ": ";
#endif

      for (unsigned int j = 0; j < j_count_; ++j) {
        costs_i[j] = calc_cost(i, j);

#if defined(MURRAYC_DP_DEBUG_OUTPUT)
        if (j != 0) {
          std::cout << ", ";
        }
        std::cout << std::setw(2) << costs_i[j].cost;
#endif
      }

#if defined(MURRAYC_DP_DEBUG_OUTPUT)
      std::cout << std::endl;
#endif

      //costs_i will then be read as costs_i_minus_1;
      //and costs_i_minus_1 will be filled as costs_i;
      costs_.step(); //Swap costs_i and costs_i_minus_1.
    }

    unsigned int goal_i = 0;
    unsigned int goal_j = 0;
    get_goal_cell(goal_i, goal_j);
    //std::cout << "goal_i=" << goal_i << ", goal_j=" << goal_j << std::endl;

    const type_costs& costs_i = costs_.get_at_offset_from_start(goal_i);
    return costs_i[goal_j];
  }

private:
  virtual type_cost calc_cost(unsigned int i, unsigned int j) const = 0;

  /** Get the cell whose value contains the solution.
   */
  virtual void get_goal_cell(unsigned int& i, unsigned int& j) const = 0;

protected:
  using type_vec_costs = circular_vector<type_costs>;
  type_vec_costs costs_;
  uint i_count_;
  uint j_count_;
};

class Cost {
public:
  using uint = unsigned int;
  
  enum class Operation {
    INVALID,
    MATCH,
    INSERT,
    DELETE
  };

  using type_path = std::vector<Operation>;

  explicit Cost()
  : cost(0)
  {}

  Cost(uint in_cost, Operation initial_path)
  : cost(in_cost),
    path(1, initial_path)
  {}
  
  Cost(uint in_cost, const type_path& in_path)
  : cost(in_cost),
    path(in_path)
  {}

  Cost(const Cost& src) = default;
  Cost& operator=(const Cost& src) = default;

  Cost(Cost&& src) = default;
  Cost& operator=(Cost&& src) = default;

  static std::string get_operation_as_string(Operation op) {
    switch (op) {
      case Operation::MATCH:
        return "match";
      case Operation::INSERT:
        return "insert";
      case Operation::DELETE:
        return "delete";
      default:
        return "invalid";
    }
  }

  uint cost;
  type_path path;
};

class DpSubstringMatching
  : public DpBase<2 /* cost to keep, used in calc_cost() */, Cost> {
public:
  DpSubstringMatching(const std::string& str, const std::string& pattern)
  : DpBase(str.size() + 1, pattern.size() + 1),
    str_(str),
    pattern_(pattern)
  {}

private:
  type_cost calc_cost(uint i, uint j) const override {
    if (i == 0) {
      //Base case:
      return Cost(0, Cost::Operation::INVALID);
    }

    if (j == 0) {
      //Base case:
      return Cost(i * indel(' '), Cost::Operation::DELETE);
    }

    const type_costs& costs_i = costs_.get(0);
    const type_costs& costs_i_minus_1 = costs_.get(-1);

    //Get the cost of the possible operations, and choose the least costly:
    const auto char_str_i = str_[i - 1]; //i is 1-indexed, but the str is 0-indexed.
    const auto char_pattern_j = pattern_[j - 1]; //j is 1-indexed, but the pattern is 0-indexed.

    const uint cost_match = costs_i_minus_1[j - 1].cost + match(char_str_i, char_pattern_j);
    const uint cost_insert = costs_i[j - 1].cost + indel(char_pattern_j);
    const uint cost_delete = costs_i_minus_1[j].cost + indel(char_str_i);
    
    auto min = std::min(cost_match, cost_insert);
    min = std::min(min, cost_delete);

    //Remember the path, based on what operation produced this minimum cost:
    Cost::type_path path;
    if (min == cost_match) {
        path = costs_i_minus_1[j - 1].path;
        path.emplace_back(Cost::Operation::MATCH);
    } else if (min == cost_insert) {
        path = costs_i[j - 1].path;
        path.emplace_back(Cost::Operation::INSERT);
    } else if (min == cost_delete) {
        path = costs_i_minus_1[j].path;
        path.emplace_back(Cost::Operation::DELETE);
    } else {
      std::cerr << "Unexpected min." << std::endl;
    }

    return Cost(min, path);
  }

  void get_goal_cell(unsigned int& i, unsigned int& j) const override {
     //The answer is in the last-calculated i cell,
     //in the j cell that has the least cost:
     i = i_count_ - 1;
     const type_costs& costs_i = costs_.get_at_offset_from_start(i);

     uint min_cost = std::numeric_limits<uint>::max();
     unsigned int j_min_cost = 0;
     for (unsigned int k = 1; k < j_count_; ++k) {
       const auto& cost = costs_i[k].cost;
       if (cost < min_cost) {
          min_cost = cost;
          j_min_cost = k;
       }
     }
     
     j = j_min_cost;
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
  const auto str = "absckdieenfagh";
  const auto pattern = "skiena";
  
  DpSubstringMatching dp(str, pattern);
  const auto result = dp.calc();
  std::cout << "string: " << str << std::endl
    << "pattern: " << pattern << std::endl
    << "distance: " << result.cost << std::endl;

  std::cout << "Operations: ";
  for (const auto op : result.path) {
    std::cout << Cost::get_operation_as_string(op) << ", ";
  }
  std::cout << std::endl;
  
  return EXIT_SUCCESS;
}
