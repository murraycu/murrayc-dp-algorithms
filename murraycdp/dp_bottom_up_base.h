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

#include <murraycdp/dp_base.h>
#include <murraycdp/circular_vector.h>
#include <murraycdp/vector_of_vectors.h>

#define MURRAYC_DP_DEBUG_OUTPUT = 1;

/** A base class for a 2D bottom-up dynamic programming algorithm.
 *
 * Override this, implementing calc_subproblem(), and then call calc() to get the
 * overall solution.
 * @tparam T_COUNT_SUBPROBLEMS_TO_KEEP The number of previous i values that calc_subproblem() needs to use.
 * @tparam T_subproblem The type of the subproblem solution, such as unsigned int,
 * or a custom class containing a value and a partial path.
 */
template <unsigned int T_COUNT_SUBPROBLEMS_TO_KEEP,
  typename T_subproblem, typename... T_value_types>
class DpBottomUpBase : public DpBase<T_subproblem, T_value_types...> {
public:
  using type_subproblem = T_subproblem;
  using type_subproblems = typename vector_of_vectors<T_subproblem, 0>::type;
  using type_base = DpBase<T_subproblem, T_value_types...>;

  /**
   * @param The number of i values to calculate the subproblem for.
   * @pram The number of j values to calculate the subproblem for.
   */
  DpBottomUpBase(unsigned int i_count, unsigned int j_count)
  : subproblems_(T_COUNT_SUBPROBLEMS_TO_KEEP, type_subproblems(j_count)),
    i_count_(i_count),
    j_count_(j_count)
  {
    subproblems_.foreach(
      [j_count] (type_subproblems& item) {
        resize_vector_of_vectors(item, j_count);
      }
    );
  }
  
  DpBottomUpBase(const DpBottomUpBase& src) = delete;
  DpBottomUpBase& operator=(const DpBottomUpBase& src) = delete;

  DpBottomUpBase(DpBottomUpBase&& src) = delete;
  DpBottomUpBase& operator=(DpBottomUpBase&& src) = delete;

  type_subproblem calc() override {
    //TODO: subproblems_.clear();

    typename type_base::type_level level = 0; //unused

    for (unsigned int i = 0; i < i_count_; ++i) {
#if defined(MURRAYC_DP_DEBUG_OUTPUT)
      std::cout << "i=" << std::setw(2) << i << ": ";
#endif

      type_subproblems& subproblems_i = subproblems_.get_at_offset_from_start(i);
      for_vector_of_vectors(subproblems_i,
        [this, level, i] (unsigned int j) {
          const auto subproblem = this->calc_subproblem(level, i, j);
          this->set_subproblem(subproblem, i, j);

#if defined(MURRAYC_DP_DEBUG_OUTPUT)
          if (j != 0) {
            std::cout << ", ";
          }
          std::cout << std::setw(2) << subproblem.cost;
#endif
        },
        (std::size_t)0, (std::size_t)j_count_
      );

#if defined(MURRAYC_DP_DEBUG_OUTPUT)
      std::cout << std::endl;
#endif

      //subproblems_i will then be read as subproblems_i_minus_1;
      //and subproblems_i_minus_1 will be filled as subproblems_i;
      subproblems_.step(); //Swap subproblems_i and subproblems_i_minus_1.
    }

    unsigned int goal_i = 0;
    unsigned int goal_j = 0;
    this->get_goal_cell(goal_i, goal_j);
    //std::cout << "goal_i=" << goal_i << ", goal_j=" << goal_j << std::endl;

    const type_subproblems& subproblems_i = subproblems_.get_at_offset_from_start(goal_i);
    return subproblems_i[goal_j];
  }

private:
  //TODO: void set_subproblem(T_value_types... values) {
  void set_subproblem(const type_subproblem& subproblem, unsigned int i, unsigned int j) {
    //TODO: Performance: Avoid repeated calls to get_at_offset_from_start(),
    //while still keeping the code generic:
    type_subproblems& subproblems_i = subproblems_.get_at_offset_from_start(i);
    subproblems_i[j] = subproblem;
  }

protected:
  using type_vec_subproblems = circular_vector<type_subproblems>;
  type_vec_subproblems subproblems_;
  uint i_count_;
  uint j_count_;
};

#endif //MURRAYCDP_DP_BOTTOM_UP_BASE_H
