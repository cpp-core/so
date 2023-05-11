// Copyright (C) 2022, 2023 by Mark Melton
//

#include <concepts>
#include <string>
#include <tuple>

// Does T have an inner type named `inner`.
template<class T>
concept Inner = requires(T x) {
    typename T::inner;
};

// Do T and U have same inner type.
template<class T, class U>
concept SameInner = Inner<T> and Inner<U> and std::is_same_v<typename T::inner, typename U::inner>;

// Helper for checking that all template parameters satisfy Inner
// concept and all pairs satisfy SameInner.
template<typename>
struct inner_container_impl : std::false_type {};

template<template<typename...> class Tp, Inner T>
struct inner_container_impl<Tp<T>> {
    static constexpr bool value = true;
};

template<template<typename...> class Tp, Inner T, Inner... Ts>
struct inner_container_impl<Tp<T, Ts...>> {
    static constexpr bool value = (SameInner<T, Ts> and ...);
};

// The concept just use the helper.
template<class T>
concept InnerContainer = inner_container_impl<T>::value;

// Test types.
struct type_1 {
    using inner = int;
};

struct type_2 {
    using inner = std::string;
};

struct type_3 {
    using inner = int;
};

template<Inner... Ts> struct container {};

void f(InnerContainer auto) {}

using containers_X = std::tuple<type_1, type_2>;
using containers_Y = std::tuple<type_1, type_3>;

static_assert(not SameInner<type_1, type_2>);
static_assert(not InnerContainer<std::tuple<type_1, type_2>>);
static_assert(not InnerContainer<containers_X>);

int main() {

    // containers_X _x;
    // f(_x);

    containers_Y _y;
    f(_y);

  return 0;
}
