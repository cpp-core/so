// Copyright (C) 2022, 2023 by Mark Melton
//

#include "core/util/tool.h"
#include "core/chrono/stopwatch.h"
#include "core/string/lexical_cast_stl.h"

// The following solution constructs a bijective function F that maps
// a range of integers onto itself. This function can be used to
// compute a pseudo-random index directly from an actual index such
// that the resulting pseudo-random indices are a permutation of the
// actual indices.
//
// There are three ideas (borrowed from cryptography) that allow the
// construction of such a function: 1) a pseudo-random-function (PRF)
// (https://en.wikipedia.org/wiki/Pseudorandom_function_family) that
// mixes the bits of the input and a round constant, 2) a Feistel
// network (https://en.wikipedia.org/wiki/Feistel_cipher) that uses
// the PRF to generate an encrypted version of the input that has at
// most a few more bits than the input, and 3) the idea of using a
// cycle-walk to construct an format-preserving-encryption FPE
// (https://en.wikipedia.org/wiki/Format-preserving_encryption) from a
// pseudo-random permutation with a slightly larger domain than the
// target domain.
//
// While these concepts draw on well-studied cryptography concepts, I
// believe the end product is probably unqiue and defintely insecure.

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
	 argValue<'m'>("range", std::make_pair(0, 16), "Permutation range min:max"),
	 argValue<'r'>("rounds", 3, "Number of rounds"),
	 argFlag<'p'>("performance", "Measure performance")
	 );
    opts.parse(argc, argv);
    auto [min, max] = opts.get<'m'>();
    auto rounds = opts.get<'r'>();
    auto measure_performance = opts.get<'p'>();

    if (measure_performance) {
	PseudoRandomPermutation perm(min, max, rounds);
	measure(cout, "Permutation", [&]() {
	    for (auto i = perm.min(); i < perm.max(); ++i) {
		auto code = perm.encode(i);
		if (code < perm.min() or code > perm.max())
		    return true;
	    }
	    return false;
	});
    } else {
	std::set<uint64_t> codes;
	PseudoRandomPermutation perm(min, max, rounds);
	for (auto i = min; i < max; ++i) {
	    auto code = perm.encode(i);
	    assert(code >= min and code < max);
	    codes.insert(code);
	    cout << i << " " << code << endl;
	}
	assert(codes.size() == max - min);
    }

    return 0;
}
