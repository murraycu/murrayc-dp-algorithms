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

#include <murraycdp/dp_top_down_base.h>

class SubSolution {
public:
  using type_value = std::size_t;

  SubSolution() : value(0) {}

  explicit SubSolution(type_value value_in) : value(value_in) {}

  SubSolution(const SubSolution& src) = default;
  SubSolution&
  operator=(const SubSolution& src) = default;

  SubSolution(SubSolution&& src) noexcept = default;
  SubSolution&
  operator=(SubSolution&& src) noexcept = default;

  type_value value;
  std::string solution;
};

/**
 * Longest Common Subsequence.
 * Based on the example in section 15.4 of CLRS.
 */
class DpLCS
  : public murraycdp::DpTopDownBase<SubSolution,
      std::string::size_type, std::string::size_type> {
public:
  using type_value = SubSolution::type_value;
  using type_size = std::string::size_type;

  DpLCS(const std::string& x, const std::string& y)
  : x_(x), y_(y) {}

private:
  type_subproblem
  calc_subproblem(type_level level, type_size i, type_size j) const override {
    if (i == 0) {
      return type_subproblem(0);
    }

    if (j == 0) {
      return type_subproblem(0);
    }

    if (x_[i - 1] == y_[j - 1]) {
      auto result = get_subproblem(level, i - 1, j - 1);
      result.value++;

      // TODO: It would use less space to just store the case that was chosen,
      // and would still take only linear time to reconstruct the solution from that.
      result.solution += x_[i - 1];
      return result;
    }

    auto sub_i_less = get_subproblem(level, i - 1, j);
    auto sub_j_less = get_subproblem(level, i, j - 1);
    if (sub_i_less.value >= sub_j_less.value ) {
      return sub_i_less;
    } else {
      return sub_j_less;
    }
  }

  void
  get_goal_cell(type_size& i, type_size& j) const override {
    // The answer is in the last-calculated cell:
    i = x_.size();
    j = y_.size();
  }

  const std::string x_, y_;
};

int
main() {
  const std::string X = "ABCBDAB";
  const std::string Y = "BDCABA";
  std::cout << "X: " << X << ", Y: " << Y << std::endl;

  DpLCS dp(X, Y);
  const auto result = dp.calc();

  std::cout << "solution: value: " << result.value << std::endl
            << "with solution: " << result.solution << std::endl;
  std::cout << std::endl;

  std::cout << "Count of sub-problems calculated: "
            << dp.count_cached_sub_problems() << std::endl;

  assert(result.value == 4);
  assert(result.solution == "BCBA");

  return EXIT_SUCCESS;
}
