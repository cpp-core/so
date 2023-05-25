// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>

// Rule of three with copy-and-swap idiom
class A {
public:
    A() {
	x = new int[10];
    }

    A(const A& other) : A() {
	std::copy(other.x, other.x + 10, x);
    }

    const A& operator=(A other) {
        std::swap(*this, other);
        return *this;
    }

    ~A() {
        delete x;
    }
    
private:
    int *x{nullptr};
};

// Copy-and-swap idiom using a unique pointer
class AUP {
public:
    AUP() {
        x = std::make_unique<int[]>(10);
    }

    AUP(const AUP& other) : AUP() {
        std::copy(other.x.get(), other.x.get() + 10, x.get());
    }

    const AUP& operator=(AUP other) {
        std::swap(*this, other);
        return *this;
    }
    
private:
    std::unique_ptr<int[]> x;
};

int main() {
    A a;
    A b = a;
    AUP aup;
    AUP bup = aup;
}
