// Copyright (C) 2022, 2023 by Mark Melton
//

#include <algorithm>
#include <iostream>
#include <cmath>
#include <chrono>
#include <iostream>
#include <memory>
#include "nanobench.h"

using std::cin, std::cout, std::endl;

template<class T>
auto clamp(const T& v, const T& lo, const T& hi) {
    return v < lo ? lo : hi < v ? hi : v;
}

template<class Op>
void func(uint8_t *mem, Op&& op)
{
    for (int i = 0; i != 720; ++i) {
        for (int j = 0; j != 1280; ++j) {
            for (int k = 0; k != 3; ++k) {
                float tmp = i + j + k;
		*mem++ = op(tmp);
            }
        }
    }
}

int main() {
    std::unique_ptr<uint8_t[]> ptr(new uint8_t[1280 * 720 * 3]);
    auto *mem = ptr.get();

    ankerl::nanobench::Bench().run("compare", [&]() {
	func(mem, [](float x) {
	    return (x > 0 ? (x < 255 ? x : 255) : 0) + 0.5;
	});
    });
    
    ankerl::nanobench::Bench().run("round-fminmax", [&]() {
	func(mem, [](float x) {
	    return std::round(fmin(255.f, fmax(0.f, x)));
	});
    });
    
    ankerl::nanobench::Bench().run("clamp", [&]() {
	func(mem, [](float x) {
	    return clamp(x + 0.5f, 0.0f, 255.0f);
	});
    });
}
