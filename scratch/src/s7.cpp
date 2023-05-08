// Copyright (C) 2022, 2023 by Mark Melton
//

#include <fmt/format.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>

using std::cin, std::cout, std::endl;
using AdjacencyList = std::vector<std::vector<std::pair<int,int>>>;
using Edge = std::pair<int,int>;
using Edges = std::vector<Edge>;

auto readGraphFromFile(std::istream& inFile) {
    int vertexCount{};
    AdjacencyList adjacencyList{};
    
    // Read vertex count
    inFile >> vertexCount;
    adjacencyList.resize(vertexCount);

    // Read edges
    std::string line;
    std::getline(inFile, line); // Read the rest of the line after vertexCount
    while (std::getline(inFile, line)) {
        std::istringstream lineStream(line);
        int vertex, neighbor, weight;
        lineStream >> vertex;
	while (lineStream >> neighbor >> weight) {
	    adjacencyList[vertex].push_back(std::make_pair(neighbor, weight));
	}
    }
    
    return adjacencyList;
}

auto getMST(const AdjacencyList& list) {
    auto n = list.size();
    std::vector<bool> visited(n, false);
    std::vector<int> parent(n, -1);
    std::vector<int> key(n, std::numeric_limits<int>::max());
    Edges edges;
    
    // Start from vertex 0
    key[0] = 0;
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
    pq.push({0, 0});

    while (!pq.empty()) {
        auto [w, u] = pq.top();
        pq.pop();
        if (visited[u]) {
            continue;
        }
        visited[u] = true;
        if (parent[u] != -1) {
            edges.push_back({parent[u], u});
        }
        for (const auto& [v, weight] : list[u]) {
            if (!visited[v] && weight < key[v]) {
                key[v] = weight;
                pq.push({key[v], v});
                parent[v] = u;
            }
        }
    }

    return edges;
}

int main() {
    auto list = readGraphFromFile(cin);
    for (auto i = 0; i < list.size(); ++i) {
	cout << fmt::format("vertex({}) : ", i);
	for (auto [n, w] : list[i])
	    cout << fmt::format("({},{}) ", n, w);
	cout << endl;
    }

    auto edges = getMST(list);
    for (auto [a, b] : edges)
	cout << fmt::format("{} -> {}", a, b) << endl;
    
    return 0;
}
