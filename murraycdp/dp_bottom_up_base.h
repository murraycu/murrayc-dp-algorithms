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

#ifndef MURRAYCDP_DP_BOTTOM_UP_BASE_H
#define MURRAYCDP_DP_BOTTOM_UP_BASE_H

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include <experimental/tuple> //For apply().

#include <murraycdp/dp_base.h>
#include <murraycdp/utils/circular_vector.h>
#include <murraycdp/utils/vector_of_vectors.h>
#include <tuple-utils/tuple_cdr.h>
#include <tuple-utils/tuple_interlace.h>

namespace murraycdp {

//#define MURRAYC_DP_DEBUG_OUTPUT = 1;

/** A base class for a 2D bottom-up dynamic programming algorithm.
 *
 * Override this, implementing calc_subproblem(), and then call calc() to get
 * the
 * overall solution.
 * @tparam T_COUNT_SUBPROBLEMS_TO_KEEP The number of previous i values that
 * calc_subproblem() needs to use, or 0 to keep subproblems for all previous i
 * values.
 * @tparam T_subproblem The type of the subproblem solution, such as unsigned
 * int,
 * or a custom class containing a value and a partial path.
 * @tparam T_value_types The types of the parameters for the calc_subproblem()
 * method.
 */
template <unsigned int T_COUNT_SUBPROBLEMS_TO_KEEP, typename T_subproblem,
  typename... T_value_types>
class DpBottomUpBase : public DpBase<T_subproblem, T_value_types...> {
public:
  using type_base = DpBase<T_subproblem, T_value_types...>;
  using type_subproblem = T_subproblem;
  using type_values = typename type_base::type_values;
  using type_level = typename type_base::type_level;

  using type_subproblems = typename utils::vector_of_vectors<T_subproblem,
    sizeof...(T_value_types)-1>::type;

  /**
   * @param The number of i values to calculate the subproblem for.
   * @pram The number of j values to calculate the subproblem for.
   */
  DpBottomUpBase(typename std::decay<T_value_types>::type... value_counts)
  : subproblems_(T_COUNT_SUBPROBLEMS_TO_KEEP == 0 ? std::get<0>(std::make_tuple(value_counts...)) : T_COUNT_SUBPROBLEMS_TO_KEEP),
    value_counts_(value_counts...) {
    const auto value_counts_without_i = tupleutils::tuple_cdr(value_counts_);
    constexpr auto tuple_size =
      std::tuple_size<decltype(value_counts_without_i)>::value;
    if (tuple_size > 0) {
      call_resize_sub_vectors_with_tuple(
        value_counts_without_i, std::make_index_sequence<tuple_size>());
    }
  }

  DpBottomUpBase(const DpBottomUpBase& src) = delete;
  DpBottomUpBase&
  operator=(const DpBottomUpBase& src) = delete;

  DpBottomUpBase(DpBottomUpBase&& src) noexcept = delete;
  DpBottomUpBase&
  operator=(DpBottomUpBase&& src) noexcept = delete;

