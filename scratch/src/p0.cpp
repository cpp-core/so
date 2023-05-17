// Copyright (C) 2022, 2023 by Mark Melton
//

#include <array>
#include <iostream>

template<size_t N>
void target_function() {
    std::cout << N << std::endl;
}

template<class T, size_t N, std::array<T, N> Arr, size_t... Is>
void indirect_function(std::index_sequence<Is...>) {
    (target_function<Is>(), ...);
}

int main()
{
    constexpr std::array<int, 3> arr{1, 2, 3};
    indirect_function<int, 3, arr>(std::make_index_sequence<arr.size()>{});
    return 0;
}
