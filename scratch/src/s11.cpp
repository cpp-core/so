
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
using Prices = std::vector<int>;
using Costs = std::vector<int>;

auto cost_for_selected(const std::vector<int>& prices, BitMask selected) {
    int sum{};
    int idx{std::countr_zero(selected)};
    while (selected) {
	sum += prices[idx];
	selected >>= std::countr_zero(selected) + 1;
	idx += std::countr_zero(selected) + 1;
    }
    return sum;
}

struct Partition {
    Partition(const Prices& prices, int needed) {
	BitMask end_mask = BitMask{1} << prices.size();
	BitMask mask = (BitMask{1} << needed) - 1;
	while (mask < end_mask) {
	    costs.push_back(cost_for_selected(prices, mask));
	    mask = next_permutation_n(mask);
	}
	std::sort(costs.begin(), costs.end());
    }
    Costs costs;
};

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

    // Partition the available items into left and right halfs (roughly).
    auto nleft = nitems / 2, nright = nitems - nleft;

    // Partition the prices into left and right.
    Prices left_prices, right_prices;
    for (auto i = 0; i < nleft; ++i)
	left_prices.push_back(prices[i]);
    for (auto i = 0; i < nright; ++i)
	right_prices.push_back(prices[i + nleft]);

    // Compute all the partitions, from 0 to needed for both left and right.
    std::vector<Partition> left, right;
    for (auto i = 0; i <= needed; ++i) {
	left.emplace_back(left_prices, i);
	right.emplace_back(right_prices, i);
    }

    // Combine partitions and count results.
    size_t n{};
    for (auto i = 0; i <= needed; ++i) {
	auto& left_costs = left[i].costs;
	auto& right_costs = right[needed - i].costs;

	if (left_costs.size() > right_costs.size())
	    std::swap(left_costs, right_costs);
	
	for (int i = 0, j = right_costs.size() - 1; i < left_costs.size() and j >= 0; ++i) {
	    auto lc = left_costs[i];
	    while (j >= 0 and lc + right_costs[j] > cost)
		--j;
	    if (j >= 0)
		n += j + 1;
	}
    }
    
    cout << fmt::format("result  : {}", n) << endl;

    return 0;
}
