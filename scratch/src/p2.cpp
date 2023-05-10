// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>

struct Foo {
    static void bar(int) { }
    static void baz(int) { }
    static void baz(std::string) { }
};


struct universal_type {
    std::size_t ignore;
    constexpr operator int () const;
    template<class T>
    constexpr operator T& () const;
};


// template<class U = decltype(Foo::baz())>
// constexpr bool callable(std::index_sequence<>) {
//     return true;
// }

template<class T, size_t I, size_t... Ix,
	 class U = decltype(T::baz(universal_type{I}, universal_type{Ix}...))>
constexpr bool callable(std::index_sequence<I, Ix...>) {
    return true;
}

template<class T, size_t... Ix>
constexpr bool callable(std::index_sequence<Ix...>) {
    return false;
}

static_assert(callable<Foo>(std::index_sequence<1>{}));

using std::cout, std::endl;

int main(int argc, const char *argv[]) {
    return 0;
}
