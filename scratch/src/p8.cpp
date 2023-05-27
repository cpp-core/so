// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>
#include <cstddef>
#include <cstdint>
#include <tuple>

template <typename /*CRTP*/>
struct calc_offset;

template <template <size_t> class ElemTraits>
struct calc_offset<ElemTraits<0>>
{
    constexpr static size_t value = 0;
};

template <template <size_t> class ElemTraits, size_t Index>
struct calc_offset<ElemTraits<Index>>
{
    constexpr static size_t _get_value() noexcept {
        constexpr auto roundUp = [](size_t num, size_t multiple) -> size_t {
            const size_t mod = num % multiple;
            return mod == 0 ? num : num + multiple - mod;
        };

        using prev_traits = ElemTraits<Index - 1>;

        const size_t type_alignment = std::alignment_of<typename ElemTraits<Index>::value_type>::value,
            prev_type_size = sizeof(typename prev_traits::value_type),
            prev_offset = prev_traits::offset;

        return roundUp(prev_offset + prev_type_size, type_alignment);
    }

    constexpr static size_t value = _get_value();
};

template <class>
struct calc_packed_offset;

template <template <size_t> class ElemTraits>
struct calc_packed_offset<ElemTraits<0>>
{
    constexpr static size_t value = 0;
};

template <template <size_t> class ElemTraits, size_t Index>
struct calc_packed_offset<ElemTraits<Index>>
{
    using prev_traits = ElemTraits<Index - 1>;
    
    constexpr static size_t value = 
	sizeof(typename prev_traits::value_type) + prev_traits::packed_offset;
};

// not a real structure
template <typename POD, typename Fields>
struct pod_traits
{
    template <size_t I>
    struct element
    {
        using value_type = std::tuple_element_t<I, Fields>;
        using pod_type = POD;
        constexpr static size_t offset = calc_offset<element<I>>::value;
        constexpr static size_t packed_offset = calc_packed_offset<element<I>>::value;
    };
};

struct normal
{
    uint8_t _0;
    uint64_t _1;
    uint8_t _2;
    uint16_t _3;
};

using normal_types = std::tuple<decltype(normal{}._0),
                                decltype(normal{}._1),
                                decltype(normal{}._2),
                                decltype(normal{}._3)>;
using normal_traits = pod_traits<normal, normal_types>;

static_assert(offsetof(normal, _0) == normal_traits::element<0>::offset);
static_assert(offsetof(normal, _1) == normal_traits::element<1>::offset);
static_assert(offsetof(normal, _2) == normal_traits::element<2>::offset);
static_assert(offsetof(normal, _3) == normal_traits::element<3>::offset);


#pragma pack(push, 1)
struct packed_1
{
    uint8_t _0;
    uint64_t _1;
    uint8_t _2;
    uint16_t _3;
};
#pragma pack(pop)

using packed_1_types = std::tuple<decltype(packed_1{}._0),
                                decltype(packed_1{}._1),
                                decltype(packed_1{}._2),
                                decltype(packed_1{}._3)>;
using packed_1_traits = pod_traits<packed_1, packed_1_types>;

static_assert(offsetof(packed_1, _0) == packed_1_traits::element<0>::packed_offset);
static_assert(offsetof(packed_1, _1) == packed_1_traits::element<1>::packed_offset);
static_assert(offsetof(packed_1, _2) == packed_1_traits::element<2>::packed_offset);
static_assert(offsetof(packed_1, _3) == packed_1_traits::element<3>::packed_offset);

// using Types = std::tuple<int8_t, uint64_t, uint8_t, uint16_t>;

// template<class... Ts>
// struct Foo {
//     Ts...;
// };

using std::cout, std::endl;

int main(int argc, const char *argv[]) {

    cout << offsetof(normal, _0) << endl;
    cout << offsetof(normal, _1) << endl;
    cout << offsetof(normal, _2) << endl;
    cout << offsetof(normal, _3) << endl;
    cout << endl;
    cout << offsetof(packed_1, _0) << endl;
    cout << offsetof(packed_1, _1) << endl;
    cout << offsetof(packed_1, _2) << endl;
    cout << offsetof(packed_1, _3) << endl;
    cout << endl;
    cout << packed_1_traits::element<0>::packed_offset << endl;
    cout << packed_1_traits::element<1>::packed_offset << endl;
    cout << packed_1_traits::element<2>::packed_offset << endl;
    cout << packed_1_traits::element<3>::packed_offset << endl;
    return 0;
}
