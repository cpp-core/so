// Copyright (C) 2022, 2023 by Mark Melton
//

#include "core/util/tool.h"
#include "core/chrono/stopwatch.h"
#include "core/string/lexical_cast_stl.h"

uint64_t pseudo_random_function(uint64_t s0, uint64_t s1) {
    auto a = s0 + s1;
    a ^= a >> 12;
    a ^= a << 25;
    a ^= a >> 27;
    return a * 0x2545f4914f6cdd1dull;
}

template<class PRF>
class FeistelNetwork {
public:
    FeistelNetwork(int number_of_bits, int number_rounds, PRF&& prf)
	: shift_((1 + number_of_bits) / 2)
	, mask_((uint64_t{1} << shift_) - 1)
	, nrounds_(number_rounds)
	, prf_(std::forward<PRF>(prf)) {
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

    auto encode3(uint64_t msg) const {
	auto right = msg bitand mask_;
	auto left = (msg >> shift_) bitand mask_;
	round(left, right, Rounds[0]);
	round(left, right, Rounds[1]);
	round(left, right, Rounds[2]);
	return (left << shift_) bitor right;
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
	auto prf_value = prf_(right, constant) bitand mask_;
	auto r = left ^ prf_value;
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
    PRF prf_;
};

template<class PRF>
class PseudoRandomPermutation {
public:
    PseudoRandomPermutation(uint64_t min, uint64_t max, int rounds, PRF&& prf)
	: min_(min)
	, size_(1 + max - min)
	, cipher_(log2_ceil(size_), rounds, std::forward<PRF>(prf)) { 
    }

    auto min() const {
	return min_;
    }

    auto max() const {
	return min_ + size_ - 1;
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
    static size_t log2_ceil(size_t n) {
	if (n == 0)
	    return 0;
	else {
	    return 64 - std::countl_zero(n - 1);
	}
    }
    
    uint64_t min_, size_;
    FeistelNetwork<PRF> cipher_;
};

uint64_t iterate_prf(uint64_t n, size_t r) {
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
    
    for (auto i = 0; i < r; ++i)
	n = pseudo_random_function(n, Rounds[i]);
    return n;
}

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
	 argValue<'m'>("range", std::make_pair(0, 16), "Permutation range min:max"),
	 argValue<'r'>("rounds", 3, "Number of rounds"),
	 argFlag<'p'>("performance", "Measure performance"),
	 argFlag<'s'>("sort", "Sort index based on PRF")
	 );
    opts.parse(argc, argv);
    auto [min, max] = opts.get<'m'>();
    auto rounds = opts.get<'r'>();
    auto measure_performance = opts.get<'p'>();
    auto sort_index = opts.get<'s'>();

    if (measure_performance) {
	PseudoRandomPermutation perm(min, max, rounds, &pseudo_random_function);
	measure(cout, "Permutation", [&]() {
	    for (auto i = perm.min(); i < perm.max(); ++i) {
		auto code = perm.encode(i);
		if (code < perm.min() or code > perm.max())
		    return true;
	    }
	    return false;
	});
    } else if (sort_index) {
	std::vector<uint64_t> codes;
	for (auto i = min; i < max; ++i)
	    codes.push_back(i);
	std::sort(codes.begin(), codes.end(), [](uint64_t a, uint64_t b) {
	    return iterate_prf(a, 3) < iterate_prf(b, 3);
	});
	for (auto elem : codes)
	    cout << elem << endl;
    } else {
	std::set<uint64_t> codes;
	PseudoRandomPermutation perm(min, max, rounds, &pseudo_random_function);
	for (auto i = min; i < max; ++i) {
	    auto code = perm.encode(i);
	    assert(code >= min and code <= max);
	    codes.insert(code);
	    cout << i << " " << code << endl;
	}
	assert(codes.size() == max - min);
    }

    return 0;
}
