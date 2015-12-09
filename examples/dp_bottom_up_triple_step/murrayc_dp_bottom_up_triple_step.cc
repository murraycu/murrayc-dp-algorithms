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

#include <cstdlib>
#include <cassert>
#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>

#include <murraycdp/dp_bottom_up_base.h>


class DpTripleStep
  : public murraycdp::DpBottomUpBase<
      4 /* count of subproblems to keep, used in calc_subproblem() */,
      std::size_t, // sub problem type
      std::size_t // i
      > {
public:
  DpTripleStep(const std::size_t steps_count)
  : DpBottomUpBase(steps_count), //TODO: Why does the compiler fail when we prefix this with murraycdp::
    steps_count_(steps_count)
  {}

private:
  type_subproblem calc_subproblem(type_base::type_level level, std::size_t i) const override {
    //Base cases:
    if (i == 0) {
      //0 steps to get to 0:
      return 0;
    }

    std::size_t result = 0;
    for (std::size_t j = 1; j <= 3; ++j) {
      if(i >= j) {
        const auto subproblem_prev = get_subproblem(level, i - j);
        result += subproblem_prev + j;
      }
    }

    return result;
  }

  void get_goal_cell(std::size_t& i) const override {
     //The answer is in the last-calculated cell:
     i = steps_count_ - 1;
  }
  
  const std::size_t steps_count_;
};

int main() {
  const auto steps_count = 20;
  
  DpTripleStep dp(steps_count);
  const auto result = dp.calc();
  std::cout << "steps count: " << steps_count << std::endl
    << "result: " << result << std::endl;

  assert(result == 233923);
  
  return EXIT_SUCCESS;
}
