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


#define MURRAYC_DP_DEBUG_OUTPUT = 1;

/** A base class for a 2D top-down (memoization) dynamic programming algorithm.
 *
 * Override this, implementing calc_cost(), and then call calc() to get the
 * overall cost.
 * @tparam T_cost The type of the cost, such as unsigned int.
 */
template <typename T_cost, typename T_i = unsigned int, typename T_j = unsigned int>
class DpTopDownBase {
public:
  using type_cost = T_cost;
  using type_i = T_i;
  using type_j = T_j;
  using type_level = unsigned int;
  
  //Map of j values to costs.
  using type_costs = std::unordered_map<type_j, T_cost>;

  /**
   * @param The number of i values to calculate the cost for.
   * @pram The number of j values to calculate the cost for.
   */
  DpTopDownBase(type_i i_count, type_j j_count)
  : i_count_(i_count),
    j_count_(j_count)
  {}
  
  type_cost calc() {
    type_i goal_i = type_i();
    type_j goal_j = type_j();
    get_goal_cell(goal_i, goal_j);

    type_level level = 0;
    return get_cost(goal_i, goal_j, level);
  }
  
  unsigned int count_cached_sub_problems() const {
    unsigned int result = 0;
    for (const auto i_pair : costs_) {
      const auto j_map = i_pair.second;
      result += j_map.size();
    }
    
    return result;
  }

protected:
  /** Get the cost from the cache if it is in the cache,
   * or call calc_cost() and put it in the cache, then return it.
   *
   * See calc_cost().
   */
  type_cost get_cost(type_i i, type_j j, type_level level) const {
    type_cost result;
    if (!get_cached_cost(i, j, result)) {
#if defined MURRAYC_DP_DEBUG_OUTPUT
      indent(level);
      std::cout << "DpTopDownBase::get_cost(): i=" << i << ", j=" << j << std::endl;
#endif //MURRAYC_DP_DEBUG_OUTPUT
      ++level;
      result = calc_cost(i, j, level);
      costs_[i][j] = result;
    }

    return result;
  }
  

private:
  /** Calculate the cost.
   * This will only be called when the cost has not yet been calculated.
   * The implementation dos not need to check any cache first,
   * and does not need to store the result in any cache.
   *
   * Implementations should call get_cost() to get related sub-problem results.
   */
  virtual type_cost calc_cost(type_i i, type_j j, type_level level) const = 0;

  /** Get the cell whose value contains the solution.
   */
  virtual void get_goal_cell(type_i& i, type_j& j) const = 0;
  
  /** Gets the already-calculated cost, if any.
   * @result true if the cost was in the cache.
   */
  bool get_cached_cost(type_i i, type_j j, type_cost& cost) const {
    //std::cout << "get_cached_cost(): i=" << i << ", j=" << j << std::endl;
    const auto iter_i = costs_.find(i);
    if (iter_i == costs_.end()) {
      cost = type_cost();
      return false;
    }

    const auto map_j = iter_i->second;
    const auto iter_j = map_j.find(j);
    if (iter_j == map_j.end()) {
      cost = type_cost();
      return false;
    }

    //std::cout << "get_cached_cost(): returning cache for i=" << i << ", j=" << j << std::endl;
    cost = iter_j->second;
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
  
  //Map of i values to maps of j values to costs:
  using type_map_costs = std::unordered_map<type_i, type_costs>;
  mutable type_map_costs costs_;

  type_i i_count_;
  type_j j_count_;
};

#endif //MURRAYCDP_DP_BOTTOM_UP_BASE_H
