// Copyright (C) 2022, 2023 by Mark Melton
//

#include <fmt/format.h>
#include "core/util/tool.h"
#include "core/chrono/stopwatch.h"
#include "core/util/random.h"

uint64_t linear_congruent_generator(uint64_t n) {
    // constexpr uint64_t Multiplier = 0x9e3779b97f4a7c15ull;
    constexpr uint64_t Multiplier = 0xda942042e4dd58b5ull;
    constexpr uint64_t Constant = 1ull;
    return Multiplier * n + Constant;
}

auto feistel_shift_and_mask(size_t nb) {
    auto shift = (1 + nb) / 2;
    auto mask = (uint64_t{1} << shift) - 1;
    return std::make_tuple(shift, mask);
}

auto feistel_split_msg(uint64_t msg, int shift, uint64_t mask) {
    auto right = msg bitand mask;
    auto left = (msg >> shift) bitand mask;
    return std::make_tuple(left, right);
}

auto feistel_swap_msg(uint64_t msg, int shift, uint64_t mask) {
    auto [left, right] = feistel_split_msg(msg, shift, mask);
    return (right << shift) bitor left;
}

auto feistel_round(uint64_t msg, uint64_t round, int shift, uint64_t mask) {
    auto [left, right] = feistel_split_msg(msg, shift, mask);
    auto new_right = (left ^ linear_congruent_generator(right ^ round)) bitand mask;
    return (right << shift) bitor new_right;
}

constexpr uint64_t FeistelRounds[] = {
    0x88ef7267b3f978daull,
    0x5457c7476ab3e57full,
    0x89529ec3c1eec593ull,
    0x3fac1e6e30cad1b6ull
};

auto feistel_cipher(uint64_t msg, size_t nb, size_t nr) {
    auto [shift, mask] = feistel_shift_and_mask(nb);
    for (auto i = 0; i < nr; ++i)
	msg = feistel_round(msg, FeistelRounds[i], shift, mask);
    msg = feistel_swap_msg(msg, shift, mask);
    return msg;
}

auto feistel_decipher(uint64_t msg, size_t nb, size_t nr) {
    auto [shift, mask] = feistel_shift_and_mask(nb);
    for (auto i = 0; i < nr; ++i)
	msg = feistel_round(msg, FeistelRounds[nr - i - 1], shift, mask);
    msg = feistel_swap_msg(msg, shift, mask);
    return msg;
}

template<class Work>
void measure(std::ostream& os, std::string_view desc, Work&& work) {
    chron::StopWatch timer;
    timer.mark();
    work();
    auto millis = timer.elapsed_duration<std::chrono::milliseconds>().count();
    os << fmt::format("{:>12s}: {:5d} ms", desc, millis) << endl;
}

int tool_main(int argc, const char *argv[]) {
    ArgParse opts
	(
	 argValue<'b'>("bits", 4, "Number of bits"),
	 argValue<'r'>("rounds", 3, "Number of Feistel rounds"),
	 argFlag<'v'>("verbose", "Verbose diagnostics")
	 );
    opts.parse(argc, argv);
    auto b = opts.get<'b'>();
    auto r = opts.get<'r'>();
    // auto verbose = opts.get<'v'>();

    for (auto i = 0; i < (1ull << b); ++i) {
	auto code = feistel_cipher(i, b, r);
	auto decode = feistel_decipher(code, b, r);
	cout << i << " " << code << " " << decode << endl;
    }

    return 0;
}
