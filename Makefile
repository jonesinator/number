test: integer_test
	./integer_test

integer_test: integer_test.cpp number.hpp integer.hpp
	g++ -std=c++23 -Wall -Wextra -Wpedantic -Werror -O3 integer_test.cpp -o integer_test

.PHONY: clean
clean:
	rm -f integer_test
