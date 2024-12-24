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

#include "integer.hpp"

template<typename T, typename F>
bool test_op(int min_i, int max_i, int min_j, int max_j, F&& func, std::string_view op, bool exclude_zero) {
    for (int i = min_i; i <= max_i; ++i) {
        for (int j = min_j; j <= max_j; ++j) {
            if (exclude_zero && j == 0) {
                continue;
	    }
            auto actual = static_cast<int>(func(T(i), T(j)));
            auto expected = func(i, j);
            if (actual != expected) {
                std::cerr << "FAILED TEST CASE: " << i << " " << op << " " << j << ", "
                          << "expected " << expected << " but got " << actual << ". "
                          << "sizeof(TDigit) = " << sizeof(typename T::number_type::digit_type) << ", "
                          << "NDigits = " << T::number_type::num_digits << ", "
                          << "NDigitMax = " << static_cast<std::uint64_t>(T::number_type::digit_max) << std::endl;
                return false;
            }
        }
    }
    return true;
}

template<typename TDigit, unsigned NDigits, typename F>
bool test_op_digits(int min_i, int max_i, int min_j, int max_j, F&& func, std::string_view op, bool exclude_zero) {
    static constexpr auto digit_max = std::numeric_limits<TDigit>::max();
    return test_op<integer<number<TDigit, NDigits,              1>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits,              2>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits,              3>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits,              4>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits,              5>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits,              6>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits,              7>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits,              8>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits,              9>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits,             10>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits,             11>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits,             12>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits,             13>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits,             14>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits,             15>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits, digit_max - 15>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits, digit_max - 14>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits, digit_max - 13>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits, digit_max - 12>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits, digit_max - 11>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits, digit_max - 10>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits, digit_max -  9>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits, digit_max -  8>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits, digit_max -  7>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits, digit_max -  6>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits, digit_max -  5>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits, digit_max -  4>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits, digit_max -  3>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits, digit_max -  2>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits, digit_max -  1>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op<integer<number<TDigit, NDigits, digit_max -  0>>>(min_i, max_i, min_j, max_j, func, op, exclude_zero);
}

template<unsigned NDigits, typename F>
bool test_op_sizes(int min_i, int max_i, int min_j, int max_j, F&& func, std::string_view op, bool exclude_zero) {
    return test_op_digits<std::uint8_t,  NDigits>(min_i, max_i, min_j, max_j, func, op, exclude_zero) &&
           test_op_digits<std::uint16_t, NDigits>(min_i, max_i, min_j, max_j, func, op, exclude_zero);
           test_op_digits<std::uint32_t, NDigits>(min_i, max_i, min_j, max_j, func, op, exclude_zero);
}

int main() {
    bool passed = test_op_sizes<32>(-1000, 1000, -1000, 1000, std::plus<void>{},       "+", false) &&
                  test_op_sizes<32>(-1000, 1000, -1000, 1000, std::minus<void>{},      "-", false) &&
                  test_op_sizes<32>(-1000, 1000, -1000, 1000, std::multiplies<void>{}, "*", false) &&
                  test_op_sizes<32>(-1000, 1000, -1000, 1000, std::divides<void>{},    "/", true ) &&
                  test_op_sizes<32>(-1000, 1000, -1000, 1000, std::modulus<void>{},    "%", true );
    return passed ? 0 : 1;
}
