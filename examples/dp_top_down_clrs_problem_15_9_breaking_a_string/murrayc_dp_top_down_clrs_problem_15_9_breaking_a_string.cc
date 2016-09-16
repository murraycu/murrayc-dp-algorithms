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
  using type_vec_cuts = std::vector<std::size_t>;
  using type_value = std::size_t;

  // static constexpr type_value COIN_COUNT_INFINITY =
  // std::numeric_limits<type_value>::max();

  SubSolution() : value_(0) {}

  explicit SubSolution(type_value value, const type_vec_cuts& solution)
  : value_(value), solution_(solution) {}

  SubSolution(const SubSolution& src) = default;
  SubSolution&
  operator=(const SubSolution& src) = default;

  SubSolution(SubSolution&& src) noexcept = default;
  SubSolution&
  operator=(SubSolution&& src) noexcept = default;

  type_value value_;
  type_vec_cuts solution_;
};

/** See CLRS Problem 15-9.
 */
class DpBreakingString : public murraycdp::DpTopDownBase<SubSolution,
                           SubSolution::type_vec_cuts::size_type,
                           SubSolution::type_vec_cuts::size_type> {
public:
  using type_value = SubSolution::type_value;
  using type_vec_cuts = SubSolution::type_vec_cuts;
  using type_size = type_vec_cuts::size_type;

  DpBreakingString(std::size_t str_len, const type_vec_cuts& cuts)
  : str_len_(str_len), cuts_(cuts) {}

private:
  type_subproblem
  calc_subproblem(type_level level, type_size i, type_size j) const override {
    assert(j >= i);
    if (j - i <= 1) {
      return type_subproblem(); // 0 cost for cutting a 0 or 1-length string
    }

    bool cut_needed = false;
    std::size_t min_cost = std::numeric_limits<std::size_t>::max();
    type_vec_cuts min_solution;
    for (const auto& cut : cuts_) {
      if (cut <= i) {
        continue;
      }

      if (cut >= j) {
        continue;
      }

      cut_needed = true;
      const auto left = get_subproblem(level, i, cut);
      const auto right = get_subproblem(level, cut + 1, j);
      const auto cost = (j - i + 1) + left.value_ + right.value_;
      if (cost < min_cost) {
        min_cost = cost;
        min_solution = {cut};
        min_solution.insert(
          min_solution.end(), left.solution_.begin(), left.solution_.end());
        min_solution.insert(
          min_solution.end(), right.solution_.begin(), right.solution_.end());
      }
    }

    if (!cut_needed) {
      min_cost = 0;
    }

    return type_subproblem(min_cost, min_solution);
  }

  void
  get_goal_cell(type_size& i, type_size& j) const override {
    assert(str_len_ > 0);
    i = 0;
    j = str_len_ - 1;
  }

  const std::size_t str_len_;
  const type_vec_cuts cuts_;
};

void
print_vec(const std::vector<std::size_t>& vec) {
  for (auto item : vec) {
    std::cout << item << ", ";
  }
}

int
main() {
  const std::size_t str_len = 20;
  const DpBreakingString::type_vec_cuts cuts{2, 6, 9, 14, 18};

  std::cout << "Problem: str_len: " << str_len << std::endl << "with cuts: ";
  print_vec(cuts);
  std::cout << std::endl;

  DpBreakingString dp(str_len, cuts);
  const auto result = dp.calc();

  std::cout << "solution: value: " << result.value_ << std::endl
            << "with solution: ";
  print_vec(result.solution_);
  std::cout << std::endl;

  std::cout << "Count of sub-problems calculated: "
            << dp.count_cached_sub_problems() << std::endl;

  // Uncomment to show the sequence: dp.print_subproblem_sequence();

  assert(result.value_ == 52);

  return EXIT_SUCCESS;
}
