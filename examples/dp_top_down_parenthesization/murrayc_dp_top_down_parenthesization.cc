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

#include <murraycdp/dp_top_down_base.h>

using uint = unsigned int;

/**
 * A "3 x 2" matrix has 3 rows and 2 columns.
 */
class MatrixDimensions {
public:
  std::string name;
  uint rows_count = 0;
  uint columns_count = 0;

  std::string to_string() const {
    return name + "(" + std::to_string(rows_count) + "x" +
      std::to_string(columns_count) + ")";
  }
};

using VecMatrixDimensions = std::vector<MatrixDimensions>;

using Range = std::pair<uint, uint>;

/** This is the cost of the "substring" i:j.
 */
class SubProblem {
public:
  std::string solution;
  uint cost = 0;
  MatrixDimensions dimensions;
};

/** Based on the Parenthesization problem for matrix multiplication here
 * https://www.rows_countoutube.com/watch?v=ocZMDMZwhCY
 * and
 * http://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-006-introduction-to-algorithms-fall-2011/lecture-videos/MIT6_006F11_lec21.pdf
 *
 * See also: https://en.wikipedia.org/wiki/Matrix_chain_multiplication
 */
class DpParenthesization
  : public murraycdp::DpTopDownBase<SubProblem, uint, uint> {
public:
  DpParenthesization(const VecMatrixDimensions& items)
  : items_(items)
  {}

private:

  type_subproblem calc_subproblem(type_level level, uint i, uint j) const override {
    //indent(level);
    //std::cout << "calc_subproblem: i=" << i << ", j=" << j << std::endl;
    SubProblem result;

    if (j == i) {
      //Base case:
      result.cost = 0;
      result.dimensions = items_[i];
      result.solution = items_[i].name;
      //indent(level);
      //std::cout << "returning base cost: " << result.cost << " for: " << result.dimensions.to_string() << std::endl;
      return result;
    }
    
    uint min_cost = std::numeric_limits<uint>::max();
    uint min_k = 0;
    for (uint k = i + 1; k <= j; ++k) {
      const auto sub_i_to_k = get_subproblem(level, i, k - 1);
      const auto sub_k_to_j = get_subproblem(level, k, j);
      const auto cost = sub_i_to_k.cost + sub_k_to_j.cost +
        calc_cost(sub_i_to_k.dimensions, sub_k_to_j.dimensions);
      if (cost < min_cost) {
        min_cost = cost;
        min_k = k;
      }
    }
    
    const auto sub_i_to_k = get_subproblem(level, i, min_k-1);
    const auto sub_k_to_j = get_subproblem(level, min_k, j);
    result.cost = min_cost;
    result.dimensions = calc_dimensions(sub_i_to_k.dimensions, sub_k_to_j.dimensions);
    result.solution = "(" + sub_i_to_k.solution + sub_k_to_j.solution + ")";

    //indent(level);
    //std::cout << "returning cost: " << result.cost << " for: " << result.dimensions.to_string() << std::endl;
    return result;
  }

  void get_goal_cell(unsigned int& i, unsigned int& j) const override {
     i = 0;
     j = items_.size() - 1;
  }

  static uint calc_cost(const MatrixDimensions& a, const MatrixDimensions& b) {
    // a p x q matrix multiplied by a q x r makes a p x r matrix,
    // and is only possible if r == q.
    if (a.columns_count != b.rows_count) {
      std::cout << "calc_cost: Invalid dimensions for "
        << a.to_string() << " and " << b.to_string() 
        << ": a.columns_count=" << a.rows_count << ", b.rows_count=" << b.columns_count << std::endl;
      return std::numeric_limits<uint>::max();
    }

    //The cost is p * q * r:
    return a.rows_count * a.columns_count * b.columns_count;
  }

  static MatrixDimensions calc_dimensions(const MatrixDimensions& a, const MatrixDimensions& b) {
    return MatrixDimensions{a.name + b.name, a.rows_count, b.columns_count};
  }
  
  VecMatrixDimensions items_;
};

int main() {
   const VecMatrixDimensions vec( {
    {"A", 10, 30},
    {"B", 30, 5},
    {"C", 5, 60}});
  
  DpParenthesization dp(vec);
  const auto result = dp.calc();
  std::cout << "result: " << result.cost << ": " << result.solution << std::endl;

  //This shows that we calculate all sub-problems, missing none,
  //so this top-down version has no advantage over the bottom-up version.
  //However, the bottom-up version can discard older (> i-2) results along the
  //way.
  std::cout << "Count of sub-problems calculated: " <<
    dp.count_cached_sub_problems() << std::endl;

  assert(result.cost == 4500);
  assert(result.solution == "((AB)C)");

  return EXIT_SUCCESS;
}
