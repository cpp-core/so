// Copyright (C) 2022, 2023 by Mark Melton
//

#include "core/util/tool.h"
#include "core/string/lexical_cast.h"
#include "core/string/split.h"

template<class T>
class Matrix {
public:
    Matrix(size_t nrows, size_t ncols)
	: nrows_(nrows)
	, ncols_(ncols)
	, data_(nrows * ncols) {
    }

    auto nrows() const {
	return nrows_;
    }
    
    auto ncols() const {
	return ncols_;
    }
    
    const auto& operator[](size_t i, size_t j) const {
	return data_[i * ncols_ + j];
    }
    
    auto& operator[](size_t i, size_t j) {
	return data_[i * ncols_ + j];
    }
    
private:
    size_t nrows_, ncols_;
    std::vector<T> data_;
};

template<class T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix) {
    for (auto i = 0; i < matrix.nrows(); ++i) {
	for (auto j = 0; j < matrix.ncols(); ++j)
	    os << matrix[i, j] << " ";
	os << endl;
    }
    return os;
}

int tool_main(int argc, const char *argv[]) {
    const int MaxFlow = 1'000'000'009;
    
    std::string line;
    std::getline(cin, line);
    
    auto n = core::str::lexical_cast<int>(line);
    Matrix<int> graph(n, n);
    size_t rdx{};
    while (std::getline(cin, line)) {
	auto fields = core::str::split(line, " ");
	if (fields.size() != n)
	    throw core::runtime_error("expected {} fields, but found {}", n, fields.size());

	for (auto i = 0; i < fields.size(); ++i)
	    graph[rdx, i] = core::str::lexical_cast<int>(fields[i]);
	++rdx;
    }

    cout << "input: " << endl;
    cout << graph << endl;

    for (auto i = 0; i < graph.nrows(); ++i) {
	graph[i, i] = MaxFlow;
	for (auto j = 0; j < graph.ncols(); ++j)
	    if (graph[i, j] < 0)
		graph[i, j] = MaxFlow;
    }

    cout << "flows: " << endl;
    cout << graph << endl;

    for (auto k = 0; k < n; ++k) {
	for (auto i = 0; i < n; ++i) {
	    for (auto j = 0; j < n; ++j) {
		if (graph[i, j] < std::min(graph[i, k], graph[k, j]))
		    graph[i, j] = std::min(graph[i, k], graph[k, j]);
	    }
	}
    }

    cout << "best: " << endl;
    cout << graph << endl;
    

    return 0;
}
