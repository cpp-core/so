// Copyright (C) 2022, 2023 by Mark Melton
//

#include "core/util/tool.h"
#include "core/util/random.h"
#include "core/chrono/stopwatch.h"

template<class BidirectionalIterator, class Compare>
BidirectionalIterator partition_impl(BidirectionalIterator first, BidirectionalIterator last,
				     Compare cmp) {
    auto pivot = std::prev(last, 1);
    if (first == pivot) return first;
    while (first != last) {
        while (cmp(*first, *pivot)) { ++first; } // search greater > pivot
        do { --last; } while (last >= first and !cmp(*last, *pivot)); // search < pivot
        if (first > last) {     // if left iterator has surpassed right iterator
            std::iter_swap(first, pivot);
            return first;
        }
        if (*first > *last) {
            std::iter_swap(first, last);
            ++first;
        }
    }
    std::iter_swap(first, pivot);
    return first;
}

template <typename FwdIt, typename Comp = std::less<>>
void quick_sort(FwdIt first, FwdIt last, Comp cmp = Comp{}) {
    if (std::distance(first, last) <= 1) return;

    FwdIt split = partition_impl(first, last, cmp);

    quick_sort(first, split, cmp);  // sorts the left partition
    quick_sort(split + 1, last, cmp);   // sorts the right partition
}

auto generate_random_data(size_t n) {
    std::vector<uint64_t> data;
    std::uniform_int_distribution<uint64_t> d{};
    for (auto i = 0; i < n; ++i)
	data.push_back(d(core::rng()));
    return data;
}

template<class Work>
void measure_sort(std::string_view desc, size_t n, Work&& work) {
    auto data = generate_random_data(n);
    chron::StopWatch timer;
    work(data);
    for (auto i = 1; i < data.size(); ++i)
	if (not (data[i-1] <= data[i]))
	throw core::runtime_error("{} failed correctness check", desc);
    auto ms = timer.elapsed_duration<std::chrono::milliseconds>().count();
    cout << fmt::format("{} {} {} ms", desc, n, ms) << endl;
}

int tool_main(int argc, const char *argv[]) {
    std::vector<int> tmp = { 1, 2, 2 };
    for (auto i = 0; i < 1'000; ++i) {
	for (auto i = 0; i < tmp.size(); ++i)
	    cout << tmp[i] << " ";
	cout << endl;
	std::shuffle(tmp.begin(), tmp.end(), core::rng());
	quick_sort(tmp.begin(), tmp.end());
	for (auto i = 1; i < tmp.size(); ++i)
	    if (not (tmp[i-1] < tmp[i]))
		throw std::runtime_error("bad");
    }
    return 0;
    
    const size_t N = 20;
    for (auto i = 1; i <= N; ++i)
	measure_sort("std::sort", 1'000'000 * i, [](auto& data) {
	    std::sort(data.begin(), data.end(), [](const auto& a, const auto& b) {
		return a < b;
	    });
	});
    
    for (auto i = 1; i <= N; ++i)
	measure_sort("quick_sort", 1'000'000 * i, [](auto& data) {
	    quick_sort(data.begin(), data.end(), [](const auto& a, const auto& b) {
		return a < b;
	    });
	});
    
    return 0;
}
