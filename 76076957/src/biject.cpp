// Copyright (C) 2022, 2023 by Mark Melton
//

#include <fmt/format.h>
#include "core/util/tool.h"
#include "core/chrono/stopwatch.h"

uint64_t pseudo_random_function(uint64_t s0, uint64_t s1) {
    auto a = s0 + s1;
    a ^= a >> 12;
    a ^= a << 25;
    a ^= a >> 27;
    return a * 0x2545f4914f6cdd1dull;
}

class FeistelCipher {
public:
    FeistelCipher(int number_of_bits, int number_rounds)
	: shift_((1 + number_of_bits) / 2)
	, mask_((uint64_t{1} << shift_) - 1)
	, nrounds_(number_rounds) {
    }

    auto encode(uint64_t msg) const {
	auto [left, right] = split(msg);
	for (auto i = 0; i < nrounds_; ++i)
	    round(left, right, Rounds[i]);
	return combine(left, right);
    }

    auto decode(uint64_t msg) const {
	auto [left, right] = split(msg);
	for (int i = nrounds_ - 1; i >= 0; --i)
	    round(right, left, Rounds[i]);
	return combine(left, right);
    }

private:
    std::tuple<uint64_t, uint64_t> split(uint64_t msg) const {
	auto right = msg bitand mask_;
	auto left = (msg >> shift_) bitand mask_;
	return std::make_tuple(left, right);
    }

    uint64_t combine(uint64_t left, uint64_t right) const {
	return (left << shift_) bitor right;
    }

    void round(uint64_t& left, uint64_t& right, uint64_t constant) const {
	auto prf = pseudo_random_function(right, constant) bitand mask_;
	auto r = left ^ prf;
	left = right;
	right = r;
    }

    static constexpr uint64_t Rounds[] = {
	0x88ef7267b3f978daull,
	0x5457c7476ab3e57full,
	0x89529ec3c1eec593ull,
	0x3fac1e6e30cad1b6ull,
	0x56c644080098fc55ull,
	0x70f2b329323dbf62ull,
	0x08ee98c0d05e3dadull,
	0x3eb3d6236f23e7b7ull,
	0x47d2e1bf72264fa0ull,
	0x1fb274465e56ba20ull,
	0x077de40941c93774ull,
	0x857961a8a772650dull
    };
    
    int shift_;
    uint64_t mask_;
    int nrounds_;
};

class PseudoRandomPermutation {
public:
    PseudoRandomPermutation(uint64_t min, uint64_t max, int rounds = 3)
	: min_(min)
	, size_(max - min)
	, cipher_(64 - std::countl_zero(size_), rounds) { 
    }

    auto min() const {
	return min_;
    }

    auto max() const {
	return min_ + size_;
    }

    auto size() const {
	return size_;
    }
    
    uint64_t encode(uint64_t msg) const {
	msg -= min_;
	do {
	    msg = cipher_.encode(msg);
	} while (msg >= size_);
	msg += min_;
	return msg;
    }
    
private:
    uint64_t min_, size_;
    FeistelCipher cipher_;
};

template<class Work>
void measure(std::ostream& os, std::string_view desc, Work&& work) {
    chron::StopWatch timer;
    timer.mark();
    if (work())
	os << fmt::format("{:>12s}: work failed", desc) << endl;
    auto millis = timer.elapsed_duration<std::chrono::milliseconds>().count();
    os << fmt::format("{:>12s}: {:5d} ms", desc, millis) << endl;
}

int tool_main(int argc, const char *argv[]) {
    ArgParse opts
	(
	 argValue<'m'>("min", 0, "Minimum index"),
	 argValue<'x'>("max", 10, "Maximum index"),
	 argValue<'r'>("rounds", 3, "Number of rounds"),
	 argFlag<'v'>("verbose", "Verbose diagnostics")
	 );
    opts.parse(argc, argv);
    auto min = opts.get<'m'>();
    auto max = opts.get<'x'>();
    auto r = opts.get<'r'>();
    // auto verbose = opts.get<'v'>();
    uint64_t size = max - min;

    // {
    // 	auto size = 1ull << max;
    // 	FeistelCipher cip(max, r);
    // 	std::set<uint64_t> codes;
    // 	uint64_t msg = 0;
    // 	for (auto i = 0; i < size; ++i) {
    // 	    cout << msg << endl;
    // 	    msg = cip.encode(msg);
    // 	}
    // 	return 0;
    // }
    
    // {
    // 	auto size = 1ull << max;
    // 	FeistelCipher cip(max, r);
    // 	std::set<uint64_t> codes;
    // 	for (auto i = 0; i < size; ++i) {
    // 	    auto code = cip.encode(i);
    // 	    assert(code < size);
    // 	    codes.insert(code);
    // 	    cout << i << " " << code << " " << cip.decode(code) << endl;
    // 	}
    // 	assert(codes.size() == size);
    // 	return 0;
    // }

    FeistelCipher cipher(64 - std::countl_zero(size), r);
    PseudoRandomPermutation perm(min, max, r);
    std::set<uint64_t> fcodes, pcodes;
    for (auto i = min; i < max; ++i) {
	auto fcode = cipher.encode(i - min);
	auto pcode = perm.encode(i);
	assert(pcode >= min and pcode < max);
	fcodes.insert(fcode);
	pcodes.insert(pcode);
	cout << i << " " << fcode << " " << pcode << endl;
    }

    if (fcodes.size() != (max - min))
     	throw std::runtime_error("Feistel not a valid permutation");
    if (pcodes.size() != (max - min))
	throw std::runtime_error("Perm not a valid permutation");

    // measure(cout, "FeistelCipher", [&]() {
    // 	for (auto i = 0; i < size; ++i) {
    // 	    auto c = cipher.encode(i);
    // 	    if (c > 4 * size)
    // 		return true;
    // 	}
    // 	return false;
    // });

    // std::vector<uint64_t> data;
    // for (auto i = min; i < max; ++i)
    // 	data.push_back(i);
    // std::shuffle(data.begin(), data.end(), core::rng());
    // std::shuffle(data.begin(), data.end(), core::rng());
    // std::shuffle(data.begin(), data.end(), core::rng());
    // std::shuffle(data.begin(), data.end(), core::rng());
    // std::shuffle(data.begin(), data.end(), core::rng());
    // std::shuffle(data.begin(), data.end(), core::rng());
    // for (auto elem : data)
    // 	cout << elem << endl;

    return 0;
}
