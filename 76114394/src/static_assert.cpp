// Copyright (C) 2022, 2023 by Mark Melton
//

#include <type_traits>
#include <tuple>

template<class T>
concept HasFunc = requires (T a) { a.func(); };

template<size_t I, class T>
concept HasFuncI = std::tuple_size_v<T> <= I or HasFunc<std::tuple_element_t<I, T>>;

template<class T>
concept HasFuncs = HasFuncI<0, T> and HasFuncI<1, T> and HasFuncI<2, T> and HasFuncI<3, T> and
    HasFuncI<4, T> and HasFuncI<5, T> and HasFuncI<6, T> and HasFuncI<7, T> and
    std::tuple_size_v<T> < 8;

struct good { void func(); };
struct bad { };

using ok = std::tuple<good, good, good>;
using nope = std::tuple<good, bad, good>;

int main(int argc, const char *argv[]) {
    static_assert(HasFuncs<ok>);
    static_assert(HasFuncs<nope>);
    return 0;
}
