// Copyright (C) 2022, 2023 by Mark Melton
//

#include <algorithm>
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

using Element = uint;
using Elements = std::vector<Element>;
using PartitionMask = uint64_t;

// Return the number of unique elements in in the selected partition
// of `data`, which must be in sorted order.
auto count_unique(const Elements& data, PartitionMask mask) {
    std::optional<Element> last;
    int idx{std::countr_zero(mask)};
    size_t n{};
    while (mask) {
	auto elem = data[idx];
	if (not last or (elem != *last)) {
	    ++n;
	    last = elem;
	}
	mask >>= std::countr_zero(mask) + 1;
	idx += std::countr_zero(mask) + 1;
    }
    return n;
}

void add_partition(Elements& part, const Elements& data, PartitionMask mask) {
    int idx{std::countr_zero(mask)};
    while (mask) {
	part.push_back(data[idx]);
	mask >>= std::countr_zero(mask) + 1;
	idx += std::countr_zero(mask) + 1;
    }
}

// Return true if the two partitions select the same element values.
bool same(const Elements& data, PartitionMask a, PartitionMask b) {
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

    // These are the only bits that count.
    auto global_mask = end_partition - 1;

    std::vector<std::vector<uint>> results;
    
    // Iterate over all the partitionings.
    while (partition < end_partition) {
	if (count_unique(data, partition) == count_unique(data, ~partition bitand global_mask)) {
	    results.push_back({});
	    add_partition(results.back(), data, partition);
	    add_partition(results.back(), data, ~partition bitand global_mask);
	}
	partition = next_permutation_n(partition);
    }

    std::sort(results.begin(), results.end());
    auto last = std::unique(results.begin(), results.end());
    results.erase(last, results.end());

    for (const auto& r : results) {
	cout << "[ ";
	for (auto i = 0; i < r.size() / 2; ++i)
	    cout << r[i] << " ";
	cout << "] - [ ";
	for (auto i = r.size() / 2; i < r.size(); ++i)
	    cout << r[i] << " ";
	cout << "]" << endl;
    }
    
    return 0;
}
