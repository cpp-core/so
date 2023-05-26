// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>

using std::cout, std::endl;

struct S0 {
    char c;
    int i;
};

#pragma pack(1)
struct S1 {
    char c;
    int i;
};

int main(int argc, const char *argv[]) {
    
    cout << sizeof(S0) << endl;
    cout << sizeof(S1) << endl;
    
    return 0;
}

