// Copyright (C) 2022, 2023 by Mark Melton
//

#include <string>

size_t constexpr const_cstr_length(const char* str) {
    return *str ? 1 + const_cstr_length(str + 1) : 0;
}

std::string_view operator ""_bs(const char *str) {
    return std::string_view{str, const_cstr_length(str)};
}

int main() {
    return 0;
}
