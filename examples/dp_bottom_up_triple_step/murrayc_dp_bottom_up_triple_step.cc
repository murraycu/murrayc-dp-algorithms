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
  type_subproblem calc_subproblem(type_base::type_level /* level */, std::size_t i) const override {
    //Base cases:
    if (i == 0) {
      //0 steps to get to 0:
      return 0;
    }
    
    const type_subproblems subproblem_i_minus_1 = subproblems_.get(-1);
    std::size_t result = subproblem_i_minus_1 + 1;

    if(i >= 2) {
      const type_subproblems subproblem_i_minus_2 = subproblems_.get(-2);
      result += subproblem_i_minus_2 + 2;
    }

    if(i >= 3) {
      const type_subproblems subproblem_i_minus_3 = subproblems_.get(-3);
      result += subproblem_i_minus_3 + 3;
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
