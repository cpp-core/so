
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

int tool_main(int argc, const char *argv[]) {
    ArgParse opts
	(
	 argValue<'n'>("choices", 10, "Number choices"),
	 argFlag<'v'>("verbose", "Verbose diagnostics")
	 );
    opts.parse(argc, argv);
    auto choices = opts.get<'n'>();
    auto verbose = opts.get<'v'>();

    return 0;
}
