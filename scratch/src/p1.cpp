// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>

size_t alpha_string_to_number(const std::string& str) {
    size_t n{}, place_digit{1};
    for (auto elem : str) {
	n *= place_digit;
	n += 1 + (elem - 'a');
	place_digit *= 26;
    }
    return n;
}

std::string number_to_alpha_string(size_t n) {
    std::string r{};
    while (n > 0) {
	r += 'a' + (n % 26) - 1;
        n /= 26;
    }
    std::reverse(r.begin(), r.end());
    return r;
}

int main(int argc, const char *argv[]) {

    auto n1 = alpha_string_to_number("z");
    auto n2 = alpha_string_to_number("ad");
    std::cout << n1 << std::endl;
    std::cout << n2 << std::endl;
    std::cout << number_to_alpha_string((n1 + n2) / 2) << std::endl;
    return 0;
}
