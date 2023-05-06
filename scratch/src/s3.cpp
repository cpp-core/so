// Copyright (C) 2022, 2023 by Mark Melton
//

#include "core/util/tool.h"
#include "core/util/random.h"

/// Return the lexicographically next bit permutation with the same
/// number of bits set as `r`.
template<std::unsigned_integral T>
auto next_permutation_n(T n) {
    auto m = n bitor (n - 1);
    return (m + 1) bitor (((~m bitand -~m) - 1) >> (std::countr_zero(n) + 1));
}

/// Return the number of distinct subsets of size `k` given a set of
/// size `n` (informally, n choose k).
template<std::integral T>
auto n_choose_k(T n, T k) {
    if (k == 0) return T{1};
    if (k > n / 2) k = n - k;
    
    T r{1};
    for (auto i = 1; i <= k; ++i) {
	r *= n - i + 1;
	r /= i;
    }
    return r;
}

// Return the number of unique elements in in the selected partition
// of `data`, which must be in sorted order.
template<std::unsigned_integral T>
auto count_unique(const std::vector<unsigned int>& data, T selected) {
    T mask{1};
    size_t n{};
    std::optional<unsigned int> last;
    for (auto elem : data) {
	if (mask bitand selected) {
	    if (not last or (elem != *last))
		++n;
	    last = elem;
	}
	mask <<= 1;
    }
    return n;
}

// Output the selected elements in `data`
template<std::unsigned_integral T>
void output_partition(std::ostream& os, const std::vector<unsigned int>& data, T selected) {
    T mask{1};
    for (auto elem : data) {
	if (mask bitand selected)
	    os << elem << " ";
	mask <<= 1;
    }
}

// Return true if the two partitions select the same element values.
template<std::unsigned_integral T>
bool same(const std::vector<unsigned int>& data, T a, T b) {
    int idx{std::countr_zero(a)}, jdx{std::countr_zero(b)};
    while (a and b) {
	if (data[idx] != data[jdx])
	    return false;
	a >>= std::countr_zero(a) + 1;
	b >>= std::countr_zero(b) + 1;
	idx += std::countr_zero(a);
	jdx += std::countr_zero(b);
    }
    return true;
}

int tool_main(int argc, const char *argv[]) {

    // Start with sorted elements.
    std::vector<unsigned int> data = { 1, 2, 2, 3, 3, 3, 4, 4, 4, 4 };
    std::sort(data.begin(), data.end(), std::less{});

    // There must be an even number of elements for the left and right
    // subsets to be of equal size.
    auto n = data.size();
    if (n == 0 or n bitand 1)
	return 0;

    // Construct a bit-mask where the one bits mark the left subset of
    // elements and the zero bits mark the right subset of
    // elements. Start with the lower half of the bits set.
    auto partition = (1ull << (n / 2)) - 1;

    // One past the last partition to consider.
    auto end_partition = 1ull << n;

    // Iterate over all the partitionings.
    decltype(partition) last_partition{};
    while (partition < end_partition) {
	bool different = not last_partition or
	    not same(data, partition, last_partition) or
	    not same(data, ~partition, ~last_partition);
	if (different and (count_unique(data, partition) == count_unique(data, ~partition))) {
	    cout << "[ ";
	    output_partition(cout, data, partition);
	    cout << "] - [ ";
	    output_partition(cout, data, ~partition);
	    cout << "]";
	    cout << endl;
	}
	last_partition = partition;
	partition = next_permutation_n(partition);
    }
    
    return 0;
}
