// Copyright (C) 2022, 2023 by Mark Melton
//

#include "core/util/tool.h"
#include "core/chrono/stopwatch.h"

auto sieve_mod6_prime_seq(int max = int{1} << 20) {
    std::vector<int> primes;
    primes.push_back(2);
    primes.push_back(3);

    auto max_index = max / 3;
    auto bits_per = sizeof(uint64_t) * CHAR_BIT;
    auto nwords = (bits_per + max_index - 1) / bits_per;
    std::vector<uint64_t> words(nwords);

    words[0] |= 1;
    size_t wdx = 0;
    while (wdx < nwords) {
	auto b = std::countr_one(words[wdx]);
	auto p = 3 * (64 * wdx + b) + 1 + (b bitand 1);
	if (b < 64 and p < max) {
	    primes.push_back(p);
	    
	    for (auto j = p; j < max; j += 6 * p) {
		auto idx = j / 3;
		auto jdx = idx / 64;
		auto jmask = uint64_t{1} << (idx % 64);
		words[jdx] |= jmask;
	    }

	    for (auto j = 5 * p; j < max; j += 6 * p) {
		auto idx = j / 3;
		auto jdx = idx / 64;
		auto jmask = uint64_t{1} << (idx % 64);
		words[jdx] |= jmask;
	    }
	}
	else {
	    ++wdx;
	}
    }
    return primes;
}

auto sieve_mod2_prime_seq(int max = int{1} << 20) {
    std::vector<int> primes;
    
    auto bits_per = 2 * sizeof(uint64_t) * CHAR_BIT;
    auto nwords = (bits_per + max - 1) / bits_per;
    std::vector<uint64_t> words(nwords);
    if (max > 2)
	primes.push_back(2);
    words[0] |= 1;
    size_t idx = 0;
    while (idx < nwords) {
	auto bdx = 2 * std::countr_one(words[idx]) + 1;
	auto p = 128 * idx + bdx;
	if (bdx < 128 and p < max) {
	    primes.push_back(p);
	    for (auto j = p; j < max; j += 2 * p) {
		auto jdx = j / 128;
		auto jmask = uint64_t{1} << ((j % 128) / 2);
		words[jdx] |= jmask;
	    }
	}
	else {
	    ++idx;
	}
    }
    return primes;
}

auto sieve_index(int max = int{1} << 20) {
    std::vector<int> primes;
    std::vector<int> primesSum;
    int tally = 1;
    bool primeFound = false;
    while (tally < max) {
        int i = 0;
        while (!primeFound) {
            primeFound = true;
            tally++;
            for (i = 0; i < (int)primes.size(); i++) {
                if (tally == primesSum[i]) {
                    primeFound = false;
                    primesSum[i] += primes[i];
                }
            }
        }
        primeFound = false;
        primesSum.push_back(tally*2);
        primes.push_back(tally);
    }
    return primes;
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
	 argValue<'n'>("number", 100000, "Number of primes"),
	 argFlag<'v'>("verbose", "Verbose diagnostics")
	 );
    opts.parse(argc, argv);
    auto n = opts.get<'n'>();
    // auto verbose = opts.get<'v'>();

    measure(cout, "sieve_index", [&]() { sieve_index(n); });
    measure(cout, "sieve_2n", [&]() { sieve_mod2_prime_seq(n); });
    measure(cout, "sieve_6n", [&]() { sieve_mod6_prime_seq(n); });
    return 0;
}
