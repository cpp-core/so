// Copyright (C) 2022, 2023 by Mark Melton
//

#include <cassert>
#include <type_traits>
#include <iterator>
#include <vector>

union Data {
    uint32_t uin;
    int32_t sin;
    float fin;
    uint8_t u8n[4];
};

template<class base_iterator>
struct iterator_adaptor {
    using iterator_category = std::input_iterator_tag;
    using different_type = typename base_iterator::difference_type;
    using value_type = Data;
    using pointer = Data*;
    using reference = Data&;

    iterator_adaptor(base_iterator iter)
        : iter_(iter) {
    }

    value_type operator*() const {
        Data d;
        d.fin = *iter_;
        return d;
    }

    iterator_adaptor& operator++() {
        ++iter_;
        return *this;
    }

    iterator_adaptor operator++(int) {
        iterator_adaptor tmp = *this;
        ++(*this);
        return tmp;
    }

    friend bool operator==(const iterator_adaptor& a, const iterator_adaptor& b) {
        return a.iter_ == b.iter_;
    }

    friend bool operator!=(const iterator_adaptor& a, const iterator_adaptor& b) {
	return not (a == b);
    }

private:
    base_iterator iter_;
};

namespace std {
template<class T>
struct iterator_traits<iterator_adaptor<T>> {
    using value_type = typename iterator_adaptor<T>::value_type;
    using difference_type = typename iterator_adaptor<T>::different_type;
    using iterator_category = typename iterator_adaptor<T>::iterator_category;
    using pointer = typename iterator_adaptor<T>::pointer;
    using reference = typename iterator_adaptor<T>::reference;
};
};

int main(int argc, const char *argv[]) {
    std::vector<float> floats = { 1.0, 2.0, 3.0, 4.0, 5.0 };
    std::vector<Data> data(iterator_adaptor(floats.begin()), iterator_adaptor(floats.end()));

    for (auto i = 0; i < floats.size(); ++i)
	assert(floats[i] == data[i].fin);
    
    return 0;
}
