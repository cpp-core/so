// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>
#include <vector>

using num_t = uint32_t;
using std::cout, std::endl;

auto min_distance(const std::vector<num_t>& codes) {
    size_t distance{32};
    for (auto n : codes) {
	for (auto m : codes) {
	    if (n == m)
		continue;
	    distance = std::min(distance, (size_t)std::popcount(n ^ m));
	}
    }
    return distance;
}

auto n_choose_k(size_t n, size_t k) {
    if (k == 0) return size_t{1};
    else return (n * n_choose_k(n - 1, k - 1)) / k;
}

int main(int argc, char *argv[]) {
    size_t n{};
    for (auto k = 1; k < 17; ++k)
	n += n_choose_k(32, k);
    cout << n << endl;
    return 0;
    
    std::vector<num_t> codes = {
	0b00011, 0b00101, 0b00110, 0b01001, 0b01010,
	0b01100, 0b10001, 0b10010, 0b10100, 0b11000
    };

    cout << min_distance(codes) << endl;
    return 0;
    
    std::vector<num_t> parity_masks = {
	0b11111, 0b11110, 0b11101, 0b11011, 0b10111, 0b11110
    };

    size_t bdx = 4;
    for (auto parity_mask : parity_masks) {
	for (auto& code : codes) {
	    auto parity = std::popcount(code bitand parity_mask);
	    if (parity % 2 == 0)
		code |= (1ul << bdx);
	}
	++bdx;
	cout << min_distance(codes) << endl;
    }
    
    return 0;
}
