// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>

int main(int argc, const char *argv[]) {
    // ArgParse opts
    // 	(
    // 	 argFlag<'v'>("verbose", "Verbose diagnostics")
    // 	 );
    // opts.parse(argc, argv);

    long long a = 0;
    long long until = 1000000000;
    for (long long i = 0; i < until; i++) {
        a += i;
    }
    std::cout << a << std::endl;

    return 0;
}
