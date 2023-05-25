// Copyright (C) 2022, 2023 by Mark Melton
//

#undef NDEBUG
#include "core/util/tool.h"

// If we are using gcc or clang, using the compiler builtin.
#if defined(__GNUC__) || defined(__clang__)

int countr_one(unsigned int n) {
    return ~n == 0 ? (sizeof(unsigned int) * CHAR_BIT) : __builtin_ctz(~n);
}

int countr_one(unsigned long int n) {
    return ~n == 0 ? (sizeof(unsigned long int) * CHAR_BIT) : __builtin_ctzl(~n);
}

int countr_one(unsigned long long int n) {
    return ~n == 0 ? (sizeof(unsigned long long int) * CHAR_BIT) : __builtin_ctzll(~n);
}

// Otherwise, a standards compliant implementation
#else

int countr_one(uint32_t n) {
    n = ~n & (n+1);   // this gives a 1 to the left of the trailing 1's
    n--;              // this gets us just the trailing 1's that need counting
    n = (n & 0x55555555) + ((n>>1) & 0x55555555);  // 2 bit sums of 1 bit numbers
    n = (n & 0x33333333) + ((n>>2) & 0x33333333);  // 4 bit sums of 2 bit numbers
    n = (n & 0x0f0f0f0f) + ((n>>4) & 0x0f0f0f0f);  // 8 bit sums of 4 bit numbers
    n = (n & 0x00ff00ff) + ((n>>8) & 0x00ff00ff);  // 16 bit sums of 8 bit numbers
    n = (n & 0x0000ffff) + ((n>>16) & 0x0000ffff); // sum of 16 bit numbers
    return n;
}

int countr_one(uint64_t n) {
    n = ~n & (n+1);
    n--;           
    n = (n & 0x5555555555555555ul) + ((n>>1) & 0x5555555555555555ul);
    n = (n & 0x3333333333333333ul) + ((n>>2) & 0x3333333333333333ul);
    n = (n & 0x0f0f0f0f0f0f0f0ful) + ((n>>4) & 0x0f0f0f0f0f0f0f0ful);
    n = (n & 0x00ff00ff00ff00fful) + ((n>>8) & 0x00ff00ff00ff00fful);
    n = (n & 0x0000ffff0000fffful) + ((n>>16) & 0x0000ffff0000fffful);
    n = (n & 0x00000000fffffffful) + ((n>>32) & 0x00000000fffffffful);
    return n;
}

#endif

int tool_main(int argc, const char *argv[]) {
    assert(countr_one(7ull) == 3);
    assert(countr_one(15ull + 1024ull) == 4);
    return 0;
}
