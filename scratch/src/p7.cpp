// Copyright (C) 2022, 2023 by Mark Melton
//

#include <algorithm>
#include <iostream>
#include <vector>

using std::cout, std::endl;

class Matrix {
public:
    Matrix(int nr, int nc)
	: nrows_(nr)
	, ncols_(nc)
	, data_(nrows_ * ncols_) {
    }

    auto nrows() const {
	return nrows_;
    }
    
    auto ncols() const {
	return ncols_;
    }
    
    auto operator[](int i, int j) const {
	return data_[i * ncols_ + j];
    }
	
    auto& operator[](int i, int j) {
	return data_[i * ncols_ + j];
    }
	
private:
    int nrows_, ncols_;
    std::vector<int> data_;
};

enum class Color {
    Red, Blue, Green, Yellow, Black
};

using ColorPair = std::pair<Color, Color>;
using ColorPairs = std::vector<ColorPair>;
using Order = std::vector<int>;

auto cost(const ColorPair& a, const ColorPair& b) {
    int c{};
    if (a.first != b.first and a.first != b.second)
	++c;
    if (a.second != b.second and a.second != b.second)
	++c;
    return c;
}

auto brute_force(const Matrix& matrix, int sdx) {
    const int n = matrix.nrows();
    std::vector<int> vertex, optimal;
    for (auto i = 0; i < n; ++i)
	if (i != sdx)
	    vertex.push_back(i);

    auto min_cost = std::numeric_limits<int>::max();
    while (std::next_permutation(vertex.begin(), vertex.end())) {
	int curr_cost{};
	auto j = sdx;
	for (int i = 0; i < vertex.size(); ++i) {
	    curr_cost += matrix[j, vertex[i]];
	    j = vertex[i];
	}
	// Add in the cost of going back to the start.
	curr_cost += matrix[j,sdx];
	if (curr_cost < min_cost) {
	    min_cost = std::min(min_cost, curr_cost);
	    optimal = vertex;
	}
    }
    optimal.insert(optimal.begin(), sdx);
    return std::make_pair(min_cost, optimal);
}

int main(int argc, const char *argv[]) {
    ColorPairs parts = {
	{ Color::Red, Color::Blue },
	{ Color::Yellow, Color::Green },
	{ Color::Yellow, Color::Red },
	{ Color::Black, Color::Yellow }
    };

    const int n = parts.size();
    Matrix cost_matrix(n, n);
    for (auto i = 0; i < n; ++i)
	for (auto j = 0; j < n; ++j)
	    cost_matrix[i, j] = cost(parts[i], parts[j]);

    auto [c0, p0] = brute_force(cost_matrix, 0);
    cout << "cost " << c0 << " : ";
    for (auto p : p0)
	cout << p << " -> ";
    cout << endl;
    return 0;
}
