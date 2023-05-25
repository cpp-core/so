// Copyright (C) 2022, 2023 by Mark Melton
//

#undef NDEBUG
#include <cassert>

#include <algorithm>
#include <bit>
#include <iostream>
#include <random>
#include <set>

using std::cout, std::endl;
using num_t = uint32_t;

auto n_choose_k(size_t n, size_t k) {
    if (k == 0) return size_t{1};
    else return (n * n_choose_k(n - 1, k - 1)) / k;
}

auto bits_to_int(const std::vector<int>& bits) {
    num_t n{};
    for (auto i = 0; i < bits.size(); ++i)
	if (bits[i])
	    n |= (num_t{1} << i);
    return n;
}

auto min_distance(const std::vector<num_t>& codes) {
    size_t min_distance{32};
    for (auto n : codes) {
	for (auto m : codes) {
	    if (n == m)
		continue;
	    min_distance = std::min(min_distance, (size_t)std::popcount(n ^ m));
	}
    }
    return min_distance;
}

int main(int argc, const char *argv[]) {
    size_t count = argc > 1 ? atoi(argv[1]) : 16;
    size_t distance = argc > 2 ? atoi(argv[2]) : 17;

    size_t data_bits{1};
    while ((1ul << data_bits) < count)
	++data_bits;

    assert(data_bits == 4);

    size_t parity_needed = distance - 1;
    size_t parity_bits{n_choose_k(data_bits, data_bits - 1)};
    
    size_t nx = (parity_needed + parity_bits - 1) / parity_bits;
    size_t code_bits = data_bits + parity_bits;
    
    cout << "count         : " << count << endl;
    cout << "distance      : " << distance << endl;
    cout << "data_bits     : " << data_bits << endl;
    cout << "parity_bits   : " << parity_bits << endl;
    cout << "parity_needed : " << parity_needed << endl;
    cout << "nx            : " << nx << endl;
    cout << "code_bits     : " << code_bits << endl;
    
    if (nx * code_bits > 32) {
	cout << "too many bits needed" << endl;
	return -1;
    }

    std::vector<num_t> codes(count);
    std::generate(codes.begin(), codes.end(), [n=0]() mutable { return n++; });

    std::vector<num_t> parity_masks = {
	0b0111,
	0b1011,
	0b1101,
	0b1110
    };
    
    cout << min_distance(codes) << endl;
    size_t bdx = data_bits;
    for (const auto& parity_mask : parity_masks) {
	for (auto& code : codes) {
	    auto parity = std::popcount(code bitand parity_mask) bitand 1u;
	    if (parity == 0)
		code |= (1ul << bdx);
	}
	++bdx;
	cout << min_distance(codes) << endl;
    }
    
    // std::vector<int> bits = { 0, 1, 1, 1 };
    // size_t bdx = data_bits;
    // cout << min_distance(codes) << endl;
    // do {
    // 	for (auto& code : codes) {
    // 	    auto parity = std::popcount(code bitand bits_to_int(bits)) bitand 1u;
    // 	    if (parity == 0)
    // 		code |= (1ul << bdx);
    // 	}
    // 	++bdx;
    // 	cout << min_distance(codes) << endl;
    // } while (std::next_permutation(bits.begin(), bits.end()));

    uint32_t code_mask = (uint32_t{1} << code_bits) - 1;
    for (auto& code : codes) {
	auto n = (code bitand code_mask) << code_bits;
	for (auto i = 1; i < nx; ++i) {
	    code |= n;
	    n <<= code_bits;
	}
    }

    cout << "code_bits    : " << (nx * code_bits) << endl;
    cout << "min_distance : " << min_distance(codes) << endl;

    return 0;
}
