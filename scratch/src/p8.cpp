// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>
#include <mdspan/mdspan.hpp>
#include <mdspan/mdarray.hpp>

using std::cout, std::endl;

template<class T, size_t N>
class Matrix : public Kokkos::mdspan<T, Kokkos::dextents<int, N>> {
public:
    using Base = Kokkos::mdspan<T, Kokkos::dextents<int, N>>;
    using Base::Base;
    
    Matrix(int r, int c)
	: Base(nullptr, r, c)
	, data_(r * c) {
	*(Base*)this = Base(&data_[0], r, c);
    }
    
private:
    std::vector<T> data_;
};

namespace core::md {
namespace detail {

template<class ElementType>
struct shared_ownership_handle {
    std::shared_ptr<ElementType[]> base;
    size_t offset;
};

template<class ElementType>
auto make_shared_ownership_handle(size_t n, size_t offset = 0) {
    return shared_ownership_handle<ElementType>{std::make_shared<ElementType>(), offset};
}

template<std::integral... Sizes>
constexpr size_t product_dynamic_extents(Sizes... sizes) {
    std::array<size_t, sizeof...(Sizes)> arr{static_cast<size_t>(sizes)...};
    size_t n{1};
    for (auto elem : arr)
	n *= elem;
    return n;
}

template<class ElementType>
struct shared_accessor {
    using offset_policy = shared_accessor;
    using element_type = ElementType;
    using reference = ElementType&;
    using data_handle_type = shared_ownership_handle<ElementType>;
    
    constexpr shared_accessor() noexcept = default;
    
    template<class OtherElementType>
    constexpr shared_accessor(shared_accessor<OtherElementType>) noexcept  {};
    
    constexpr data_handle_type offset(data_handle_type h, size_t i) const noexcept {
	return data_handle_type{h.base, h.offset + i};
    }
    
    constexpr reference access(data_handle_type h, size_t i) const noexcept {
	return h.base.get()[h.offset + i];
    };
};

}; // detail

template<class ElementType,
	 class Extents,
	 class LayoutPolicy = Kokkos::layout_right,
	 class AccessorPolicy = detail::shared_accessor<ElementType>>
class mdshared : public Kokkos::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy> {
public:
    using Base = Kokkos::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>;

    template<std::integral... Args>
    mdshared(Args... dynamic_extents)
	: Base(detail::make_shared_ownership_handle<ElementType>
	       (detail::product_dynamic_extents(dynamic_extents...)),
	       dynamic_extents...) {
    }
};

template<size_t Rank, std::integral IndexType = int>
using mdshared_dynamic = mdshared<IndexType, Kokkos::dextents<IndexType, Rank>>;

}; // core::md

int main(int argc, const char *argv[]) {
    Kokkos::Experimental::mdarray<int, Kokkos::dextents<int, 2>> x(6, 10);
    // core::md::mdshared_dynamic<2> x(7, 7);

    for (auto i = 0; i < x.extent(0); ++i)
	for (auto j = 0; j < x.extent(1); ++j)
	    x[i, j] = i * x.extent(1) + j;
    
    for (auto i = 0; i < x.extent(0); ++i) {
	for (auto j = 0; j < x.extent(1); ++j)
	    cout << x[i, j] << " ";
	cout << endl;
    }
    return 0;
}
