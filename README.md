# number

A fixed-precision large unsigned integer type implemented as a single header library in c++23. No exceptions or dynamic memory allocations are used.

This library was designed with clarity and brevity in mind much more than performance. According to [cloc](https://github.com/AlDanial/cloc) the library currently weighs in around `308` lines, not including blanks and comments, so this is a rather small implementation, but it gets the job done.

Do not use in production, for educational purposes only.

## Theory

The number is simply stored as an array of native unsigned integers, from most to least significant.

These algorithms were implemented fairly closely from:

Knuth, Donald E. *The Art of Computer Programming, Volume 2: Seminumerical Algorithms*, 3rd ed., Section 4.3.1. Addison-Wesley, 1997. ISBN: 978-0201896848.

## Interface

The library exposes class `number` with three template parameters:

- `TDigit` - The type used to store digits. Must be an unsigned integer type. Exposed on the class as `digit_type`.
- `NDigits` - The number of digits to store in the `number`. Must be greater than zero. Exposed on the class as `num_digits`.
- `NDigitMax` - The maximum value a digit can have. `1 <= NDigitMax <= std::numeric_limits<TDigit>::max()`. Defaults to `std::numeric_limits<TDigit>::max()`, i.e. the maximally efficient representation. Exposed on the class as `digit_max`.

The `number` can represent values `0 <= number < (NDigitMax+1)^TDigit`.

The following standard operations are implemented for `class number`:

- default construction (zero)
- construction from native unsigned integer
- construction from `number` with more digits
- copy construction
- move construction
- copy assignment
- move assignment
- three-way comparison
- assignment from native unsigned integer
- assignment from `number` with more digits
- conversion (lossy) to native unsigned integer
- `+`
- `+=`
- `++` (prefix)
- `++` (postfix)
- `-`
- `-=`
- `--` (prefix)
- `--` (postfix)
- `*`
- `*=`
- `/`
- `/=`
- `%`
- `%=`
- `<<` (streaming)
- `>>` (streaming)

While bitwise operations could be implemented, they really only make sense when the base is a power of two. While they could be exposed only if that is the case, they have simply been omitted from this library for brevity.

Additionally, `number`s have the following custom arithmetic functions:

- `pow(number exponent)` - Raise a `number` to an exponent.

Additionally, `number`s have the following custom digit-related functions:

- `digit(unsigned power)` - Get the digit given its exponent, i.e. the digit representing base^power.
- `set_digit(unsigned power, digit_type value)` - Set the digit to a given value given its exponent.
- `most_significant_digit()` - Get the power of the highest digit currently set in the `number`.

Additionally, `number`s have the following custom serialization/deserialization functions:

- `static from_string(std::string_view input_string, string_base input_base)` - Convert a `std::string_view` to a `number`.
- `to_string(string_base sb)` - Convert the `number` to a `std::string`.

Where `string_base` is an `enum class` that can have the following values:

- `string_base::binary`
- `string_base::octal`
- `string_base::decimal`
- `string_base::hexadecimal`

When initializing numbers from literals, the literals _must_ be unsigned, that is to say the following will *not* compile:

```c++
number<std::uint32_t, 128> n(1);
```

But the following *will* work:

```c++
number<std::uint32_t, 128> n(1U);
```

This is to reduce the number of operations that might silently lose data.

## Usage

To use this library, first copy `number.hpp` into your project, and then `#include "number.hpp"` in one of your c++ files and use the `number<TDigit, NDigits, NDigitMax>` class much like the native unsigned integer types. You may wish to rename the class, enclose it in a namespace, etc. It is likely you'll want to use a type definition for a concrete number type, since dealing with the template paramters is unweildy, e.g. `using num = number<std::uint32_t, 128>;`.

## Testing

A simple unit test comes in this repository. Run the following command to run the test:

```
make && ./number_test; echo $?
```

It should print `0` if successful, and something else otherwise.
