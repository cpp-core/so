// Copyright (C) 2022, 2023 by Mark Melton
//

#include <cassert>
#include <iostream>

int* scopeProblem()
{
    int *bret;
    int b=10;
    bret =&b;
    
    return bret;
}

auto foo() {
    int a = 0, b = 1, c = 2;
    return a + b + c;
}

int main(int argc, const char *argv[]) {
    auto ptr = scopeProblem();
    auto a = foo();
    std::cout << *ptr << std::endl;
    assert(a == 3);
    return 0;
}
