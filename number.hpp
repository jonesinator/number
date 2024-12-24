/**
 * Implements class number, a fixed-width unsigned integer with basic operations sufficient for many
 * cryptographic procedures. That said, use at your own risk. _That_ said, please do not use this
 * library for any production purpose, but please do use it for educational purposes.
 *
 * This header file is self-contained, depending only on the C++23 or later standard library. It
 * should be easy to add this library to any project simply by copying this file and including it.
 *
 * See class documentation for further details. See "number_test.cpp" for unit tests.
 */

#ifndef NUMBER_HPP_
#define NUMBER_HPP_

#include <algorithm>
#include <array>
#include <climits>
#include <cstdint>
#include <istream>
#include <limits>
#include <optional>
#include <ostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

/**
 * The base of a string representing a number.
 */
enum class string_base : unsigned { binary = 2, octal = 8, decimal = 10, hexadecimal = 16 };

/**
 * A fixed-width unsigned integer that does not require exceptions or dynamic memory allocation.
 *
 * @tparam TDigit    The type used to store digits. Must be an unsigned integer type.
 * @tparam NDigits   The number of digits that compose the number. Must be greater than zero.
 * @tparam NDigitMax The maximum value a single digit may hold. Must be greater than zero. This is
 *                   one less than the "base" of the number.
 *
 * Ideally, one would specify the base and the number of digits to define a number, but since we're
 * working with a computer, we must use one of the pre-defined unsigned integer widths to store our
 * digits, thus TDigit and NDigitMax are both required, rather than simply the base. We take
 * NDigitMax as a template parameter instead of a base, since it can always be expressed as a
 * TDigit, but the base may be outside of that range. For example, if TDigit is std::uint8_t then a
 * it can represent numbers 0 through 255 -- 256 possible values -- so the highest base compatible
 * with std::uint8_t is 256, but that cannot be represented using a TDigit itself, while an
 * NDigitMax of 255 can fit into a TDigit.
 *
 * The number is stored as an array of TDigits which is NDigits in length. The digit at index zero
 * is most significant, and the digit at index NDigits - 1 is least significant. This way, the
 * default three-way comparison operator works correctly to apply a strong ordering to the numbers.
 *
 *             Most Significant           Least Significant
 *             V                          V
 * digits := [ TDigit[0], TDigit[1], ..., TDigit[NDigits-1] ]
 *
 * Intermediate results must be stored in a type larger than the storage type, so a larger than
 * TDigit must be available. Practically that means that TDigit can be uint32_t at most. While
 * uint64_t _could_ be supported by some compilers by using the nonstandard unsigned __int128, it is
 * not implemented here.
 *
 * This class is optimized for simplicity and readability over performance. Numerous optimizations
 * could be made such as implementing special algorithms for when multiplying by a single digit, or
 * optimizing for cases where the base is a power of two, but none of these are done in order to
 * preserve the clarity of the code. While the code is very slow compared to sophisticated
 * implementations, it is still usable.
 *
 * No exceptions are thrown by this class. All operations succeed, even if there is loss of
 * information. No flags or other mechanisms exist to detect overflow or underflow of any
 * calculations. This class is no "safer" than standard unsigned integers, it's just larger.
 *
 * This is less flexible than an arbitrary-precision integer, but uses fixed memory and simpler
 * algorithms. No dynamic memory allocation is required except when converting the number to a
 * std::string.
 */
template <typename TDigit, unsigned NDigits, TDigit NDigitMax = std::numeric_limits<TDigit>::max()>
requires (std::unsigned_integral<TDigit> && NDigits > 0 && NDigitMax > 0)
class number {
public:
    /**
     * The storage type for digits.
     */
    using digit_type = TDigit;

    /**
     * The number of digits stored in the number.
     */
    static constexpr unsigned num_digits = NDigits;

    /**
     * The maximum value a single digit may hold.
     */
    static constexpr digit_type digit_max = NDigitMax;

