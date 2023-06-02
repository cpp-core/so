// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>
#include <random>

using std::cout, std::endl;

int main(int argc, const char *argv[]) {
    // We have 100 records each with 50 bytes.
    int nrecords = 100, nbytes = 50;
    std::vector<uint8_t> data(nrecords * nbytes);

    // Generate random data for testing.
    std::uniform_int_distribution<uint8_t> d;
    std::mt19937_64 rng;
    std::generate(data.begin(), data.end(), [&]() { return d(rng); });

    int key_index = 20;
    auto cmp = [&](uint8_t *a, uint8_t *b) {
	int aval = *(int*)(a + key_index), bval = *(int*)(b + key_index);
	return aval < bval;
    };

    // How can I call std::sort with run-time record length?
    // std::sort(data.begin(), ...)
    return 0;
}
