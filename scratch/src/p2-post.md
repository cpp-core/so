

You might want to first build an intermediate map from id to parent id
and write a function that takes the map and an id and return a list of
the parents. The function has a natural recursive implementation. With
these in hand, it is easy to loop over the items and construct the
final data structure that you want.

I know the post is tagged as `Java`, but I think the following code
still clearly illustrates the algorithm for getting to a solution.

Sample Code
-----------
```c++
#include <map>
#include <vector>
#include <iostream>

using Triplet = std::tuple<int, char, std::optional<int>>;
using Triplets = std::vector<Triplet>;
using TripletMap = std::map<int, std::pair<char, std::optional<int>>>;
using ArrayList = std::tuple<int, char, std::vector<int>>;
using ArrayLists = std::vector<ArrayList>;

using std::cout, std::endl;

std::optional<int> parent(const TripletMap& map, int id) {
    if (auto iter = map.find(id); iter != map.end())
        return iter->second.second;
    return std::nullopt;
}

void parents(const TripletMap& map, int id, std::vector<int>& vec) {
    if (auto p = parent(map, id); p) {
        vec.push_back(*p);
        parents(map, *p, vec);
    }
}

auto parents(const TripletMap& map, int id) {
    std::vector<int> ids;
    parents(map, id, ids);
    return ids;
}

int main(int argc, const char *argv[]) {
    // The test data
    Triplets triples = {
        { 1, 'a', 10 },
        { 2, 'b', 20 },
        { 3, 'c', std::nullopt },
        { 10, 'm', 12 },
        { 12, 'f', 14 },
        { 20, 'x', 22 },
        { 22, 'y', 23 },
        { 23, 'z', 26 }
    };

    // Add each id to a map for easy lookup.
    TripletMap map;
    for (auto [id, name, parent] : triples)
        map[id] = std::make_pair(name, parent);

    ArrayLists arr;
    for (auto [id, name, parent] : triples)
        arr.emplace_back(id, name, parents(map, id));

    for (auto [id, name, parents] : arr) {
        cout << id << " " << name << " [ ";
        for (auto id : parents)
            cout << id << " ";
        cout << "]";
        cout << endl;
    }

    return 0;
}
```