    /**
     * Creates a fixed-width unsigned integer from a std::string_view. The entire std::string_view
     * must be consumed for success, otherwise std::nullopt is returned.
     *
     * @param input_string The string to convert to a number.
     * @param input_base The base of the input string.
     *
     * @returns A number wrapped in a std::optional if succesful, otherwise std::nullopt.
     */
    static constexpr std::optional<number> from_string(
        std::string_view input_string,
        string_base input_base = string_base::decimal
    ) noexcept {
        auto decode_char = [input_base](char c) -> std::optional<unsigned> {
            std::optional<unsigned> value;
            if (c >= '0' && c <= '9') {
                value = static_cast<unsigned>(c) - static_cast<unsigned>('0');
            } else if (c >= 'a' && c <= 'f') {
                value = 10 + (static_cast<unsigned>(c) - static_cast<unsigned>('a'));
            } else if (c >= 'A' && c <= 'F') {
                value = 10 + (static_cast<unsigned>(c) - static_cast<unsigned>('A'));
            }
            return (value && *value < std::to_underlying(input_base)) ? value : std::nullopt;
        };

        number result;
        number power(1U);
        number base_number(std::to_underlying(input_base));
        std::string_view::reverse_iterator character = input_string.rbegin();

        for (; character != input_string.rend(); character++, power *= base_number) {
            if(std::optional<unsigned> value = decode_char(*character)) {
                result += number(*value) * power;
            } else {
                return std::nullopt;
            }
        }

        return result;
    }

    constexpr number() noexcept {
        digits_.fill(0U);
    }

    template <typename T>
    requires std::unsigned_integral<T>
    explicit constexpr number(T value) noexcept {
        *this = value;
    }

    template <unsigned NDigits2>
    requires (NDigits2 < num_digits)
    constexpr number(const number<digit_type, NDigits2, digit_max>& value) noexcept {
        *this = value;
    }

    constexpr number(const number& other) noexcept = default;
    constexpr number(number&& other) noexcept = default;
    constexpr ~number() noexcept = default;
    constexpr number& operator=(const number& other) noexcept = default;
    constexpr number& operator=(number&& other) noexcept = default;
    constexpr std::strong_ordering operator<=>(const number&) const noexcept = default;

    template <typename T>
    requires std::unsigned_integral<T>
    explicit constexpr operator T() const noexcept {
        T value = 0;
        T power = 1;
        typename digits::const_reverse_iterator i = digits_.rbegin();

        for (; i != digits_.rend(); ++i) {
            big_digit_type power_previous = power;
            value += power * *i;
            power *= base;
            if (power < power_previous) {
                break;
            }
        }

        return value;
    }

    template <typename T>
    requires std::unsigned_integral<T>
    constexpr number& operator=(T value) noexcept {
        typename digits::reverse_iterator cursor = digits_.rbegin();
        for (; value != 0 && cursor != digits_.rend(); ++cursor, value /= base) {
            *cursor = value % base;
        }
        for (; cursor != digits_.rend(); ++cursor) {
            *cursor = 0;
        }
        return *this;
    }

    template <unsigned NDigits2>
    requires (NDigits2 < num_digits)
    constexpr number& operator=(const number<digit_type, NDigits2, digit_max>& value) noexcept {
        for (unsigned i = 0; i < num_digits; ++i) {
            set_digit(i, value.digit(i));
        }
        return *this;
    }

    constexpr number operator+(const number& rhs) const noexcept {
        number result;
        typename digits::const_reverse_iterator op1 = digits_.rbegin();
        typename digits::const_reverse_iterator op2 = rhs.digits_.rbegin();
        typename digits::reverse_iterator res = result.digits_.rbegin();
        unsigned carry = 0;

        for(; op1 != digits_.rend(); ++op1, ++op2, ++res) {
            big_digit_type digit_result = static_cast<big_digit_type>(*op1) + *op2 + carry;
            carry = digit_result >= base;
            *res = digit_result % base;
        }

        return result;
    }

    constexpr number& operator+=(const number& rhs) noexcept {
        *this = *this + rhs;
        return *this;
    }

    constexpr number& operator++() noexcept {
        *this = *this + number(1U);
        return *this;
    }

    constexpr number operator++(int) noexcept {
        number previous = *this;
        ++(*this);
        return previous;
    }

    constexpr number operator-(const number& rhs) const noexcept {
        number result;
        typename digits::const_reverse_iterator op1 = digits_.rbegin();
        typename digits::const_reverse_iterator op2 = rhs.digits_.rbegin();
        typename digits::reverse_iterator res = result.digits_.rbegin();
        unsigned borrow = 0;

        for(; op1 != digits_.rend(); ++op1, ++op2, ++res) {
            big_digit_type digit_result = (base + *op1) - (borrow + *op2);
            borrow = digit_result < base;
            *res = digit_result % base;
        }

        return result;
    }

