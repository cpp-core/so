// Copyright (C) 2022, 2023 by Mark Melton
//

#include <type_traits>
#include <tuple>
#include <variant>

template<template<class> class T, class... Ts>
struct variant_wrapper_impl;

// The base case is that all types come packaged in a single tuple.
template<template<class> class T, class... Ts>
struct variant_wrapper_impl<T, std::tuple<Ts...>> {
    using type = std::variant<T<Ts>...>;
};

// The first type is not a tuple.
template<template<class> class T, class U, class... Us>
struct variant_wrapper_impl<T, U, Us...> {
    using type = typename variant_wrapper_impl<T, std::tuple<U>, Us...>::type;
};

// The first type is a tuple and the second type is not a tuple.
template<template<class> class T, class... Ts, class U, class... Us>
struct variant_wrapper_impl<T, std::tuple<Ts...>, U, Us...> {
    using type = typename variant_wrapper_impl<T, std::tuple<Ts..., U>, Us...>::type;
};

// The first type is a tuple and the second type is a tuple.
template<template<class> class T, class... Ts, class... Us, class... Ws>
struct variant_wrapper_impl<T, std::tuple<Ts...>, std::tuple<Us...>, Ws...> {
    using type = typename variant_wrapper_impl<T, std::tuple<Ts..., Us...>, Ws...>::type;
};

template<template<class> class T, class... Ts>
using variant_wrapper_t = typename variant_wrapper_impl<T, Ts...>::type;

template<class T>
class Wrapper {
};

static_assert(std::is_same_v<variant_wrapper_t<Wrapper, int, float>,
 	      std::variant<Wrapper<int>, Wrapper<float>>>);

static_assert(std::is_same_v<variant_wrapper_t<Wrapper, std::tuple<int, float>>,
	      std::variant<Wrapper<int>, Wrapper<float>>>);

static_assert(std::is_same_v<variant_wrapper_t<Wrapper, std::tuple<int, float>, double>,
 	      std::variant<Wrapper<int>, Wrapper<float>, Wrapper<double>>>);

static_assert(std::is_same_v<variant_wrapper_t<Wrapper, char, std::tuple<int, float>, double>,
 	      std::variant<Wrapper<char>, Wrapper<int>, Wrapper<float>, Wrapper<double>>>);

int main(int argc, const char *argv[]) {
    return 0;
}
