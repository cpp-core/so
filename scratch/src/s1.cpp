
#include "core/util/tool.h"

/// Return the lexicographically next bit permutation with the same
/// number of bits set as `r`.
template<std::unsigned_integral T>
auto next_permutation_n(T n) {
    auto m = n bitor (n - 1);
    return (m + 1) bitor (((~m bitand -~m) - 1) >> (std::countr_zero(n) + 1));
}

/// Return the number of distinct subsets of size `k` given a set of
/// size `n` (informally, n choose k).
template<std::integral T, std::integral U>
auto n_choose_k(T n, U k) {
    if (k == 0) return T{1};
    if (k > n / 2) k = n - k;
    
    T r{1};
    for (auto i = 1; i <= k; ++i) {
	r *= n - i + 1;
	r /= i;
    }
    return r;
}

using BitMask = uint32_t; // Since the upper limit of nitems was specified as 32.

auto total_cost(const std::vector<int>& prices, BitMask selected) {
    int sum{};
    int idx{std::countr_zero(selected)};
    while (selected) {
	sum += prices[idx];
	selected >>= std::countr_zero(selected) + 1;
	idx += std::countr_zero(selected) + 1;
    }
    return sum;
}

int tool_main(int argc, const char *argv[]) {
    ArgParse opts
	(
	 argValue<'m'>("needed", 10, "Items needed"),
	 argValue<'q'>("cost", 20, "Maximum cost"),
	 argValues<'*', std::vector, int>("prices", "Prices"),
	 argFlag<'v'>("verbose", "Verbose diagnostics")
	 );
    opts.parse(argc, argv);
    auto needed = opts.get<'m'>();
    auto cost = opts.get<'q'>();
    auto prices = opts.get<'*'>();
    auto nitems = prices.size();
    auto verbose = opts.get<'v'>();

    std::sort(prices.begin(), prices.end(), std::greater{});
    if (verbose) {
	cout << fmt::format("Items   : {}", nitems) << endl;
	cout << fmt::format("Needed  : {}", needed) << endl;
	cout << fmt::format("Cost    : {}", cost) << endl;
	cout << fmt::format("Prices  : ");
	for (auto price : prices)
	    cout << price << ",";
	cout << endl;
    }

    if (nitems < needed)
	throw core::runtime_error("Need {} items, but only {} available", needed, nitems);
    if (nitems > 32)
	throw core::runtime_error("Maximum of 32 items, but {} available", nitems);

    auto combinations = n_choose_k(nitems, needed);

    BitMask end_mask = BitMask{1} << nitems;
    BitMask mask = (BitMask{1} << needed) - 1;

    if (verbose) {
	cout << fmt::format("combs   : {}", combinations) << endl;
	cout << fmt::format("bit-mask: {:032b}", mask) << endl;
    }

    size_t n{};
    while (mask < end_mask) {
	if (total_cost(prices, mask) < cost)
	    ++n;
	mask = next_permutation_n(mask);
    }
    cout << fmt::format("result  : {}", n) << endl;

    return 0;
}
