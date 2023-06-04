// Copyright (C) 2022, 2023 by Mark Melton
//

#include <array>
#include <iostream>

using std::cin, std::cout, std::endl;


template<size_t D, class T>
struct Vec {
    Vec() : data_() { }

    template<class... Us> requires (sizeof...(Us) == D and (std::is_convertible_v<Us, T> and ...))
    Vec(Us... args)
	: data_({args...}) {
    }
    
    Vec(const std::vector<T>& other) {
	assert(other.size() == D);
	std::copy(other.begin(), other.end(), data_.begin());
    }

    template<class U>
    Vec(const Vec<D, U>& other) {
	std::copy(other.begin(), other.end(), data_.begin());
    }

    auto& operator[](size_t idx) { return data_[idx];  }
    const auto& operator[](size_t idx) const { return data_[idx]; }

    T& x() { return data_[0]; }
    const T& x() const { return data_[0]; }
    
    T& y() requires (D > 1) { return data_[1]; }
    const T& y() const requires (D > 1) { return data_[1];  }
    
    T& z() requires (D > 2) { return data_[2]; }
    const T& z() const requires (D > 2) { return data_[2]; }
    
    T& w() requires (D > 3) { return data_[3]; }
    const T& w() const requires (D > 3) { return data_[3]; }

private:
    std::array<T, D> data_;
};


int main(int argc, const char *argv[]) {
    Vec<1, int> a(1);
    cout << a.x() << endl;
    
    Vec<2, int> b(1, 2);
    cout << b.x() << " " << b.y() << endl;
    
    Vec<3, int> c(1, 2, 3);
    cout << c.x() << " " << c.y() << " " << c.z() << endl;
    
    Vec<4, int> d(1, 2, 3, 4);
    cout << d.x() << " " << d.y() << " " << d.z() << " " << d.w() << endl;
}
