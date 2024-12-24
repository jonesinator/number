#ifndef INTEGER_HPP_
#define INTEGER_HPP_

#include <sstream>

#include "number.hpp"

template <typename TNumber>
class integer {
public:
    using number_type = TNumber;

    constexpr integer() = default;
    constexpr integer(const integer& other) noexcept = default;
    constexpr integer(integer&& other) noexcept = default;
    constexpr ~integer() noexcept = default;
    constexpr integer& operator=(const integer& other) noexcept = default;
    constexpr integer& operator=(integer&& other) noexcept = default;
    constexpr std::strong_ordering operator<=>(const integer&) const noexcept = default;

    template <typename T>
    requires std::integral<T>
    explicit constexpr integer(T value) noexcept {
        *this = value;
    }

    template <typename T>
    requires std::integral<T>
    explicit constexpr operator T() const noexcept {
        if (positive_) {
            return static_cast<T>(static_cast<std::make_unsigned_t<T>>(magnitude_));
        } else {
            return static_cast<T>(static_cast<std::make_unsigned_t<T>>(magnitude_)) * -1;
        }
    }

    template <typename T>
    requires std::integral<T>
    constexpr integer& operator=(T value) noexcept {
        positive_ = value >= 0;
	if (positive_) {
            magnitude_ = number_type(static_cast<std::make_unsigned_t<T>>(value));
	} else {
            magnitude_ = number_type(static_cast<std::make_unsigned_t<T>>(value * -1));
	}
        return *this;
    }

    void show() const {
        std::cout << "(" << positive_ << ", ";
	for (int i = number_type::num_digits - 1; i >= 0; --i) {
            std::cout << (int)magnitude_.digit(i) << "|";
	}
	std::cout << ")" << std::endl;
    }

    constexpr integer operator+() const noexcept {
        return integer(true, magnitude_);
    }

    constexpr integer operator+(const integer& rhs) const noexcept {
        if (positive_ == rhs.positive_) {
            return integer(positive_, magnitude_ + rhs.magnitude_);
        } else if (magnitude_ < rhs.magnitude_) {
            return integer(rhs.positive_, rhs.magnitude_ - magnitude_);
        } else if (magnitude_ > rhs.magnitude_) {
            return integer(positive_, magnitude_ - rhs.magnitude_);
        } else {
            return integer();
        }
    }

    constexpr integer& operator+=(const integer& rhs) noexcept {
        *this = *this + rhs;
        return *this;
    }

    constexpr integer& operator++() noexcept {
        *this = *this + integer(1U);
        return *this;
    }

    constexpr integer operator++(int) noexcept {
        integer previous = *this;
        ++(*this);
        return previous;
    }

    constexpr integer operator-() const noexcept {
        if (magnitude_ != number_type(0U)) {
            return integer(!positive_, magnitude_);
        } else {
            return *this;
        }
    }

    constexpr integer operator-(const integer& rhs) const noexcept {
        return *this + (-rhs);
    }

    constexpr integer& operator-=(const integer& rhs) noexcept {
        *this = *this - rhs;
        return *this;
    }

    constexpr integer& operator--() noexcept {
        *this = *this - integer(1U);
        return *this;
    }

    constexpr integer operator--(int) noexcept {
        integer previous = *this;
        --(*this);
        return previous;
    }

    constexpr integer operator*(const integer& rhs) const noexcept {
        number_type result_magnitude = magnitude_ * rhs.magnitude_;
        if (result_magnitude == number_type(0U)) {
            return integer();
        } else {
            return integer(!(positive_ != rhs.positive_), result_magnitude);
        }
    }

    constexpr integer& operator*=(const integer& rhs) noexcept {
        *this = *this * rhs;
        return *this;
    }

    constexpr integer operator/(const integer& rhs) const noexcept {
        number_type result_magnitude = magnitude_ / rhs.magnitude_;
        if (result_magnitude == number_type(0U)) {
            return integer();
        } else {
            return integer(!(positive_ != rhs.positive_), result_magnitude);
        }
    }

    constexpr integer& operator/=(const integer& rhs) noexcept {
        *this = *this / rhs;
        return *this;
    }

    constexpr integer operator%(const integer& rhs) const noexcept {
        number_type result_magnitude = magnitude_ % rhs.magnitude_;
        bool positive = result_magnitude != number_type(0U) ? positive_ : true;
        return integer(positive, result_magnitude);
    }

    constexpr integer& operator%=(const integer& rhs) noexcept {
        *this = *this % rhs;
        return *this;
    }

    constexpr bool positive() const noexcept {
        return positive_;
    }

    constexpr bool negative() const noexcept {
        return !positive();
    }

    constexpr number_type magnitude() const noexcept {
        return magnitude_;
    }

private:
    constexpr integer(bool positive, number_type magnitude) noexcept: positive_(positive), magnitude_(magnitude) {
    }

    bool positive_ = true;
    number_type magnitude_;
};

template <typename TNumber>
std::ostream& operator<<(std::ostream& stream, const integer<TNumber>& num) {
    if (num.negative()) {
        stream << '-';
    } else if (stream.flags() & std::ios_base::showpos) {
        stream << '+';
    }
    return stream << num.magnitude();
}

template <typename TNumber>
std::istream& operator>>(std::istream& stream, integer<TNumber>& out) {
    std::string integer_string;
    stream >> integer_string;
    std::stringstream ss(integer_string);

    bool positive = true;
    if (char first = ss.peek(); first == '+' || first == '-') {
        ss.get();
        if (first == '-') {
            positive = false;
        }
    }

    typename integer<TNumber>::number_type magnitude;
    ss >> magnitude;

    out = integer<TNumber>(positive, magnitude);
    return stream;
}

#endif /* INTEGER_HPP_ */
