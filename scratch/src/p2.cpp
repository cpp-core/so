// Copyright (C) 2022, 2023 by Mark Melton
//

#include <algorithm>
#include <map>
#include <string>
#include <iostream>

using Map = std::map<std::string,std::string>;

int main(int argc, const char *argv[]) {
    Map symbols = { { "alignas", "ALIGNAS" },
		    { "alignof", "ALIGNOF" },
		    { "and", "AND" },
		    { "and_eq", "AND_EQ" },
		    { "asm", "ASM" },
		    { "atomic_cancel", "ATOMIC_CANCEL" }
    };

    auto iter = symbols.upper_bound("amex");
    std::cout << iter->first << std::endl;
    return 0;
}
