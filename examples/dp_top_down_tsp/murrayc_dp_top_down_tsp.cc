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
#include <cmath>
#include <algorithm>
#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <murraycdp/dp_top_down_base.h>

typedef double type_length;


const type_length LENGTH_INFINITY = std::numeric_limits<type_length>::max();

class Vertex
{
public:
  Vertex()
  : x(0), y(0)
  {}

  Vertex(type_length x_in, type_length y_in)
  : x(x_in), y(y_in)
  {}

  Vertex(const Vertex& src) = default;
  Vertex& operator=(const Vertex& src) = default;

  Vertex(Vertex&& src) = default;
  Vertex& operator=(Vertex&& src) = default;

  type_length x, y;
};

using type_vec_nodes = std::vector<Vertex>;
using type_node_id = type_vec_nodes::size_type;
using type_vec_node_ids = std::vector<type_node_id>;

const type_node_id NODE_FIRST = 0; //0-indexed

static
inline type_length calc_distance(const Vertex& a, const Vertex& b)
{
  return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

using type_subproblem = type_length;

static
inline type_length get_sub_problem_base_case(const type_vec_node_ids& subset, type_node_id start_node_id)
{
  //Base case:
  if((subset.size() == 1) && (subset[0] == start_node_id))
  {
    //We are already at the start and can get back to the
    //start in 0 steps, visiting start only once.
    //std::cout << ": 0" << std::endl;
    return 0;
  }
  else
  {
    //We cannot go from the start to the start, visiting intermediate
    //nodes, visiting start only once.
    //std::cout << ": LENGTH_INFINITY" << std::endl;
    return LENGTH_INFINITY;
  }
}

using type_subset_id = std::string;

static
inline type_subset_id get_subset_id(const type_vec_node_ids& vertex_ids)
{
  type_subset_id result;
  //This seems to make it slower: result.reserve(vertex_ids.size() * 2);
  char buffer[10];
  for(const auto& id : vertex_ids)
  {
    if(!result.empty())
      result += ",";

    std::snprintf(buffer, sizeof(buffer), "%lu", id);
    result += buffer;
  }

  return result;
}


//TODO: Avoid declaring something in std:
namespace std {

template<>
struct hash<type_vec_node_ids> {
  size_t operator ()(const type_vec_node_ids& value) const {
      //TODO: Performance:
      const auto subset_id = get_subset_id(value);
      return std::hash<type_subset_id>()(subset_id);
  }
};

} //namespace std

class DpTsp
  : public murraycdp::DpTopDownBase<
      type_subproblem,
      std::size_t, // m: nodes_count
      type_vec_node_ids,
      std::size_t // j: each node id in the subset.
      > {
public:
  DpTsp(const type_vec_nodes& vertices)
  : vertices_(vertices),
    start_node_id_(NODE_FIRST),
    start_node_(vertices_[start_node_id_])
  {
    //A vector of just the vertex IDs:
    const auto vertices_count = vertices.size() - NODE_FIRST;
    vertex_ids_.resize(vertices_count);
    for(type_node_id i = NODE_FIRST; i <= vertices_count; ++i)
    {
      vertex_ids_[i - NODE_FIRST] = i;
    }
  }

  type_subproblem calc() override {
    clear();
    const type_level level = 0;

    const auto& full_subset = vertex_ids_;

    const int i = vertices_.size();
    type_length min = LENGTH_INFINITY;
    for(const auto j : vertex_ids_) {
      if(j == start_node_id_)
        continue;

      const auto length = get_subproblem(level, i, full_subset, j);
      //std::cout << "length=" << length << std::endl;
      const auto Cj1 =
        calc_distance(vertices_[j], start_node_);
      //std::cout << "Cj1=" << Cj1 << std::endl;
      const auto full_length = length + Cj1;

      //std::cout << "  : j=" << j << ", full_length=" << full_length << std::endl;

      if(full_length < min)
        min = full_length;
    }

    return min;
  }

private:
  using uint = unsigned int;
  
  type_subproblem calc_subproblem(type_level level, std::size_t m, type_vec_node_ids subset, std::size_t j) const override {  
    if(j == start_node_id_)
      return 0; //TODO

    auto subset_without_j = subset;
    subset_without_j.erase(
      std::remove(subset_without_j.begin(), subset_without_j.end(), j),
      subset_without_j.end());

    type_length min = LENGTH_INFINITY;

    //std::cout << ": j=" << j << ", subset_without_j = {" << get_subset_id(subset_without_j) << "}" << std::endl;

    for(const auto k : subset)
    {
      if(k == j)
        continue;

      //std::cout << "  j= " << j << ", k=" << k << ", subset_without_j = {" << get_subset_id(subset_without_j) << "}" << std::endl;

      type_length length = 0;
      if(k == start_node_id_)
      {
        //Base cases, for when the destination == start.
        length = get_sub_problem_base_case(subset_without_j, start_node_id_);
      }
      else
      {
        length = get_subproblem(level, m - 1, subset_without_j, k);
      }

      const auto Ckj =
        calc_distance(vertices_[k], vertices_[j]);
      //std::cout << "  using A[{" << subset_without_j_id << "}, " << k << "] = " << length << std::endl;
      //std::cout << "  using Ckj = " << Ckj << std::endl;

      const type_length sub_length = length + Ckj;
      //std::cout << "  sub_length=" << sub_length << std::endl;

      if(sub_length < min)
        min = sub_length;
    }

    return min;
  }

  void get_goal_cell(std::size_t& /* m */, type_vec_node_ids& /* subset */, std::size_t& /* j */) const override {
     //We don't use this.
     //Instead we override coal() and get a minimum over several cells.
  }

  const type_vec_nodes vertices_;
  type_vec_node_ids vertex_ids_;

  const type_node_id start_node_id_;
  const Vertex& start_node_;
};

/** Compare doubles @a a and @a b to @a N_decimal_places decimal places.
 * @tparam N_decimal_places The number of decimal places to use when comparing.
 */
template<std::size_t N_decimal_places>
bool doubles_are_equal(double a, double b) {
  const double multiplier = std::pow(10, N_decimal_places);
  return std::round(a * multiplier) == std::round(b * multiplier);
}

int main() {
  /*
  const type_vec_nodes vertices = {
    {0, 0},
    {0, 2},
    {0, 4},
    {0, 6}};
  */

  const type_vec_nodes vertices = {
    {0.328521, 0.354889},
    {0.832, 0.832126},
    {0.680803, 0.865528},
    {0.734854, 0.38191},
    {0.14439, 0.985427},
    {0.90997, 0.587277},
    {0.408464, 0.136019},
    {0.896868, 0.916344},
    {0.991904, 0.383134},
    {0.451197, 0.741267},
    {0.825205, 0.761446},
    {0.421804, 0.0374936},
    {0.332503, 0.26436},
    {0.107117, 0.51559},
    {0.845227, 0.21359},
    {0.880095, 0.593086},
    {0.454773, 0.834355},
    {0.7464, 0.363176}};

  DpTsp dp(vertices);
  std::cout << "vertices count: " << vertices.size() << std::endl;
  const auto result = dp.calc();
  std::cout << "result: " << std::setprecision(10) << result << std::endl;

  assert(doubles_are_equal<5>(result, 3.50116));

  std::cout << "Count of sub-problems calculated: " <<
    dp.count_cached_sub_problems() << std::endl;
  
  return EXIT_SUCCESS;
}
