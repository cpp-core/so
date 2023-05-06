
Algorithm
---------

The following algorithm greedily addes sequence starting with the
smallest periods and growing to cover all of the bits in the input
sequence.

- Start with input sequence I of length N.
- Iterate period from 1 to N / 2
  - Iterate offset from 0 to period - 1
	- Generate a trial bitset T of the given period and offset
	- If T bitand ~I is not null, discard T and continue with the next offset
	- Record the bitset and the number of bits M in the bitset T bitand I.
  - If no bitsets were recorded, continue with the next period.
  - Add the bitset with the largest M to the output O.
  - Update the input with added bitset:  I = I bitand ~T.
- The bitor of the output bitsets O is equal to I.

Sample Code
-----------

You can find the code and build instruction on
[GitHub](https://github.com/cpp-core/so.git) in directory `76185918`.

```c++
#include <bitset>
#include <iostream>
#include <random>

using BitSet = std::bitset<20>;

BitSet generate_periodic(size_t period, size_t offset) {
    BitSet bits;
    for (auto i = offset; i < bits.size(); i += period)
        bits[i] = true;
    return bits;
}

int main(int argc, const char *argv[]) {
    BitSet input;
    std::uniform_int_distribution<uint64_t> d;
    std::mt19937 rng;
    for (auto i = 0; i < input.size(); ++i)
        input[i] = d(rng) % 2;

    std::cout << "input : " << input << std::endl;

    std::vector<BitSet> series;

    for (auto i = 1; i < input.size(); ++i) {
        std::optional<BitSet> best_bits;
        size_t max{};
        for (auto j = 0; j < i; ++j) {
            auto bits = generate_periodic(i, j);
            auto common = (input bitand bits).count();
            auto excess = (~input bitand bits).count();
            if (excess == 0 and common > max) {
                best_bits = bits;
                max = common;
            }
        }

        if (best_bits) {
            input &= ~*best_bits;
            series.push_back(*best_bits);
        }
    }

    BitSet check;
    for (const auto& bits : series) {
        std::cout << "series: " << bits << std::endl;
        check |= bits;
    }
    std::cout << "check : " << check <<  std::endl;
    return 0;
}
```
