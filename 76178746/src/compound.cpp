// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>
#include <string>
#include <set>

void print_compound(const std::set<std::string>& words, const std::string& word) {
    for (auto i = 1; i < word.size(); ++i) {
	const auto& w1 = word.substr(0, i);
	const auto& w2 = word.substr(i);
	if (words.contains(w1) and words.contains(w2)) {
	    std::cout << w1 << "-" << w2 << std::endl;
	    break;
	}
    }
}

int main(int argc, const char *argv[]) {

    std::vector<std::string> data = {
	"A", "book", "and", "a", "worm", "started", "a", "fire", "in", "the", "house",
	"It", "was", "a", "bookworm", "housefire"
    };

    std::set<std::string> words;

    for (const auto& word : data)
	words.insert(word);

    for (const auto& word : data)
	print_compound(words, word);
    
    return 0;
}
