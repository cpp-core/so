// Copyright (C) 2022, 2023 by Mark Melton
//

#include "core/util/tool.h"
#include "core/mp/type_name.h"

template<size_t I, class T> requires std::is_aggregate_v<T>
constexpr auto aggregate_field_count_impl() {
    if constexpr (I == 0) return 0;
    else return aggregate_field_count_impl<I - 1, T>();
}

template<class T> requires std::is_aggregate_v<T>
constexpr auto aggregate_field_count() {
    return aggregate_field_count_impl<sizeof(T) * CHAR_BIT, T>();
}

struct Foo {
    int a, b, c;
};

struct Bar {
    int *ptr;
};

struct universal_constructor {
    std::size_t ignore;
    template<class T>
    constexpr operator T& () const;
};

template<class T, size_t I, size_t... Ix,
	 class U = decltype(T{universal_constructor{I}, universal_constructor{Ix}...})>
constexpr bool constructable(std::index_sequence<I, Ix...>) {
    return true;
};

template<class T, size_t... Ix>
constexpr bool constructable(std::index_sequence<Ix...>) {
    return false;
};

static_assert(constructable<Foo>(std::index_sequence<1, 2>{}));
static_assert(constructable<Foo>(std::index_sequence<1, 2, 3>{}));
static_assert(not constructable<Foo>(std::index_sequence<1, 2, 3, 4>{}));

template<class T>
struct aggr_field_count {
    template<class A>
    struct impl;

    template<size_t I, size_t... Ix>
    struct impl<std::integer_sequence<size_t, I, Ix...>> {
	static constexpr bool can_construct = 
	    constructable<T>(std::make_index_sequence<sizeof...(Ix) + 1>{});
	using rtype = impl<std::make_index_sequence<sizeof...(Ix)>>;
	static constexpr size_t value = can_construct ? sizeof...(Ix) + 1
	    : 0;
    };

    static constexpr size_t value = impl<std::make_index_sequence<3>>::value;
};

template<class T, size_t I, size_t... Is>
constexpr auto number_initializers(size_t& out, std::index_sequence<I, Is...>) ->
    decltype(T{universal_constructor{I}, universal_constructor{Is}...}) {
    out = sizeof...(Is) + 1;
}

template<class T, size_t... Is>
constexpr void number_initializers(size_t& out, std::index_sequence<Is...>) {
    number_initializers(out, std::make_index_sequence<sizeof...(Is) - 1>{});
}

int tool_main(int argc, const char *argv[]) {
    ArgParse opts
	(
	 argFlag<'v'>("verbose", "Verbose diagnostics")
	 );
    opts.parse(argc, argv);

    return 0;
}
