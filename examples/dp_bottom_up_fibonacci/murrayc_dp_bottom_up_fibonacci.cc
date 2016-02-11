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
#include <boost/timer/timer.hpp>

#include <murraycdp/dp_bottom_up_base.h>


/** This is the simplest example of dynamic programming,
 * just to see how much boilerplate is added by the use of DpBottomUpBase.
 * You'd be far better off just doing this:
 * @code
 * unsigned long calc_fibonacci(unsigned int n) {
 *   if (n == 0)
 *     return 0;
 *   else if (n == 1)
 *     return 1;
 *
 *   unsigned long result = 0;
 *   unsigned long prev1 = 1; // f(1) = 0;
 *   unsigned long prev2 = 0; // f(0) = 0;
 *   for (unsigned int i = 1; i < n; ++i) {
 *     result = (prev1 + prev2);
 *     prev2 = prev1;
 *     prev1 = result;
 *   }
 *
 *   return result;
 * }
 * @endcode
 */
class DpFibonacci
  : public murraycdp::DpBottomUpBase<
      2 /* count of subproblems to keep, used in calc_subproblem() */,
      unsigned long, // sub problem type
      unsigned int // i
      > {
public:
  explicit DpFibonacci(unsigned int n)
  : DpBottomUpBase(n + 1), //DpBottomUpBase without the specialization is apparently allowed.
    n_(n)
  {}

private:
  type_subproblem calc_subproblem(type_base::type_level level, unsigned int i) const override {
    //Base cases:
    if (i == 0) {
      return 0;
    } else if (i == 1) {
      return 1;
    }

    return get_subproblem(level, i - 1) + get_subproblem(level, i -2);
  }

  void get_goal_cell(unsigned int& i) const override {
     //The answer is in the last-calculated cell:
     i = n_;
  }
  
  const unsigned int n_;
};

int main() {
  const auto n = 90;
  
  DpFibonacci dp(n);

  unsigned long result = 0;
  {
    boost::timer::auto_cpu_timer timer;
    result = dp.calc();
  }

  std::cout << "fibonacci number: " << n << std::endl
    << "result: " << result << std::endl;

  assert(result == 2880067194370816120ul);
  
  return EXIT_SUCCESS;
}
