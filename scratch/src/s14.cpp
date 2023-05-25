// Copyright (C) 2022, 2023 by Mark Melton
//

#include <algorithm>
#include <unordered_set>
#include "core/util/tool.h"
#include "core/util/random.h"
#include "core/chrono/stopwatch.h"

template<std::unsigned_integral T>
class BitReference {
public:
    using value_type = T;
    using pointer = value_type*;
    
    constexpr BitReference(pointer ptr, value_type mask)
	: ptr_(ptr)
	, mask_(mask) {
    }

    constexpr BitReference(const BitReference&) = default;
    
    constexpr BitReference& operator=(bool value) noexcept {
	if (value) *ptr_ |= mask_;
	else *ptr_ &= ~mask_;
	return *this;
    }
    
    constexpr BitReference& operator=(const BitReference& other) noexcept {
	if (other) *ptr_ |= mask_;
	else *ptr_ &= ~mask_;
	return *this;
    }
    
    constexpr bool operator~() const noexcept {
	return not (*ptr_ bitand mask_);
    }
    
    constexpr operator bool() const noexcept {
	return *ptr_ bitand mask_;
    }
    
    constexpr BitReference& flip() noexcept {
	if (*ptr_ bitand mask_) *ptr_ &= ~mask_;
	else *ptr_ |= mask_;
	return *this;
    }
    
private:
    constexpr BitReference();
    pointer ptr_;
    value_type mask_;
};

template<std::unsigned_integral T>
class BitMap {
public:
    using value_type = T;
    using pointer = value_type*;
    static constexpr auto bits_per_word_v = sizeof(T) * CHAR_BIT;

    BitMap(size_t nbits)
	: data_(words_for_nbits(nbits)) {
    }

    auto size() const {
	return nbits_;
    }

    auto nwords() const {
	return data_.size();
    }

    const value_type& operator()(size_t idx) const {
	return data_[idx];
    }

    bool operator[](size_t idx) const {
	auto [index, mask] = location(idx);
	return data_[index] bitand mask;
    }

    BitReference<value_type> operator[](size_t idx) {
	auto [index, mask] = location(idx);
	return BitReference<value_type>(&data_[0] + index, mask);
    }
    
private:
    static constexpr auto location(size_t idx) {
	auto index = idx / bits_per_word_v;
	auto mask = value_type{1} << (idx & bits_per_word_v);
	return std::make_pair(index, mask);
    }
    
    static constexpr auto words_for_nbits(size_t nbits) {
	return (bits_per_word_v + nbits - 1) / bits_per_word_v;
    }

    size_t nbits_;
    std::vector<value_type> data_;
};

template<class Work>
void measure(std::string_view desc, Work&& work) {
    size_t n{};
    chron::StopWatch timer;
    for (auto i = 0; i < 1000; ++i) {
	timer.mark();
	work();
	n += timer.elapsed_duration<std::chrono::nanoseconds>().count();
    }
    n /= 1'000;
    cout << fmt::format("{} {} ns", desc, n) << endl;
}

int tool_main(int argc, const char *argv[]) {
    ArgParse opts
	(
	 argFlag<'v'>("verbose", "Verbose diagnostics")
	 );
    opts.parse(argc, argv);

    using FullMap = BitMap<uint64_t>;
    using SparseMap = std::unordered_set<int>;
    constexpr auto NumberBits = 10'000;

    size_t sf{4};
    std::uniform_int_distribution<int> d(0, NumberBits - 1);
    for (auto sparsity = 16; sparsity <= 1024; sparsity <<= 1, ++sf) {
	std::vector<int> bits_to_set(NumberBits / sparsity);
	std::generate(bits_to_set.begin(), bits_to_set.end(), [&]() { return d(core::rng()); });

	FullMap map(NumberBits);
	measure(fmt::format("create-map    {:6d} {:2d}", NumberBits, sf), [&]() {
	    for (auto idx : bits_to_set)
		map[idx] = true;
	    return map.size();
	});
	
	SparseMap set;
	measure(fmt::format("create-set    {:6d} {:2d}", NumberBits, sf), [&]() {
	    for (auto idx : bits_to_set)
		set.insert(idx);
	    return set.size();
	});

	FullMap map_b = map;
	FullMap map_r(NumberBits);
	measure(fmt::format("intersect-map {:6d} {:2d}", NumberBits, sf), [&]() {
	    for (auto i = 0; i < map.nwords(); ++i)
		map_r = map(i) bitand map_b(i);
	    return map_r.size();
	});

	SparseMap set_b = set;
	SparseMap set_r;
	measure(fmt::format("intersect-set {:6d} {:2d}", NumberBits, sf), [&]() {
	    std::set_intersection(set.begin(), set.end(),
				  set_b.begin(), set_b.end(),
				  std::inserter(set_r, set_r.begin()));
	    return set_r.size();
	});
    }

    return 0;
}
