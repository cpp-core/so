// Copyright (C) 2022, 2023 by Mark Melton
//

#undef NDEBUG
#include <cassert>

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

using std::cout, std::endl;

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& data) {
    os << "[ ";
    for (auto elem : data)
	os << elem << " ";
    os << "]";
    return os;
}

int *partition(int *first, int *last, int value) {
    while (true) {
	while (*first < value)
	    ++first;
	while (value < *last)
	    --last;
	if (first >= last)
	    return last;
	std::swap(*first, *last);
	++first;
	--last;
    }
}

void check_partition(int *first, int *split, int *last, int value) {
    while (first <= split) {
        assert(*first <= value);
        ++first;
    }

    while (first <= last) {
        assert(*first >= value);
        ++first;
    }
}

void qsort(int *first, int *last) {
    if (first < last) {
	auto pivot = *(first + (last - first) / 2);
	auto split = partition(first, last, pivot);
	check_partition(first, split, last, pivot);
	qsort(first, split);
	qsort(split + 1, last);
    }
}

void check_sort(int *first, int *last) {
    while (first < last) {
	assert(*first <= *(first + 1));
	++first;
    }
}

int main(int argc, const char *argv[]) {
    std::vector<int> data(30);
    std::generate(data.begin(), data.end(), [n=0]() mutable { return n++; });
    std::shuffle(data.begin(), data.end(), std::mt19937_64{});
    cout << data <<  endl;

    auto first = &data[0];
    auto last = first + data.size() - 1;
    auto pivot = data.size() / 2;
    auto split = partition(first, last, pivot);
    check_partition(first, split, last, pivot);
    cout << data <<  endl;

    std::shuffle(data.begin(), data.end(), std::mt19937_64{});
    qsort(first, last);
    check_sort(first, last);
    cout << data <<  endl;
    
    return 0;
}
