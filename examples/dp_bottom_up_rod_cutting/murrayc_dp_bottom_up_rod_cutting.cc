/* Copyright (C) 2016 Murray Cumming
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

/**
 * This is based on the Rod Cutting problem in section 15.1 of CLRS.
 */
class DpRodCutting
  : public murraycdp::DpBottomUpBase<
      100 /* TODO */ /* count of subproblems to keep, used in calc_subproblem() */,
      std::size_t, // sub problem type
      std::size_t  // i
      > {
public:
  using LengthPrices = std::vector<std::pair<std::size_t, std::size_t>>;

  explicit DpRodCutting(const LengthPrices& length_prices, std::size_t length)
  : DpBottomUpBase(length + 1), // DpBottomUpBase without the
    // specialization is apparently allowed.
    length_prices_(length_prices),
    length_(length) {
    }


private:
  type_subproblem
  calc_subproblem(type_base::type_level level, std::size_t i) const override {
    // Base cases:
    if (i == 0) {
      // 0 cost for a 0-length rod:
      return 0;
    }

    // Get the max of the prices resulting from various initial cuts:
    type_subproblem result = 0;
    for (std::size_t j = 1; j <= i; ++j) {
      // The price when cutting one i-j piece and whatever is optimal for the rest:
      const auto subproblem = length_prices_[j - 1].second + get_subproblem(level, i - j);
      if (subproblem > result) {
        result = subproblem;
      }
    }

    //std::cout << "i=" << i << ", result=" << result << std::endl;
    return result;
  }

  void
  get_goal_cell(std::size_t& i) const override {
    i = length_;
  }

  const LengthPrices length_prices_;
  const std::size_t length_;
};

static
void test(const DpRodCutting::LengthPrices& length_prices, std::size_t length, std::size_t expected_price) {
  DpRodCutting dp(length_prices, length);
  assert(dp.calc() == expected_price);
}

int
main() {
  const DpRodCutting::LengthPrices length_prices = {
    {1, 1},
    {2, 5},
    {3, 8},
    {4, 9},
    {5, 10},
    {6, 17},
    {7, 17},
    {8, 20},
    {9, 24},
    {10, 30}};
  test(length_prices, 4, 10);
  test(length_prices, 5, 13);
  test(length_prices, 9, 25);

  return EXIT_SUCCESS;
}
