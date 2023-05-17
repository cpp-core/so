// Copyright (C) 2022, 2023 by Mark Melton
//

#include <type_traits>
#include <vector>

template<class... Ts>
struct count;

template<>
struct count<> {
    static constexpr auto value = 0;
};

template<class T, class... Ts>
struct count<T, Ts...> {
    static constexpr auto value = 1 + count<Ts...>::value;
};

template<class... Ts>
inline constexpr auto count_v = count<Ts...>::value;

static_assert(count_v<> == 0);
static_assert(count_v<int> == 1);
static_assert(count_v<int,int> == 2);
static_assert(count_v<int,int,int> == 3);

#include <bit>
#include <iostream>

template<std::integral T>
auto bytes_to_int(const uint8_t *bytes) {
    T value{};
    if constexpr (std::endian::native == std::endian::big) {
        for (auto i = 0; i < sizeof(T); ++i) {
            value <<= 8;
            value += bytes[i];
        }
    } else {
	for (int i = sizeof(T) - 1; i >= 0; --i) {
	    value <<= 8;
	    value += bytes[i];
	}
    }
    return value;
}

union Data {
    uint32_t uin;
    int32_t sin;
    float fin;
    uint8_t u8n[4];
};

int main(int argc, const char *argv[]) {
    std::vector<float> a(10);
    std::vector<Data> b;
    std::transform(a.begin(), a.end(), std::back_inserter(b), [](float x) {
	Data u;
	u.fin = x;
	return u;
    });
    
    uint8_t bytes[] = { 0, 0, 0, 1 };
    std::cout << (std::endian::native == std::endian::little) << std::endl;
    std::cout << bytes_to_int<uint32_t>(bytes);
    return 0;
}
