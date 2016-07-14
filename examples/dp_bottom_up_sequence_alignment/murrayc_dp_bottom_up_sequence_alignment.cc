/* Copyright (C) 2015, 2016 Murray Cumming
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

#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include <murraycdp/dp_bottom_up_base.h>

class SubSolution {
public:
  using type_items = std::string;
  using type_value = unsigned int;

  // static constexpr type_value COIN_COUNT_INFINITY =
  // std::numeric_limits<type_value>::max();

  SubSolution() : value(0) {}

  explicit SubSolution(type_value value_in, const type_items& solution_a_in, const type_items& solution_b_in)
  : value(value_in),
    solution_a(solution_a_in),
    solution_b(solution_b_in) {}

  SubSolution(const SubSolution& src) = default;
  SubSolution&
  operator=(const SubSolution& src) = default;

  SubSolution(SubSolution&& src) noexcept = default;
  SubSolution&
  operator=(SubSolution&& src) noexcept = default;

  type_value value;
  type_items solution_a, solution_b;
};

class DpSequenceAlignment
  : public murraycdp::DpBottomUpBase<
      2, // count of subproblems to keep.
      SubSolution,
      SubSolution::type_items::size_type,
      SubSolution::type_items::size_type> {
public:
  using type_value = unsigned int;
  using type_items = SubSolution::type_items;
  using type_size = type_items::size_type;

  DpSequenceAlignment(const type_items& a, const type_items& b)
  : DpBottomUpBase(a.size() + 1, b.size() + 1),
    a_(a), b_(b) {}

private:
  //Prefer mismatches to gaps, but prefer matches to either.
  static constexpr type_value MATCH_COST = 1;
  static constexpr type_value GAP_COST = 3;
  static constexpr type_value MISMATCH_COST = 2;
  static constexpr char GAP_CHAR = '-';

  type_value match_cost(type_size a_count, type_size b_count) const {
    if (a_count >= a_.size()) {
      return GAP_COST;
    } else if (b_count >= b_.size()) {
      return GAP_COST;
    } else if (a_[a_count] == b_[b_count]) {
      return MATCH_COST;
    } else {
      return MISMATCH_COST;
    }
  }

  char get_char_a(type_size a_count) const {
    if (a_count >= a_.size()) {
      return GAP_CHAR;
    } else {
      return a_[a_count];
    }
  }

  char get_char_b(type_size b_count) const {
    if (b_count >= b_.size()) {
      return GAP_CHAR;
    } else {
      return b_[b_count];
    }
  }

  static
  type_items get_gap(type_size count) {
    if (count == 0) {
      return "";
    } else {
      return std::string(count, GAP_CHAR);
    }
  }


  type_subproblem
  calc_subproblem(type_level level, type_size a_count, type_size b_count) const override {
    //std::cout << "calc_subproblem: a_count=" << a_count << ", b_count=" << b_count << std::endl;

    if (a_count == 0) {
      const auto result = type_subproblem(b_count  * GAP_COST, get_gap(b_count), b_.substr(0, b_count));
      //std::cout << "  base a=0: result: a:" << result.solution_a << ", b:" << result.solution_b << "]" << std::endl;
      return result;
    }

    if (b_count == 0) {
      const auto result = type_subproblem(a_count * GAP_COST, a_.substr(0, a_count),  get_gap(a_count));
      //std::cout << "  base b=0: result: a:" << result.solution_a << ", b:" << result.solution_b << "]" << std::endl;
      return result;
    }

    //TODO: Storing the a and b solutions for each sub-solutin is a huge waste.
    //TODO: These should instead be reconstructed.
    auto case1 = get_subproblem(level, a_count - 1, b_count - 1);
    case1.value += match_cost(a_count, b_count);
    case1.solution_a += get_char_a(a_count - 1); //Could be a mismatch or gap at end.
    case1.solution_b += get_char_b(b_count - 1); //Could be a mismatch or gap at end.
    //std::cout << "  case 1: a:" << case1.solution_a << ", b:" << case1.solution_b << "]" << std::endl;
    //assert(case1.solution_a.size() == case1.solution_b.size());
    auto case2 = get_subproblem(level, a_count - 1, b_count);
    case2.value += GAP_COST;
    case2.solution_a += GAP_CHAR;
    case2.solution_b += get_char_b(b_count - 1);
    //std::cout << "  case 2: a:" << case2.solution_a << ", b:" << case2.solution_b << "]" << std::endl;
    //assert(case2.solution_a.size() == case2.solution_b.size());
    auto case3 = get_subproblem(level, a_count, b_count - 1);
    case3.value += GAP_COST;
    case3.solution_a += get_char_a(a_count - 1); //Could be a mismatch.
    case3.solution_b += GAP_CHAR;
    //std::cout << "  case 3 a:" << case3.solution_a << ", b:" << case3.solution_b << "]" << std::endl;
    //assert(case3.solution_a.size() == case3.solution_b.size());

    const auto cases = {case1, case2, case3};
    const auto min_case_iter = std::min_element(std::begin(cases), std::end(cases),
      [] (const auto& x, const auto& y) {
        return x.value < y.value;
      });
    const auto& min_case = *min_case_iter;
    //std::cout << "  min: a:" << min_case.solution_a << ", b:" << min_case.solution_b << "]" << std::endl;
    //assert(min_case.solution_a.size() == min_case.solution_b.size());
    return min_case;
  }

  void
  get_goal_cell(type_size& a_count, type_size& b_count) const override {
    // The answer is in the last-calculated cell:
    a_count = a_.size();
    b_count = b_.size();
  }

  const type_items a_, b_;
};

int
main() {
  const DpSequenceAlignment::type_items a = "GCCCTAGCG";
  const DpSequenceAlignment::type_items b = "GCGCAATG";

  std::cout << "Problem:" << std::endl
    << "  a: " << a << std::endl
    << "  b: " << b << std::endl;

  DpSequenceAlignment dp(a, b);
  const auto result = dp.calc();

  std::cout << "solution: value: " << result.value << std::endl
    << "with solution: " << std::endl
    << "  a: [" << result.solution_a << "]" << std::endl
    << "  b: [" << result.solution_b << "]" << std::endl;

  // Uncomment to show the sequence: dp.print_subproblem_sequence();

  assert(result.value == 16);
  assert(result.solution_a == "GCC-TAGCG");
  assert(result.solution_b == "GCGGCAATG"); //TODO: This doesn't seem to be correct. The GG isn't in the original string.

  return EXIT_SUCCESS;
}