    constexpr number& operator-=(const number& rhs) noexcept {
        *this = *this - rhs;
        return *this;
    }

    constexpr number& operator--() noexcept {
        *this = *this - 1U;
        return *this;
    }

    constexpr number operator--(int) noexcept {
        number previous = *this;
        --(*this);
        return previous;
    }

    constexpr number operator*(const number& rhs) const noexcept {
        number w;
        unsigned n = rhs.most_significant_digit();
        unsigned m = most_significant_digit();

        for (unsigned j = 0; j < n; ++j) {
            digit_type k = 0;

            for (unsigned i = 0; i < m; ++i) {
                big_digit_type t =
                    static_cast<big_digit_type>(digit(i)) * rhs.digit(j) + w.digit(i + j) + k;
                k = t / base;
                w.set_digit(i + j, t % base);
            }

            w.set_digit(j + m, k);
        }

        return w;
    }

    constexpr number& operator*=(const number& rhs) noexcept {
        *this = *this * rhs;
        return *this;
    }

    constexpr number operator/(const number& rhs) const noexcept {
        // Some intermediate results may require an additional digit.
        using big_number = number<digit_type, num_digits + 1, digit_max>;

        // Handle special cases.
        if (*this == number(0U) || rhs == number(0U) || rhs > *this) {
            return number(0U);
        } else if (rhs == *this) {
            return number(1U);
        } else if (rhs == number(1U)) {
            return *this;
        }
        // Given the above, if we get here, then 1 < rhs < *this.

        // Multiply the numerator and denominator by a normalization factor to ensure the leading
        // digit of the denominator is at least half the maximum digit value.
        big_number norm(base / (rhs.digit(rhs.most_significant_digit() - 1U) + 1U));
        big_number num = big_number(*this) * norm;
        big_number den = big_number(rhs) * norm;
        unsigned n = den.most_significant_digit();
        unsigned m = num.most_significant_digit() - n;

        // Loop through each possible quotient digit.
        number quotient;
        for (int j = m; j >= 0; --j) {
            // Guess the value of the quotient and remainder.
            big_digit_type qh =
                (static_cast<big_digit_type>(num.digit(j + n)) * base + num.digit(j + n - 1)) /
                den.digit(n - 1);
            big_digit_type rh =
                (static_cast<big_digit_type>(num.digit(j + n)) * base + num.digit(j + n - 1)) %
                den.digit(n - 1);

            // Correct the quotient and remainder if needed.
            while (qh >= base || (qh * den.digit(n - 2)) > (base * rh + num.digit(j + n - 2))) {
                --qh;
                rh += den.digit(n - 1);
                if (rh >= base) {
                    break;
                }
            }

            // Get the top n most significant digits from the numerator.
            big_number sig;
            for (int i = j + n; i >= j; i--) {
                sig.set_digit(i - j, num.digit(i));
            }

            // Try the guessed quotient as in long division.
            big_number trial = big_number(qh) * den;

            // If the trial is too high, reduce the guessed quotient and try again.
            // Use a while loop instead of a for loop, but this should never run more than once.
            while (sig < trial) {
                --qh;
                trial = big_number(qh) * den;
            }

            // We now have the new quotient digit. Set it in the output number, and update the
            // numerator.
            quotient.set_digit(j, qh);
            big_number diff = sig - trial;
            for (int i = j + n; i >= j; i--) {
                num.set_digit(i, diff.digit(i - j));
            }
        }

        return quotient;
    }

    constexpr number& operator/=(const number& rhs) noexcept {
        *this = *this / rhs;
        return *this;
    }

    constexpr number operator%(const number& rhs) const noexcept {
        return *this - ((*this / rhs) * rhs);
    }

    constexpr number& operator%=(const number& rhs) noexcept {
        *this = *this % rhs;
        return *this;
    }

    /**
     * Compute this number raised to a given exponent.
     *
     * @param exponent The exponent to raise this number to.
     *
     * @returns The result of this^exponent.
     */
    constexpr number pow(number exponent) const noexcept {
        number result(1U);
        number b = *this;

        while (exponent != number(0U)) {
            if (exponent.digit(0) & 1) {
                result *= b;
            }
            exponent /= number(2U);
            b *= b;
        }

        return result;
    }

