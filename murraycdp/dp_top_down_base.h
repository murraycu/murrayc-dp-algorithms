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

#ifndef _MURRAYCDP_DP_TOP_DOWN_BASE_H
#define _MURRAYCDP_DP_TOP_DOWN_BASE_H

#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <unordered_map>


//#define MURRAYC_DP_DEBUG_OUTPUT = 1;

/** A base class for a 2D top-down (memoization) dynamic programming algorithm.
 *
 * Override this, implementing calc_subproblem(), and then call calc() to get the
 * overall solution.
 * @tparam T_subproblem The type of the subproblem solution, such as unsigned int,
 * or a custom class containing a value and a partial path.
 */
template <typename T_subproblem, typename T_i = unsigned int, typename T_j = unsigned int>
class DpTopDownBase {
public:
  using type_subproblem = T_subproblem;
  using type_i = T_i;
  using type_j = T_j;
  using type_level = unsigned int;
  
  //Map of j values to subproblems.
  using type_subproblems = std::unordered_map<type_j, T_subproblem>;

  /**
   * @param The number of i values to calculate the subproblem for.
   * @pram The number of j values to calculate the subproblem for.
   */
  DpTopDownBase(type_i i_count, type_j j_count)
  : i_count_(i_count),
    j_count_(j_count)
  {}
  
  type_subproblem calc() {
    type_i goal_i = type_i();
    type_j goal_j = type_j();
    get_goal_cell(goal_i, goal_j);

    type_level level = 0;
    return get_subproblem(goal_i, goal_j, level);
  }
  
  unsigned int count_cached_sub_problems() const {
    unsigned int result = 0;
    for (const auto i_pair : subproblems_) {
      const auto j_map = i_pair.second;
      result += j_map.size();
    }
    
    return result;
  }

protected:
  /** Get the subproblem solution from the cache if it is in the cache,
   * or call calc_subproblem() and put it in the cache, then return it.
   *
   * See calc_subproblem().
   */
  type_subproblem get_subproblem(type_i i, type_j j, type_level level) const {
    type_subproblem result;
    if (!get_cached_subproblem(i, j, result)) {
#if defined MURRAYC_DP_DEBUG_OUTPUT
      indent(level);
      std::cout << "DpTopDownBase::get_subproblem(): i=" << i << ", j=" << j << std::endl;
#endif //MURRAYC_DP_DEBUG_OUTPUT
      ++level;
      result = calc_subproblem(i, j, level);
      subproblems_[i][j] = result;
    }

    return result;
  }
  

private:
  /** Calculate the subproblem solution.
   * This will only be called when the subproblem solution has not yet been calculated.
   * The implementation dos not need to check any cache first,
   * and does not need to store the result in any cache.
   *
   * Implementations should call get_subproblem() to get related sub-problem results.
   */
  virtual type_subproblem calc_subproblem(type_i i, type_j j, type_level level) const = 0;

  /** Get the cell whose value contains the solution.
   */
  virtual void get_goal_cell(type_i& i, type_j& j) const = 0;
  
  /** Gets the already-calculated subproblem solution, if any.
   * @result true if the subproblem solution was in the cache.
   */
  bool get_cached_subproblem(type_i i, type_j j, type_subproblem& subproblem) const {
    //std::cout << "get_cached_subproblem(): i=" << i << ", j=" << j << std::endl;
    const auto iter_i = subproblems_.find(i);
    if (iter_i == subproblems_.end()) {
      subproblem = type_subproblem();
      return false;
    }

    const auto map_j = iter_i->second;
    const auto iter_j = map_j.find(j);
    if (iter_j == map_j.end()) {
      subproblem = type_subproblem();
      return false;
    }

    //std::cout << "get_cached_subproblem(): returning cache for i=" << i << ", j=" << j << std::endl;
    subproblem = iter_j->second;
    return true;
  }

protected:
  static void indent(type_level level)
  {
    std::cout << "level: " << level;
    for(type_level l = 0; l < level; ++l) {
      std::cout << "  ";
    }
  }

protected:
  
  //Map of i values to maps of j values to subproblems:
  using type_map_subproblems = std::unordered_map<type_i, type_subproblems>;
  mutable type_map_subproblems subproblems_;

  type_i i_count_;
  type_j j_count_;
};

#endif //MURRAYCDP_DP_BOTTOM_UP_BASE_H
