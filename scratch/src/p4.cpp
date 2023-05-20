// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>
#include <Eigen/Eigen>

using std::cout, std::endl;

int main(int argc, const char *argv[]) {
    Eigen::VectorXi vec{{ 1, 2, 3, 4, 5, 6 }};
    Eigen::MatrixXi mat{{ 1, 2, 3, 4, 5, 6 },
			{ 7, 8, 9, 10, 11, 12 },
			{ 13, 14, 15, 16, 17, 18 },
			{ 19, 20, 21, 22, 23, 24 }};

    cout << (mat * vec) << endl << endl;

    for (auto i = 0; i < mat.rows(); ++i) {
	int sum{};
	for (auto j = 0; j < mat.cols(); ++j)
	    sum += mat(i, j) * vec(j);
	cout << sum << endl;
    }
	    
  return 0;
}