  type_subproblem
  calc() override {
    subproblems_.clear();

    type_level level = 0; // unused

    // For some reason this always sets depth to 1:
    // const bool depth = sizeof...(T_value_types);
    // so we use std::tuple_size() instead.
    constexpr auto depth =
      std::tuple_size<type_values>::value; // sizeof...(T_value_types);
    // std::cout << "debug: depth=" << depth << std::endl;

    const auto i_count = std::get<0>(value_counts_);
    for (unsigned int i = 0; i < i_count; ++i) {
#if defined(MURRAYC_DP_DEBUG_OUTPUT)
      std::cout << "i=" << std::setw(2) << i << ": ";
#endif
      if (depth == 1) {
        // Call it via a tuple,
        // to avoid a compilation error when depth is not 1:
        type_values values;
        std::get<0>(values) = i;

        const auto subproblem = std::experimental::apply(
          [this, level](T_value_types... the_values) {
            return this->calc_subproblem(level, the_values...);
          },
          values);

        std::experimental::apply(
          [this, subproblem](T_value_types... the_values) {
            return this->set_subproblem(subproblem, the_values...);
          },
          values);
      } else {
        type_subproblems& subproblems_i =
          subproblems_.get_at_offset_from_start(i);

        using type_values_counts_without_i =
          typename tupleutils::tuple_type_cdr<type_values>::type;
        // TODO: Why can't this be const when it is tuple<> (with no element
        // types):
        type_values_counts_without_i
          values_starts_without_i; // TODO: explicitly initialize to 0.
        const auto value_counts_without_i = tupleutils::tuple_cdr(value_counts_);

        const auto values_start_end = tupleutils::tuple_interlace(
          values_starts_without_i, value_counts_without_i);

        constexpr std::size_t values_start_end_size =
          std::tuple_size<decltype(values_start_end)>::value;

        call_for_sub_vectors_with_tuple(subproblems_i,
          [this, level, i](auto... params) {
            const auto subproblem = this->calc_subproblem(level, i, params...);
            this->set_subproblem(subproblem, i, params...);

#if defined(MURRAYC_DP_DEBUG_OUTPUT)
// if (j != 0) {
//  std::cout << ", ";
//}
// std::cout << std::setw(2) << subproblem.cost;
#endif
          },
          values_start_end, std::make_index_sequence<values_start_end_size>());

#if defined(MURRAYC_DP_DEBUG_OUTPUT)
        std::cout << std::endl;
#endif
      }

      // subproblems_i will then be read as subproblems_i_minus_1;
      // and subproblems_i_minus_1 will be filled as subproblems_i;
      subproblems_.step();
    }

    // We cannot do this to pass the output parameters to get_goal_cell():
    //  T_type_values... goal
    // but we can pass a std::tuple<> based on T_type_values...
    // and that will then be passed as individual parameters when we unpack it
    // via std::index_sequence.

    /*
    type_values goals;
    std::experimental::apply(
      [this] (T_value_types... the_values) {
        return this->get_goal_cell(the_values...);
      },
      goals);
    */

    type_values goals;
    this->get_goal_cell_call_with_tuple(
      goals, std::index_sequence_for<T_value_types...>());
    // std::cout << "goal_i=" << goal_i << ", goal_j=" << goal_j << std::endl;
    // std::cout << "calc: " << std::get<0>(goals) << std::endl;

    return std::experimental::apply(
      [this, level](T_value_types... the_values) {
        return this->get_subproblem(level, the_values...);
      },
      goals);
  }

private:
  /** Gets the already-calculated subproblem solution, if any.
   * @result true if the subproblem solution was in the cache.
   */
  bool
  get_cached_subproblem(
    type_subproblem& subproblem, T_value_types... values) const override {
    const std::tuple<T_value_types...> values_tuple(values...);

    const auto i = std::get<0>(values_tuple);
    const type_subproblems& subproblems_i =
      subproblems_.get_at_offset_from_start(i);

    const auto values_without_i = tupleutils::tuple_cdr(values_tuple);
    constexpr auto tuple_size =
      std::tuple_size<decltype(values_without_i)>::value;
    subproblem = call_get_at_sub_vectors_with_tuple(
      subproblems_i, values_without_i, std::make_index_sequence<tuple_size>());

    // std::cout << "get_cached_subproblem(): returning cache for i=" << i << ",
    // j=" << j << std::endl;

    return true; // TODO: Detect whether it has been calculated?
  }

  void
  set_subproblem(
    const type_subproblem& subproblem, T_value_types... values) const override {
    // TODO: Performance: Avoid repeated calls to get_at_offset_from_start(),
    // while still keeping the code generic.

    const std::tuple<T_value_types...> values_tuple(values...);
    const auto i = std::get<0>(values_tuple);
    type_subproblems& subproblems_i = subproblems_.get_at_offset_from_start(i);

    const auto values_without_i = tupleutils::tuple_cdr(values_tuple);
    constexpr auto tuple_size =
      std::tuple_size<decltype(values_without_i)>::value;
    call_get_at_sub_vectors_with_tuple(subproblems_i, values_without_i,
      std::make_index_sequence<tuple_size>()) = subproblem;
  }

  template <typename... T_sizes>
  void resize_sub_vectors(T_sizes... sizes) {
    this->subproblems_.foreach ([sizes...](type_subproblems& sub_item) {
      utils::resize_vector_of_vectors(sub_item, sizes...);
    });
  }

  template <typename T_tuple, std::size_t... Is>
  void
  call_resize_sub_vectors_with_tuple(
    T_tuple& tuple, std::index_sequence<Is...>) {
    resize_sub_vectors(std::get<Is>(tuple)...);
  }

  template <typename T_vector, typename T_tuple, std::size_t... Is>
  decltype(auto)
  call_get_at_sub_vectors_with_tuple(
    T_vector&& vector, T_tuple&& tuple, std::index_sequence<Is...>) const {
    return murraycdp::utils::get_at_vector_of_vectors<type_subproblem>(
      std::forward<T_vector>(vector), std::get<Is>(std::forward<T_tuple>(tuple))...);
  }

  template <typename T_vector, typename T_function, typename T_tuple,
    std::size_t... Is>
  void
  call_for_sub_vectors_with_tuple(T_vector&& vector, T_function f,
    T_tuple&& tuple, std::index_sequence<Is...>) const {
    murraycdp::utils::for_vector_of_vectors(std::forward<T_vector>(vector), f, std::get<Is>(std::forward<T_tuple>(tuple))...);
  }

protected:
  using type_vec_subproblems = utils::circular_vector<type_subproblems>;
  mutable type_vec_subproblems subproblems_;
  const type_values value_counts_;
};

} // namespace murraycdp

#endif // MURRAYCDP_DP_BOTTOM_UP_BASE_H
