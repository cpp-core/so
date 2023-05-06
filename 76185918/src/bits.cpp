// Copyright (C) 2022, 2023 by Mark Melton
//

#include <bitset>
#include <iostream>
#include <random>

using BitSet = std::bitset<20>;

BitSet generate_periodic(size_t period, size_t offset) {
    BitSet bits;
    for (auto i = offset; i < bits.size(); i += period)
	bits[i] = true;
    return bits;
}

int main(int argc, const char *argv[]) {
    BitSet input;
    std::uniform_int_distribution<uint64_t> d;
    std::mt19937 rng;
    for (auto i = 0; i < input.size(); ++i)
	input[i] = d(rng) % 2;

    std::cout << "input : " << input << std::endl;

    std::vector<BitSet> series;
    
    for (auto i = 1; i < input.size(); ++i) {
	std::optional<BitSet> best_bits;
	size_t max{};
	for (auto j = 0; j < i; ++j) {
	    auto bits = generate_periodic(i, j);
	    auto common = (input bitand bits).count();
	    auto excess = (~input bitand bits).count();
	    if (excess == 0 and common > max) {
		best_bits = bits;
		max = common;
	    }
	}

	if (best_bits) {
	    input &= ~*best_bits;
	    series.push_back(*best_bits);
	}
    }

    BitSet check;
    for (const auto& bits : series) {
	std::cout << "series: " << bits << std::endl;
	check |= bits;
    }
    std::cout << "check : " << check <<  std::endl;
    return 0;
}
