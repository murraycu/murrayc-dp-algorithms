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

#ifndef _MURRAYCDP_TUPLE_PRINTER_H
#define _MURRAYCDP_TUPLE_PRINTER_H

// Based on example code from here:
// http://en.cppreference.com/w/cpp/utility/tuple/tuple_cat

template<class Tuple, std::size_t N>
class TuplePrinter {
public:
  static void print(const Tuple& t) 
  {
    TuplePrinter<Tuple, N-1>::print(t);
    std::cout << ", " << std::get<N-1>(t);
  }
};

/// @cond DOXYGEN_HIDDEN_SYMBOLS
template<class Tuple>
class TuplePrinter<Tuple, 1> {
public:
  static void print(const Tuple& t)  {
    std::cout << std::get<0>(t);
  }
};
/// @endcond DOXYGEN_HIDDEN_SYMBOLS

#endif //_MURRAYCDP_TUPLE_PRINTER_H
