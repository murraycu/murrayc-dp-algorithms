/* Copyright (C) 2015 Murray Cumming
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/
 */

#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include <murraycdp/dp_bottom_up_base.h>

class Cost {
public:
  using uint = unsigned int;

  enum class Operation { INVALID, MATCH, INSERT, DELETE };

  using type_path = std::vector<Operation>;

  explicit Cost() : cost(0) {}

  Cost(uint in_cost, Operation initial_path)
  : cost(in_cost), path(1, initial_path) {}

  Cost(uint in_cost, const type_path& in_path) : cost(in_cost), path(in_path) {}

  Cost(const Cost& src) = default;
  Cost&
  operator=(const Cost& src) = default;

  Cost(Cost&& src) noexcept = default;
  Cost&
  operator=(Cost&& src) noexcept = default;

  static std::string
  get_operation_as_string(Operation op) {
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

/**
 * This is like DpEditDistance, but it doesn't care where
 * we start the match. So the base case is 0 regardless of j,
 * and it examines all j in the last iteration (in get_goal_cell())
 * to find the final answer.
 * See page 289 of The Algorithm Design Manual.
 */
class DpSubstringMatching
  : public murraycdp::DpBottomUpBase<
      2 /* cost to keep, used in calc_subproblem() */, Cost, uint, uint> {
public:
  DpSubstringMatching(const std::string& str, const std::string& pattern)
  : DpBottomUpBase(str.size() + 1, pattern.size() + 1), // DpBottomUpBase
    // without the
    // specialization is
    // apparently
    // allowed.
    str_(str),
    pattern_(pattern) {}

private:
  type_subproblem
  calc_subproblem(type_level /* level */, uint i, uint j) const override {
    if (i == 0) {
      // Base case:
      return Cost(0, Cost::Operation::INVALID);
    }

    if (j == 0) {
      // Base case:
      return Cost(i * indel(' '), Cost::Operation::DELETE);
    }

    const type_subproblems& subproblems_i = subproblems_.get(0);
    const type_subproblems& subproblems_i_minus_1 = subproblems_.get(-1);

    // Get the cost of the possible operations, and choose the least costly:
    const auto char_str_i =
      str_[i - 1]; // i is 1-indexed, but the str is 0-indexed.
    const auto char_pattern_j =
      pattern_[j - 1]; // j is 1-indexed, but the pattern is 0-indexed.

    const uint cost_match =
      subproblems_i_minus_1[j - 1].cost + match(char_str_i, char_pattern_j);
    const uint cost_insert = subproblems_i[j - 1].cost + indel(char_pattern_j);
    const uint cost_delete = subproblems_i_minus_1[j].cost + indel(char_str_i);

    auto min = std::min(cost_match, cost_insert);
    min = std::min(min, cost_delete);

    // Remember the path, based on what operation produced this minimum cost:
    Cost::type_path path;
    if (min == cost_match) {
      path = subproblems_i_minus_1[j - 1].path;
      path.emplace_back(Cost::Operation::MATCH);
    } else if (min == cost_insert) {
      path = subproblems_i[j - 1].path;
      path.emplace_back(Cost::Operation::INSERT);
    } else if (min == cost_delete) {
      path = subproblems_i_minus_1[j].path;
      path.emplace_back(Cost::Operation::DELETE);
    } else {
      std::cerr << "Unexpected min." << std::endl;
    }

    return Cost(min, path);
  }

  void
  get_goal_cell(unsigned int& i, unsigned int& j) const override {
    // The answer is in the last-calculated i cell,
    // in the j cell that has the least cost:
    i = str_.size();
    const type_subproblems& subproblems_i =
      subproblems_.get_at_offset_from_start(i);

    uint min_cost = std::numeric_limits<uint>::max();
    unsigned int j_min_cost = 0;
    const auto j_count = pattern_.size() + 1;
    for (unsigned int k = 1; k < j_count; ++k) {
      const auto& cost = subproblems_i[k].cost;
      if (cost < min_cost) {
        min_cost = cost;
        j_min_cost = k;
      }
    }

    j = j_min_cost;
  }

  static uint
  match(const char ch_str, const char ch_pattern) {
    if (ch_str == ch_pattern) {
      return 0;
    } else {
      return 1;
    }
  }

  static uint
  indel(char /* ch */) {
    return 1;
  }

  const std::string str_;
  const std::string pattern_;
};

int
main() {
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

  assert(result.cost == 8);

  return EXIT_SUCCESS;
}
