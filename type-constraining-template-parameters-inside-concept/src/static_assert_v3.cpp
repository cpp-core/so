// Copyright (C) 2022, 2023 by Mark Melton
//

#include <type_traits>
#include <tuple>

template<class T, class U>
struct push_front_typelist {
    template<class Tx, class Lx>
    struct impl;

    template<class Tx, template<class...> class Lx, class... Us>
    struct impl<Tx, Lx<Us...>> { using type = Lx<Tx, Us...>; };

    using type = typename impl<T, U>::type;
};

template<class T, class U>
using push_front_typelist_t = typename push_front_typelist<T, U>::type;

template<class T, class F>
struct filter_typelist {
    template<class Lx>
    struct impl;

    template<template<class...> class Lx>
    struct impl<Lx<>> { using type = Lx<>; };

    template<template<class...> class Lx, class U, class... Us>
    struct impl<Lx<U, Us...>> {
	using remainder = typename filter_typelist<Lx<Us...>, F>::type;
	using prepended = push_front_typelist_t<U, remainder>;
	using type = std::conditional_t<F{}.template operator()<U>(), remainder, prepended>;
    };

    using type = typename impl<T>::type;
};

template<class T, class F>
using filter_typelist_t = typename filter_typelist<T, F>::type;

template<class T>
concept HasFunc = requires (T a) { a.func(); };

struct has_func {
    template<class T>
    consteval bool operator()() const { return HasFunc<T>; }
};

template<class T, class U = filter_typelist_t<T, has_func>>
concept HasFuncN = (std::tuple_size_v<U> == 0) or HasFunc<std::tuple_element_t<0, U>>;
    
struct good { void func(); };
struct bad { };

using ok = std::tuple<good, good, good>;
using nope = std::tuple<good, bad, good>;

int main(int argc, const char *argv[]) {
    static_assert(HasFuncN<ok>);
    static_assert(HasFuncN<nope>);
    static_assert(not HasFuncN<ok>);
    static_assert(not HasFuncN<nope>);
    return 0;
}
