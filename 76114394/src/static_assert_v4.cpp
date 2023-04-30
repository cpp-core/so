// Copyright (C) 2022, 2023 by Mark Melton
//

#include <type_traits>
#include <tuple>

template<typename T>
concept has_func = requires(T t)
{ t.func(); };

///////////////////////////// final concept ///////////////////////////////////
template<typename T>
struct check_func
{ using type = T; };

template<template<typename...> typename Tp, typename T>
struct check_func<Tp<T>> : check_func<T> {};

template<template<typename...> typename Tp, typename T, typename... Ts>
struct check_func<Tp<T, Ts...>> : check_func<T> {};

template<template<typename...> typename Tp, has_func T, typename... Ts>
struct check_func<Tp<T, Ts...>> : check_func<Tp<Ts...>> {};

template<typename T>
using check_func_t = typename check_func<T>::type;

template<typename T>
concept has_func_elems = has_func<check_func_t<T>>;

///////////////////////////// testing types ///////////////////////////////////
struct good { void func(); };
struct bad {};

template<typename...>
struct tuple {};

using ok = tuple<good, good, good>;
using nope = tuple<good, bad, good>;
///////////////////////////// unit tests //////////////////////////////////////

int main(int argc, const char *argv[]) {
    static_assert(has_func_elems<ok>);
    static_assert(not has_func_elems<nope>);
    static_assert(has_func_elems<nope>); // good error message:
    return 0;
}
