#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>

#include <murraycdp/dp_base.h>

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

class DpEditDistance
  : public DpBase<2 /* count of costs to keep, used in calc_cost() */, Cost> {
public:
  DpEditDistance(const std::string& str, const std::string& pattern)
  : DpBase(str.size() + 1, pattern.size() + 1),
    str_(str),
    pattern_(pattern)
  {}

private:
  using uint = Cost::uint;

  type_cost calc_cost(uint i, uint j) const override {
    if (i == 0) {
      //Base case:
      return Cost(j * indel(' '), Cost::Operation::INSERT);
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
     //The answer is in the last-calculated cell:
     i = i_count_ - 1;
     j = j_count_ - 1;
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
  const auto pattern = "thou shalt not";
  
  DpEditDistance dp(str, pattern);
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
