/**
 * Very simple unit test for the number class defined in number.hpp.
 *
 * This test ensures that all arithmetic operations (plus, minus, multiply, divide, modulus) match
 * native arithmetic from 0-1000 for both operands across a wide variety of template arguments that
 * control how the number class works.
 *
 * This is by no means a comprehensive test, but it is a good sanity check.
 */

#include <functional>
#include <iostream>
#include <limits>
#include <string_view>

#include "number.hpp"

template<typename T, typename F>
bool test_op(int min_i, int max_i, int min_j, int max_j, F&& func, std::string_view op) {
    for (unsigned i = min_i; i <= static_cast<unsigned>(max_i); ++i) {
        for (unsigned j = min_j; j <= (max_j == -1 ? i : max_j); ++j) {
            unsigned actual = static_cast<unsigned>(func(T(i), T(j)));
            unsigned expected = func(i, j);
            if (actual != expected) {
                std::cerr << "FAILED TEST CASE: " << i << " " << op << " " << j << ", "
                          << "expected " << expected << " but got " << actual << ". "
                          << "sizeof(TDigit) = " << sizeof(typename T::digit_type) << ", "
                          << "NDigits = " << T::num_digits << ", "
                          << "NDigitMax = " << static_cast<std::uint64_t>(T::digit_max) << std::endl;
                return false;
            }
        }
    }
    return true;
}

template<typename TDigit, unsigned NDigits, typename F>
bool test_op_digits(int min_i, int max_i, int min_j, int max_j, F&& func, std::string_view op) {
    static constexpr auto digit_max = std::numeric_limits<TDigit>::max();
    return test_op<number<TDigit, NDigits,              1>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits,              2>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits,              3>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits,              4>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits,              5>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits,              6>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits,              7>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits,              8>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits,              9>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits,             10>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits,             11>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits,             12>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits,             13>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits,             14>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits,             15>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits, digit_max - 15>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits, digit_max - 14>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits, digit_max - 13>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits, digit_max - 12>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits, digit_max - 11>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits, digit_max - 10>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits, digit_max -  9>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits, digit_max -  8>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits, digit_max -  7>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits, digit_max -  6>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits, digit_max -  5>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits, digit_max -  4>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits, digit_max -  3>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits, digit_max -  2>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits, digit_max -  1>>(min_i, max_i, min_j, max_j, func, op) &&
           test_op<number<TDigit, NDigits, digit_max -  0>>(min_i, max_i, min_j, max_j, func, op);
}

template<unsigned NDigits, typename F>
bool test_op_sizes(int min_i, int max_i, int min_j, int max_j, F&& func, std::string_view op) {
    return test_op_digits<std::uint8_t,  NDigits>(min_i, max_i, min_j, max_j, func, op) &&
           test_op_digits<std::uint16_t, NDigits>(min_i, max_i, min_j, max_j, func, op);
           test_op_digits<std::uint32_t, NDigits>(min_i, max_i, min_j, max_j, func, op);
}

int main() {
    bool passed = test_op_sizes<32>(0, 1000,  0, 1000, std::plus<void>{},       "+") &&
                  test_op_sizes<32>(0, 1000, -1, 1000, std::minus<void>{},      "-") &&
                  test_op_sizes<32>(0, 1000,  0, 1000, std::multiplies<void>{}, "*") &&
                  test_op_sizes<32>(0, 1000,  1, 1000, std::divides<void>{},    "/") &&
                  test_op_sizes<32>(0, 1000,  1, 1000, std::modulus<void>{},    "%");
    return passed ? 0 : 1;
}
