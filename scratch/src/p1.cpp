// Copyright (C) 2022, 2023 by Mark Melton
//

#include <cassert>
#include <iostream>

size_t alpha_string_to_number(const std::string& str) {
    size_t n{};
    for (auto elem : str) {
	n *= 26;
	n += 1 + (elem - 'a');
    }
    return n;
}

std::string number_to_alpha_string(size_t n) {
    std::string r{};
    while (n > 0) {
	r += 'a' + (--n % 26);
        n /= 26;
    }
    std::reverse(r.begin(), r.end());
    return r;
}

int main(int argc, const char *argv[]) {

    for (auto i = 0; i < 1'000'000; ++i) {
	auto s = number_to_alpha_string(i);
	auto n = alpha_string_to_number(s);
	assert(i == n);
    }
    // auto n1 = alpha_string_to_number("aaa");
    // // auto n2 = alpha_string_to_number("aaa");
    // std::cout << n1 << std::endl;
    // // std::cout << n2 << std::endl;
    // std::cout << number_to_alpha_string(n1) << std::endl;
    return 0;
}
