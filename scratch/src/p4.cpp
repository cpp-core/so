// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>
#include <Eigen/Eigen>

using std::cout, std::endl;

template<class T, class U>
void push_back(T& vec, const U& value) {
    auto tmp = vec;
    vec.resize(vec.size() + 1);
    for (auto i = 0; i < tmp.size(); ++i)
	vec[i] = tmp[i];
    vec[vec.size() - 1] = value;
}

int main(int argc, const char *argv[]) {
    Eigen::VectorXi vec{{ 1, 2, 3, 4, 5, 6 }};
    cout << vec << endl;
    push_back(vec, 7);
    cout << vec << endl;
    
  return 0;
}
