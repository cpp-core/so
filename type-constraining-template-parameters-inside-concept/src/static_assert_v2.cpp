// Copyright (C) 2022, 2023 by Mark Melton
//

#include <type_traits>
#include <tuple>

template<class T>
concept HasFunc = requires (T a) { a.func(); };

template<class T, size_t... Is>
consteval bool hash_func_impl(std::index_sequence<Is...>) {
    static_assert((HasFunc<std::tuple_element_t<Is, T>> && ...));
    return (HasFunc<std::tuple_element_t<Is, T>> && ...);
}

template<class T, size_t... Is>
consteval bool not_hash_func_impl(std::index_sequence<Is...>) {
    static_assert((not HasFunc<std::tuple_element_t<Is, T>> || ...));
    return (not HasFunc<std::tuple_element_t<Is, T>> || ...);
}

template<class T, size_t N = std::tuple_size_v<T>>
concept HasFuncsN = hash_func_impl<T>(std::make_index_sequence<N>{});

template<class T, size_t N = std::tuple_size_v<T>>
concept NotHasFuncsN = not_hash_func_impl<T>(std::make_index_sequence<N>{});

struct good { void func(); };
struct bad { };

using ok = std::tuple<good, good, good>;
using nope = std::tuple<good, bad, good>;

int main(int argc, const char *argv[]) {
    static_assert(HasFuncsN<ok>);
    static_assert(NotHasFuncsN<nope>);
    static_assert(HasFuncsN<nope>);
    return 0;
}
