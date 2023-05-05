
TLDR;
=====

Use the [PFR Library](git@github.com:apolukhin/pfr_non_boost.git)
which is available either as part of Boost or standalone as header
only. They use some very clever standards-compliant meta-programming
to deduce the types in (possibly nested) structures as well as a
tuple-like interface to such structures.

DIY
===

Since you are only asking to enfoce type requirements, you can get by
without all the machinery in the library which also supports runtime
tuple-like access. The following is a bare-bones outline of how you
might go about the task. 

You can find the full code and with build instructions on
[GitHub](https://github.com/cpp-core/so.git). The code works for the
basic examples, but there are likely some bugs and other shortcomings
that could be improved as the code is only an outline.

The final product of our development process will be the following
template which will return `true` if `T` is a scalar but not a pointer
or if `T` is a (possibly nested) structure with such members.

```c++
template<class T>
inline constexpr bool criteria_v;
```

Sample Structs
--------------

```c++
struct Foo {
    char a;
    int b;
    double c;
};

struct Bar {
    int *ptr;
};
```

Given our sample structs, we want to be able to write the following assertions.

```c++
static_assert(criteria_v<int>);
static_assert(not criteria_v<int*>);
static_assert(criteria_v<Foo>);
static_assert(not criteria_v<Bar>);
```

The following family of overloads for the function `constructible`
allows us to determine at compile-time if an aggregate initalization
with a particular number of parameters is valid for our target type
`T`.

```c++
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
```

We can test `constructible` with our sample struct `Foo` and see that
aggregate initialization succeeds with at most three parameters (as
expected since it has three members).

```c++
// Foo can be initlaized with 0, 1, 2, or 3 arguments.
static_assert(constructible<Foo>(std::index_sequence<>{}));
static_assert(constructible<Foo>(std::index_sequence<1>{}));
static_assert(constructible<Foo>(std::index_sequence<1, 2>{}));
static_assert(constructible<Foo>(std::index_sequence<1, 2, 3>{}));
static_assert(not constructible<Foo>(std::index_sequence<1, 2, 3, 4>{}));
static_assert(not constructible<Foo>(std::index_sequence<1, 2, 3, 4, 5>{}));
```

We known that the maximum possible number of members for our target
type `T` is `sizeof(T) * CHAR_BIT` in the instance that every field
was a single bit. We can start with this maximum and recurse towards
zero with the follwing struct to determine the maximum number of
aggregate initializers `T` accepts and return that as the field count.

```c++
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
```

We can assert that `Foo` has three fields and `Bar` one.

```c++
// Foo has 3 members and Bar has one member.
static_assert(aggr_field_count_v<Foo> == 3);
static_assert(aggr_field_count_v<Bar> == 1);
```

We can extract the types as a tuple-type using structured binding that
are not actually materialized. I have only included the
specializations for up to 3 members in the struct. This is the only
part of the algorithm that is limited by the code because you have to
manually write the structured bindings as far as I can tell
(i.e. there is no meta-programming trick to make it work for arbitrary
N). I suppose you could use *shutter* a macro, but that might be a
heresy.

```c++
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
```

We can make the following assertions about `Foo` and `Bar`.

```c++
// Foo members should have types char, int, double.
using FooTypes = aggr_field_types<Foo>;
static_assert(std::is_same_v<std::tuple_element_t<0, FooTypes>, char>);
static_assert(std::is_same_v<std::tuple_element_t<1, FooTypes>, int>);
static_assert(std::is_same_v<std::tuple_element_t<2, FooTypes>, double>);

// Bar members should have type int*.
using BarTypes = aggr_field_types<Bar>;
static_assert(std::is_same_v<std::tuple_element_t<0, BarTypes>, int*>);
```

Finally, we can apply the criteria that are of interest, namely that
we want to be able to idenity scalar types (except pointers) and
(possibly nested) structures of such. Now that we have all the tools,
this part is straight forward meta-programming.

```c++
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
```

And, after way too many preliminaries, we can make the relevant assertions.

```c++
static_assert(criteria_v<int>);
static_assert(not criteria_v<int*>);
static_assert(criteria_v<Foo>);
static_assert(not criteria_v<Bar>);
```

Yeah, I am surprised it is possible too.











