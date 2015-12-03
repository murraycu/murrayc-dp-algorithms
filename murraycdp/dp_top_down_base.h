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
#include <tuple>
#include <murraycdp/dp_base.h>
#include <murraycdp/utils/tuple_hash.h>

namespace murraycdp {

//#define MURRAYC_DP_DEBUG_OUTPUT = 1;

/** A base class for a 2D top-down (memoization) dynamic programming algorithm.
 *
 * Override this, implementing calc_subproblem(), and then call calc() to get the
 * overall solution.
 * @tparam T_subproblem The type of the subproblem solution, such as unsigned int,
 * or a custom class containing a value and a partial path.
 */
template <typename T_subproblem, typename... T_value_types>
class DpTopDownBase : public DpBase<T_subproblem, T_value_types...> {
public:
  using type_base = DpBase<T_subproblem, T_value_types...>;
  using type_subproblem = T_subproblem;
  using type_level = unsigned int;
  
  using type_values = std::tuple<T_value_types...>;

  /**
   * @param The number of i values to calculate the subproblem for.
   * @pram The number of j values to calculate the subproblem for.
   */
  DpTopDownBase()
  : DpBase<T_subproblem, T_value_types...>()
  {}

  DpTopDownBase(const DpTopDownBase& src) = delete;
  DpTopDownBase& operator=(const DpTopDownBase& src) = delete;

  DpTopDownBase(DpTopDownBase&& src) = delete;
  DpTopDownBase& operator=(DpTopDownBase&& src) = delete;

  type_subproblem calc() override {
    clear();

    //We cannot do this to pass the output parameters to get_goal_cell():
    //  T_type_values... goal
    //but we can pass a std::tuple<> based on T_type_values...
    //and that will then be passed as individual parameters when we unpack it
    //via std::index_sequence.
    type_values goals;
    this->get_goal_cell_call_with_tuple(goals,
      std::index_sequence_for<T_value_types...>());
    //std::cout << "calc: " << std::get<0>(goals) << std::endl;

    type_level level = 0;
    return this->get_subproblem_call_with_tuple(level, goals,
      std::index_sequence_for<T_value_types...>());
  }

  unsigned int count_cached_sub_problems() const {
    return subproblems_.size();
  }
 
protected:
  static void indent(type_level level)
  {
    std::cout << "level: " << level;
    for(type_level l = 0; l < level; ++l) {
      std::cout << "  ";
    }
  }

private:
  /** Gets the already-calculated subproblem solution, if any.
   * @result true if the subproblem solution was in the cache.
   */
  bool get_cached_subproblem(type_subproblem& subproblem, T_value_types... values) const override {
    //std::cout << "get_cached_subproblem(): i=" << i << ", j=" << j << std::endl;
    const type_values key(values...);
    const auto iter = subproblems_.find(key);
    if (iter == subproblems_.end()) {
      subproblem = type_subproblem();
      return false;
    }

    //std::cout << "get_cached_subproblem(): returning cache for i=" << i << ", j=" << j << std::endl;
    subproblem = iter->second;
    return true;
  }

  void set_subproblem(const type_subproblem& subproblem, T_value_types... values) const override {
    const type_values key(values...);
    subproblems_[key] = subproblem;
  }

  void clear() override {
    type_base::clear();
    subproblems_.clear();
  }

private:
  //Map of values to subproblems:
  using type_map_subproblems = std::unordered_map<type_values, type_subproblem, utils::hash_tuple::hash<type_values>>;
  mutable type_map_subproblems subproblems_;
};

} //namespace murraycdp

#endif //MURRAYCDP_DP_BOTTOM_UP_BASE_H
