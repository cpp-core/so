// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>
#include <map>
#include <any>

using std::cout, std::endl;

template<class F>
struct ArgType;

template<class T>
struct ArgType<std::function<void(T)>> {
    using type = T;
};

auto any_match(std::any a) {
    throw std::runtime_error("no handler for any_match");
}

template<class T, class... Ts>
auto any_match(std::any a, T&& handler, Ts&&... handlers) {
    using arg_type = typename ArgType<decltype(std::function(handler))>::type;
    if (auto ptr = std::any_cast<arg_type>(&a); ptr)
        return handler(*ptr);
    else
        return any_match(a, std::forward<Ts>(handlers)...);
}

int main(int argc, const char *argv[]) {

    std::any a = 1;
    any_match(a,
	      [](int) { cout << "int" << endl; },
	      [](double) { cout << "double" << endl; }
	      );

    a = 1.0;
    any_match(a,
	      [](int) { cout << "int" << endl; },
	      [](double) { cout << "double" << endl; }
	      );
    
    
    return 0;
}
