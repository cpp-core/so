// Copyright (C) 2022, 2023 by Mark Melton
//

#include <type_traits>
#include <utility>
#include <tuple>

// Our samples types.
struct Foo {
    char a;
    int b;
    double c;
};

struct Bar {
    int *ptr;
};

// Can be used to aggregate initialize any type but on in an
// unevaluation context since the call operator is declared but not
// defined.
struct universal_type {
    std::size_t ignore;
    template<class T>
    constexpr operator T& () const;
};

// `constructible` has three overloads which can be used to determine
// if T can be aggregate initlaized with a given number of arguments.

// Can we aggregate initialize T with no arguements?
template<class T, class U = decltype(T{})>
constexpr bool constructible(std::index_sequence<>) {
    return true;
};

// Can we aggregate initialize T with sizeof...(Ix) + 1 arguments?
template<class T, size_t I, size_t... Ix,
	 class U = decltype(T{universal_type{I}, universal_type{Ix}...})>
constexpr bool constructible(std::index_sequence<I, Ix...>) {
    return true;
};

// If neither of the other overloads are choosen, then we must not be
// able to aggregate initialize T with sizeof...(Ix) arguments.
template<class T, size_t... Ix>
constexpr bool constructible(std::index_sequence<Ix...>) {
    return false;
};

// Foo can be initlaized with 0, 1, 2, or 3 arguments.
static_assert(constructible<Foo>(std::index_sequence<>{}));
static_assert(constructible<Foo>(std::index_sequence<1>{}));
static_assert(constructible<Foo>(std::index_sequence<1, 2>{}));
static_assert(constructible<Foo>(std::index_sequence<1, 2, 3>{}));
static_assert(not constructible<Foo>(std::index_sequence<1, 2, 3, 4>{}));
static_assert(not constructible<Foo>(std::index_sequence<1, 2, 3, 4, 5>{}));

// Returns the number of members of T. Utilizes the contructible
// overloads as helpers.
template<class T>
struct aggr_field_count {
    template<size_t N>
    struct impl;

    template<size_t N> requires (N == 0)
    struct impl<N> { static constexpr size_t value = 0; };

    template<size_t N> requires (N > 0)
    struct impl<N> {
	static constexpr bool good = constructible<T>(std::make_index_sequence<N>{});
	static constexpr size_t value = good ? N : impl<N - 1>::value;
    };

    static constexpr size_t value = impl<sizeof(T)>::value;
};

template<class T>
inline constexpr auto aggr_field_count_v = aggr_field_count<T>::value;

// Foo has 3 members and Bar has one member.
static_assert(aggr_field_count_v<Foo> == 3);
static_assert(aggr_field_count_v<Bar> == 1);

// Wrapper for containing field types.
template<class... Ts>
struct aggr_field_list {
    using type = std::tuple<Ts...>;
};

template<class T, size_t N>
struct aggr_field_type_impl;

template<class T>
struct aggr_field_type_impl<T, 0> {
    static auto ignore() { return aggr_field_list<>{};  }
    using type = decltype(ignore());
};

template<class T>
struct aggr_field_type_impl<T, 1> {
    static auto ignore() {
	T *x = nullptr; auto [a] = *x;
	return aggr_field_list<decltype(a)>{};
    }
    using type = decltype(ignore());
};

template<class T>
struct aggr_field_type_impl<T, 2> {
    static auto ignore() {
	T *x = nullptr; auto [a, b] = *x;
	return aggr_field_list<decltype(a), decltype(b)>{};
    }
    using type = decltype(ignore());
};

template<class T>
struct aggr_field_type_impl<T, 3> {
    static auto ignore() {
	T *x = nullptr; auto [a, b, c] = *x;
	return aggr_field_list<decltype(a), decltype(b), decltype(c)>{};
    }
    using type = decltype(ignore());
};

template<class T, size_t N = aggr_field_count_v<T>>
using aggr_field_types = typename aggr_field_type_impl<T, N>::type::type;

// Foo members should have types char, int, double.
using FooTypes = aggr_field_types<Foo>;
static_assert(std::is_same_v<std::tuple_element_t<0, FooTypes>, char>);
static_assert(std::is_same_v<std::tuple_element_t<1, FooTypes>, int>);
static_assert(std::is_same_v<std::tuple_element_t<2, FooTypes>, double>);

// Bar members should have type int*.
using BarTypes = aggr_field_types<Bar>;
static_assert(std::is_same_v<std::tuple_element_t<0, BarTypes>, int*>);

// We want to detect scalars (except pointers) and structures that contains such.

template<class T>
struct criteria_impl;

template<class T> requires (not std::is_aggregate_v<T>)
struct criteria_impl<T> {
    static constexpr bool value = std::is_scalar_v<T> and not std::is_pointer_v<T>;
};

template<class T> requires (std::is_aggregate_v<T>)
struct criteria_impl<T> {
    using U = aggr_field_types<T>;
    static constexpr bool value = criteria_impl<U>::value;
};

template<class... Ts> 
struct criteria_impl<std::tuple<Ts...>> {
    static constexpr bool value = (criteria_impl<Ts>::value and ...);
};

template<class T>
inline constexpr bool criteria_v = criteria_impl<T>::value;

static_assert(criteria_v<int>);
static_assert(not criteria_v<int*>);
static_assert(criteria_v<Foo>);
static_assert(not criteria_v<Bar>);

int main(int argc, const char *argv[]) {
    return 0;
}
