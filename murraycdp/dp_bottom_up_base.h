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

#ifndef _MURRAYCDP_DP_BOTTOM_UP_BASE_H
#define _MURRAYCDP_DP_BOTTOM_UP_BASE_H

#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>

#include <murraycdp/circular_vector.h>

#define MURRAYC_DP_DEBUG_OUTPUT = 1;

/** A base class for a 2D bottom-up dynamic programming algorithm.
 *
 * Override this, implementing calc_subproblem(), and then call calc() to get the
 * overall solution.
 * @tparam T_COUNT_SUBPROBLEMS_TO_KEEP The number of previous i values that calc_subproblem() needs to use.
 * @tparam T_subproblem The type of the subproblem solution, such as unsigned int,
 * or a custom class containing a value and a partial path.
 */
template <unsigned int T_COUNT_SUBPROBLEMS_TO_KEEP, typename T_subproblem>
class DpBottomUpBase {
public:
  using type_subproblem = T_subproblem;
  using type_subproblems = std::vector<T_subproblem>;

  /**
   * @param The number of i values to calculate the subproblem for.
   * @pram The number of j values to calculate the subproblem for.
   */
  DpBottomUpBase(unsigned int i_count, unsigned int j_count)
  : subproblems_(T_COUNT_SUBPROBLEMS_TO_KEEP, type_subproblems(j_count)),
    i_count_(i_count),
    j_count_(j_count)
  {}
  
  DpBottomUpBase(const DpBottomUpBase& src) = delete;
  DpBottomUpBase& operator=(const DpBottomUpBase& src) = delete;

  DpBottomUpBase(DpBottomUpBase&& src) = delete;
  DpBottomUpBase& operator=(DpBottomUpBase&& src) = delete;

  type_subproblem calc() {
    //TODO: subproblems_.clear();

    for (unsigned int i = 0; i < i_count_; ++i) {
      type_subproblems& subproblems_i = subproblems_.get(0);

#if defined(MURRAYC_DP_DEBUG_OUTPUT)
      std::cout << "i=" << std::setw(2) << i << ": ";
#endif

      for (unsigned int j = 0; j < j_count_; ++j) {
        subproblems_i[j] = calc_subproblem(i, j);

#if defined(MURRAYC_DP_DEBUG_OUTPUT)
        if (j != 0) {
          std::cout << ", ";
        }
        std::cout << std::setw(2) << subproblems_i[j].cost;
#endif
      }

#if defined(MURRAYC_DP_DEBUG_OUTPUT)
      std::cout << std::endl;
#endif

      //subproblems_i will then be read as subproblems_i_minus_1;
      //and subproblems_i_minus_1 will be filled as subproblems_i;
      subproblems_.step(); //Swap subproblems_i and subproblems_i_minus_1.
    }

    unsigned int goal_i = 0;
    unsigned int goal_j = 0;
    get_goal_cell(goal_i, goal_j);
    //std::cout << "goal_i=" << goal_i << ", goal_j=" << goal_j << std::endl;

    const type_subproblems& subproblems_i = subproblems_.get_at_offset_from_start(goal_i);
    return subproblems_i[goal_j];
  }

private:
  virtual type_subproblem calc_subproblem(unsigned int i, unsigned int j) const = 0;

  /** Get the cell whose value contains the solution.
   */
  virtual void get_goal_cell(unsigned int& i, unsigned int& j) const = 0;

protected:
  using type_vec_subproblems = circular_vector<type_subproblems>;
  type_vec_subproblems subproblems_;
  uint i_count_;
  uint j_count_;
};

#endif //MURRAYCDP_DP_BOTTOM_UP_BASE_H
