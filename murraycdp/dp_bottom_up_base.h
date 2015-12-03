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
#include <murraycdp/utils/circular_vector.h>
#include <murraycdp/utils/vector_of_vectors.h>
#include <murraycdp/utils/tuple_cdr.h>

namespace murraycdp {

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
  using type_subproblems = typename utils::vector_of_vectors<T_subproblem, 0>::type;
  using type_base = DpBase<T_subproblem, T_value_types...>;
  using type_values = typename type_base::type_values;

  /**
   * @param The number of i values to calculate the subproblem for.
   * @pram The number of j values to calculate the subproblem for.
   */
  DpBottomUpBase(T_value_types... value_counts)
  : subproblems_(T_COUNT_SUBPROBLEMS_TO_KEEP),
    value_counts_(value_counts...)
  {
    const auto value_counts_without_i = utils::cdr(value_counts_);
    constexpr std::size_t tuple_size =
      std::tuple_size<decltype(value_counts_without_i)>::value;
    call_resize_sub_vectors_with_tuple(
      value_counts_without_i,
      std::make_index_sequence<tuple_size>());
  }
  
  DpBottomUpBase(const DpBottomUpBase& src) = delete;
  DpBottomUpBase& operator=(const DpBottomUpBase& src) = delete;

  DpBottomUpBase(DpBottomUpBase&& src) = delete;
  DpBottomUpBase& operator=(DpBottomUpBase&& src) = delete;

  type_subproblem calc() override {
    //TODO: subproblems_.clear();

    typename type_base::type_level level = 0; //unused

    const auto i_count = std::get<0>(value_counts_);
    for (unsigned int i = 0; i < i_count; ++i) {
#if defined(MURRAYC_DP_DEBUG_OUTPUT)
      std::cout << "i=" << std::setw(2) << i << ": ";
#endif

      const std::size_t j_count_ = std::get<1>(value_counts_);
      type_subproblems& subproblems_i = subproblems_.get_at_offset_from_start(i);
      utils::for_vector_of_vectors(subproblems_i,
        [this, level, i] (auto j) {
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

    //We cannot do this to pass the output parameters to get_goal_cell():
    //  T_type_values... goal
    //but we can pass a std::tuple<> based on T_type_values... 
    //and that will then be passed as individual parameters when we unpack it
    //via std::index_sequence.
    type_values goals;
    this->get_goal_cell_call_with_tuple(goals,
      std::index_sequence_for<T_value_types...>());
    //std::cout << "goal_i=" << goal_i << ", goal_j=" << goal_j << std::endl;
    //std::cout << "calc: " << std::get<0>(goals) << std::endl;

    return this->get_subproblem_call_with_tuple(level, goals,
      std::index_sequence_for<T_value_types...>());
  }

private:
  /** Gets the already-calculated subproblem solution, if any.
   * @result true if the subproblem solution was in the cache.
   */
  bool get_cached_subproblem(type_subproblem& subproblem, T_value_types... values) const override {
    //std::cout << "get_cached_subproblem(): i=" << i << ", j=" << j << std::endl;

    const unsigned int i = std::get<0>(type_values(values...)); //TODO
    const unsigned int j = std::get<1>(type_values(values...)); //TODO
    const type_subproblems& subproblems_i = subproblems_.get_at_offset_from_start(i);
    subproblem = subproblems_i[j];

    //std::cout << "get_cached_subproblem(): returning cache for i=" << i << ", j=" << j << std::endl;

    return true; //TODO: Detect whether it has been calculated?
  }

  void set_subproblem(const type_subproblem& subproblem, T_value_types... values) const override {
    //TODO: Performance: Avoid repeated calls to get_at_offset_from_start(),
    //while still keeping the code generic:
    const unsigned int i = std::get<0>(type_values(values...)); //TODO
    const unsigned int j = std::get<1>(type_values(values...)); //TODO
    type_subproblems& subproblems_i = subproblems_.get_at_offset_from_start(i);
    subproblems_i[j] = subproblem;
  }

  template<typename... T_sizes>
  void resize_sub_vectors(T_sizes... sizes) {
    this->subproblems_.foreach(
      [sizes...] (type_subproblems& sub_item) {
        utils::resize_vector_of_vectors(sub_item, sizes...);
      }
    );
  }

  template<typename T_tuple, std::size_t... Is>
  void call_resize_sub_vectors_with_tuple(
    T_tuple& tuple,
    std::index_sequence<Is...>) {
    resize_sub_vectors(std::get<Is>(tuple)...);
  }

protected:
  using type_vec_subproblems = utils::circular_vector<type_subproblems>;
  mutable type_vec_subproblems subproblems_;
  type_values value_counts_;
};

} //namespace murraycdp

#endif //MURRAYCDP_DP_BOTTOM_UP_BASE_H
