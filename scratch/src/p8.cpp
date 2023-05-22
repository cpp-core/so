// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>

// https://en.wikipedia.org/wiki/Dijkstra's_algorithm
// https://takeuforward.org/data-structure/dijkstras-algorithm-using-priority-queue-g-32/

using NodePair = std::pair<int,int>;
using NodePairs = std::vector<NodePair>;

using DistanceVertex = std::pair<int, int>;
using MinQueue = std::priority_queue<DistanceVertex,
                                  std::vector<DistanceVertex>,
                                  std::greater<DistanceVertex>>;

int main(int argc, const char *argv[]) {
    // The sample problem. We store the graph as a adjacency list
    // using a multimap.
    std::multimap<int, int> edges {
        { 0, 1 },
        { 0, 2 },
        { 1, 4 },
        { 2, 3 },
        { 4, 3 },
        { 0, 4 }
    };
    
    // How many vertices?
    int max_vertex{};
    for (auto [a, b] : edges) {
        max_vertex = std::max(max_vertex, a);
        max_vertex = std::max(max_vertex, b);
    }
    int number_vertices = max_vertex + 1;

    // Initialize the distance from source to each vertex as MAX_INT.
    int source{};
    std::vector<int> distance(number_vertices, std::numeric_limits<int>::max());

    // Initialize distance to source and priority queue
    MinQueue pq;
    distance[source] = 0;
    pq.emplace(0, source);

    while (!pq.empty()) {
        auto [udist, udx] = pq.top();
        pq.pop();

        // Iterate over all neighbors of vdx
        auto [begin, end] = edges.equal_range(udx);
        for (auto iter = begin; iter != end; ++iter) {
            auto vdx = iter->second, vdist = iter->first;

            // If there is a shorter path, record it
	    if (udist + vdist < distance[vdx]) {
		distance[vdx] = udist + vdist;
		pq.push({udist, vdx});
	    }
	}
    }

    // distance now contains the shortest distance between source and each node
    for (auto i = 0; i < number_vertices; ++i)
	std::cout << distance[i] << std::endl;
    
    return 0;
}
