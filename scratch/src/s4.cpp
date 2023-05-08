// Copyright (C) 2022, 2023 by Mark Melton
//

#include <type_traits>
#include <tuple>
#include <variant>

template<template<class> class T, class... Ts>
struct variant_wrapper_impl;

// The simple case: W, int, float -> variant<W<int>, W<float>>
template<template<class> class T, class... Ts>
struct variant_wrapper_impl {
    using type = std::variant<T<Ts>...>;
};

// The tuple case: W, tuple<int, float> -> variant<W<int>, W<float>>
//
// We use the pattern matching of partial specialization to extract
// the tuple sub-types into Ts...
template<template<class> class T, class... Ts>
struct variant_wrapper_impl<T, std::tuple<Ts...>> {
    using type = std::variant<T<Ts>...>;
};

// The tuple+ case: W, tuple<int, float>, double -> variant<W<int>, W<float>, W<double>>
//
// We again use the pattern matching of partial specialization to
// extract the tuple sub-types into Ts.. and then recurse to the
// simple case.
template<template<class> class T, class... Ts, class... Us>
struct variant_wrapper_impl<T, std::tuple<Ts...>, Us...> {
    using type = typename variant_wrapper_impl<T, Ts..., Us...>::type;
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

int main(int argc, const char *argv[]) {
    return 0;
}
