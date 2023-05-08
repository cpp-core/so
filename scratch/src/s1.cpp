
#include <iostream>

auto simple_count(const std::string& pattern, const std::string& str) {
    size_t n{};
    for (auto i = 0; i < str.size() - pattern.size() + 1; ++i) {
	if (str.substr(i, pattern.size()) == pattern)
	    ++n;
    }
    return n;
}

auto find_count(const std::string& pattern, const std::string& str) {
    size_t n{};
    auto pos = str.find(pattern, 0);
    while (pos != std::string::npos) {
	++n;
	pos = str.find(pattern, pos + 1);
    }
    return n;
}

int main(int argc, const char *argv[]) {

    // Read the pattern and string.
    std::string pattern, str;
    std::getline(std::cin, pattern);
    std::getline(std::cin, str);

    // Output the count.
    std::cout << simple_count(pattern, str) << std::endl;
    std::cout << find_count(pattern, str) << std::endl;
    
    return 0;
}
