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

template<class ElementType, size_t Rank, std::integral IndexType = int>
using shared = mdshared<ElementType, Kokkos::dextents<IndexType, Rank>>;

template<class ElementType, size_t Rank, std::integral IndexType = int>
using span = Kokkos::mdspan<ElementType, Kokkos::dextents<IndexType, Rank>>;

template<class ElementType, size_t... Sizes>
using fixed_span = Kokkos::mdspan<ElementType, Kokkos::extents<int, Sizes>...>;

template<class ElementType,
	 class Extents,
	 class LayoutPolicy = Kokkos::layout_right,
	 class AccessorPolicy = Kokkos::default_accessor<ElementType>>
using mixed_span = Kokkos::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>;

template<class ElementType, size_t Rank, std::integral IndexType = int>
using array = Kokkos::Experimental::mdarray<ElementType, Kokkos::dextents<IndexType, Rank>>;

template<class ElementType, size_t... Sizes>
using fixed_array = Kokkos::Experimental::mdarray<ElementType, Kokkos::extents<int, Sizes...>>;

template<class ElementType, 
	 class Extents,
	 class LayoutPolicy = Kokkos::layout_right,
	 class AccessorPolicy = Kokkos::default_accessor<ElementType>>
using mixed_array = Kokkos::Experimental::mdarray<ElementType,
						  Extents,
						  LayoutPolicy,
						  AccessorPolicy>;

}; // core::md

template<class T>
concept MultiDimSpan = requires (T a, size_t n) {
    typename T::element_type;
    { a.extent(n) } -> std::integral;
    { a.rank() } -> std::integral;
};

std::ostream& operator<<(std::ostream& os, const MultiDimSpan auto& span) {
    using T = decltype(span);
    using E = typename std::decay_t<T>::extents_type;
    if constexpr (E::rank() == 1) {
	os << "[ ";
	for (auto i = 0; i < span.extent(0); ++i)
	    os << span[i] << " ";
	os << "]";
    } else if constexpr (E::rank() == 2) {
	for (auto i = 0; i < span.extent(0); ++i) {
	    os << "[ ";
	    for (auto j = 0; j < span.extent(1); ++j)
		os << span[i, j] << " ";
	    os << "]" << endl;
	}
    } else if constexpr (E::rank() == 3) {
	for (auto i = 0; i < span.extent(0); ++i) {
	    os << "[ ";
	    for (auto j = 0; j < span.extent(1); ++j) {
		os << "[ ";
		for (auto k = 0; k < span.extent(2); ++k)
		    os << span[i, j, k] << " ";
		os << "]" << endl;
	    }
	    os << "]" << endl;
	}
    }
    return os;
}

int main(int argc, const char *argv[]) {
    core::md::array<int, 2> x(6, 10);
    core::md::fixed_array<int, 3, 3> y{};
    core::md::shared<int, 2> z(7, 8);

    for (auto i = 0; i < x.extent(0); ++i)
	for (auto j = 0; j < x.extent(1); ++j)
	    x[i, j] = i * x.extent(1) + j;
    
    for (auto i = 0; i < x.extent(0); ++i) {
	for (auto j = 0; j < x.extent(1); ++j)
	    cout << x[i, j] << " ";
	cout << endl;
    }

    cout << x << endl;

    core::md::fixed_array<int, 3, 3, 3> w{};
    for (auto i = 0; i < w.extent(0); ++i)
	for (auto j = 0; j < w.extent(1); ++j)
	    for (auto k = 0; k < w.extent(2); ++k)
		w[i, j, k] = i * w.extent(1) * w.extent(2) + j * w.extent(2) + k;
    cout << w << endl;
    return 0;
}
