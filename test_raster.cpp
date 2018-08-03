#ifdef POPS_TEST

/*
 * Simple compilation test for the PoPS Raster class.
 *
 * Copyright (C) 2018 by the authors.
 *
 * Authors: Vaclav Petras <wenzeslaus gmail com>
 *
 * This file is part of PoPS.

 * PoPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.

 * PoPS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with PoPS. If not, see <https://www.gnu.org/licenses/>.
 */

#include "raster.hpp"
#include "simulation.hpp"

#include <map>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <string>

using std::string;
using std::cout;
using std::cerr;
using std::endl;

void test_constructor_by_type()
{
    Raster<int> a(10, 10, 1, 1);
    Raster<float> b(10, 10, 1, 1);
    Raster<double> c(10, 10, 1, 1);
}

void test_initializer_and_output()
{
    Raster<int> a = {{1, 2},
                     {3, 4},
                     {5, 6}};
    std::cout << a;
}

void test_equal_operator()
{
    Raster<int> a = {{1, 2}, {3, 4}, {5, 6}};
    Raster<int> b = {{1, 2}, {3, 4}, {5, 6}};
    if (a == b)
        std::cout << "Operator equal works" << std::endl;
    else
        std::cout << "Operator equal does not work" << std::endl;
}

void test_not_equal_operator()
{
    Raster<int> a = {{1, 2}, {3, 4}, {5, 6}};
    Raster<int> b = {{1, 2}, {3, 5}, {5, 6}};
    if (a != b)
        std::cout << "Operator not-equal works" << std::endl;
    else
        std::cout << "Operator not-equal does not work" << std::endl;
}

void test_plus_operator()
{
    Raster<int> d = {{1, 2}, {3, 4}, {5, 6}};
    Raster<int> e = {{8, 9}, {10, 11}, {12, 13}};
    auto f = d + e;
    std::cout << f;
}

void test_sqrt()
{
    Raster<int> a = {{16, 25}, {4, 9}};
    Raster<int> b = {{4, 5}, {2, 3}};
    auto c = sqrt(a);
    std::cout << "sqrt function: ";
    if (b == c)
        std::cout << "OK" << std::endl;
    else
        std::cout << "\n" << a << "!=\n" << b << std::endl;
}

int main(int argc, char *argv[])
{
    test_constructor_by_type();
    test_initializer_and_output();
    test_equal_operator();
    test_not_equal_operator();
    test_sqrt();

    return 0;
}

#endif  // POPS_TEST
