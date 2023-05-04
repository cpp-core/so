// Copyright (C) 2022, 2023 by Mark Melton
//

#include "core/util/tool.h"
#include "core/mp/type_name.h"

struct Foo {
    char a;
    int b;
    double c;
};

struct Bar {
    int *ptr;
};

struct universal_type {
    std::size_t ignore;
    template<class T>
    constexpr operator T& () const;
};

template<class T, class U = decltype(T{})>
constexpr bool constructible(std::index_sequence<>) {
    return true;
};

template<class T, size_t I, size_t... Ix,
	 class U = decltype(T{universal_type{I}, universal_type{Ix}...})>
constexpr bool constructible(std::index_sequence<I, Ix...>) {
    return true;
};

template<class T, size_t... Ix>
constexpr bool constructible(std::index_sequence<Ix...>) {
    return false;
};

static_assert(constructible<Foo>(std::index_sequence<>{}));
static_assert(constructible<Foo>(std::index_sequence<1>{}));
static_assert(constructible<Foo>(std::index_sequence<1, 2>{}));
static_assert(constructible<Foo>(std::index_sequence<1, 2, 3>{}));
static_assert(not constructible<Foo>(std::index_sequence<1, 2, 3, 4>{}));
static_assert(not constructible<Foo>(std::index_sequence<1, 2, 3, 4, 5>{}));

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

template<class T, size_t N>
struct aggr_field_type_impl;

template<class T>
struct aggr_field_type_impl<T, 0> {
    static auto ignore() { return std::tuple<>{};  }
    using type = decltype(ignore());
};

template<class T>
struct aggr_field_type_impl<T, 1> {
    static auto ignore() {
	T *x = nullptr; auto [a] = *x;
	return std::tuple<decltype(a)>{};
    }
    using type = decltype(ignore());
};

template<class T>
struct aggr_field_type_impl<T, 2> {
    static auto ignore() {
	T *x = nullptr; auto [a, b] = *x;
	return std::tuple<decltype(a), decltype(b)>{};
    }
    using type = decltype(ignore());
};

template<class T>
struct aggr_field_type_impl<T, 3> {
    static auto ignore() {
	T *x = nullptr; auto [a, b, c] = *x;
	return std::tuple<decltype(a), decltype(b), decltype(c)>{};
    }
    using type = decltype(ignore());
};

template<class T, size_t N = aggr_field_count_v<T>>
using aggr_field_types = typename aggr_field_type_impl<T, N>::type;

int tool_main(int argc, const char *argv[]) {
    ArgParse opts
	(
	 argFlag<'v'>("verbose", "Verbose diagnostics")
	 );
    opts.parse(argc, argv);

    using foo_types = aggr_field_types<Foo>;
    cout << core::mp::type_name<std::tuple_element_t<0, foo_types>>() << endl;
    cout << core::mp::type_name<std::tuple_element_t<1, foo_types>>() << endl;
    cout << core::mp::type_name<std::tuple_element_t<2, foo_types>>() << endl;

    return 0;
}
