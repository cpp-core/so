// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>

using std::cout, std::endl;

template<typename T>
class Wrapper {
    T t;
public:
    explicit Wrapper(T t) : t(t) {}

    void call() {
	cout << "Wrapper called with " << typeid(T).name() << endl;
	t();
    };
};

class Problem {
public:
    void test() { }

    // Case 1: Lambda
    static constexpr auto l = []() { };
    Wrapper<decltype(l)> x{l};

    // Case 2: Pointer to member
    Wrapper<std::function<void()>> y{std::bind(&Problem::test, this)};
};

int main() {
    Wrapper w{[]() { }};
    w.call();
    
    Problem p;
    p.x.call();
    p.y.call();

    // You need an object to invoke a pointer to member.
    // auto y = Wrapper(&Problem::test);
    // y.call(); // Fails to compile

    // This is how to use pointer to member.
    (&p)->Problem::test();
}