    /**
     * Converts the number to a string in a given base.
     *
     * @param sb The base of the string to produce.
     *
     * @returns A std::string representing the base-encoded number. 
     */
    constexpr std::string to_string(string_base sb = string_base::decimal) const noexcept {
        std::string s;
        number n = *this;
        number b(std::to_underlying(sb));

        while(n != number(0U)) {
            unsigned string_digit = static_cast<unsigned>(n % b);
            if (string_digit <= 9) {
                s += '0' + static_cast<char>(string_digit);
            } else {
                s += 'a' + static_cast<char>(string_digit - 10);
            }
            n /= b;
        }

        std::ranges::reverse(s);
        return s;
    }

    /**
     * Gets the power of the highest non-zero digit in the number.
     *
     * @returns The power of the highest non-zero digit in the number.
     */
    constexpr unsigned most_significant_digit() const noexcept {
        unsigned n = num_digits;
        for (auto i = digits_.begin(); i != digits_.end() && *i == 0; ++i, --n);
        return n;
    }

    /**
     * Gets the value of a particular digit.
     *
     * @param power The power, i.e. base^power, of the digit to get. Essentially the reverse index.
     *
     * @returns The value of the digit. Powers beyond NDigits/num_digits will always yield zero.
     */
    constexpr digit_type digit(unsigned power) const noexcept {
        return power >= num_digits ? 0 : digits_[num_digits - 1 - power];
    }

    /**
     * Sets the value of a particular digit.
     *
     * @param power The power, i.e. base^power, of the digit to set. Essentially the reverse index.
     * @param value The value to set the digit to. May be truncated mod base.
     *
     * @note Setting the value of digits for powers beyond NDigits/num_digits will succeed, but will
     *       be a no-op.
     */
    constexpr void set_digit(unsigned power, digit_type value) noexcept {
        if (power < num_digits) {
            digits_[num_digits - 1 - power] = value % base;
        }
    }

private:
    /**
     * Type used to store the digits for this number.
     */
    using digits = std::array<digit_type, num_digits>;

    /**
     * Type used to store intermediate results of calculations between digits that might overflow.
     */
    using big_digit_type =
        std::conditional_t<sizeof(digit_type) <= sizeof(std::uint8_t),  std::uint16_t,
            std::conditional_t<sizeof(digit_type) <= sizeof(std::uint16_t), std::uint32_t,
                std::conditional_t<sizeof(digit_type) <= sizeof(std::uint32_t), std::uint64_t,
                    void>>>;

    /**
     * The number base used to store the digits this number.
     */
    static constexpr big_digit_type base = static_cast<big_digit_type>(digit_max) + 1;

    /**
     * Stores the digit information for the number. More significant digits have lower indices.
     */
    digits digits_;
};

/**
 * Output stream operator for numbers.
 *
 * @param stream Stream to output the number on.
 * @param num    Number to output on the stream.
 *
 * @returns A reference to the original output stream.
 */
template <typename TDigit, unsigned NDigits, TDigit NDigitMax>
std::ostream& operator<<(std::ostream& stream, const number<TDigit, NDigits, NDigitMax>& num) {
    switch (stream.flags() & std::ios_base::basefield) {
    case std::ios_base::oct: return stream << num.to_string(string_base::octal);
    case std::ios_base::dec: return stream << num.to_string(string_base::decimal);
    case std::ios_base::hex: return stream << num.to_string(string_base::hexadecimal);
    default: return stream << num.to_string(string_base::decimal);
    }
}

/**
 * Input stream operator for numbers.
 *
 * @param stream Stream to get the number from.
 * @param num    Reference to the number to get from the stream.
 *
 * @returns A reference to the original input stream.
 */
template <typename TDigit, unsigned NDigits, TDigit NDigitMax>
std::istream& operator>>(std::istream& stream, number<TDigit, NDigits, NDigitMax>& out) {
    using num = number<TDigit, NDigits, NDigitMax>;

    std::string number_string;
    stream >> number_string;

    switch (stream.flags() & std::ios_base::basefield) {
    case std::ios_base::oct: out = num::from_string(number_string, string_base::octal); break;
    case std::ios_base::dec: out = num::from_string(number_string, string_base::decimal); break;
    case std::ios_base::hex: out = num::from_string(number_string, string_base::hexadecimal); break;
    default:                 out = num::from_string(number_string, string_base::decimal); break;
    }

    return stream;
}

#endif /* NUMBER_HPP_ */
