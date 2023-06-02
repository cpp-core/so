// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>

using std::cout, std::endl;

std::string delete_substr(const std::string& str, int sIdx, int nChar) {
    std::string answer;
    std::copy(str.begin(), str.begin() + sIdx, std::back_inserter(answer));
    std::copy(str.begin() + sIdx + nChar, str.end(), std::back_inserter(answer));
    return answer;
}

std::string delStr(const std::string& str, int sIdx, int nChar) {
    std::string answer;
    int i{};
    while (i < str.size()) {
	if (i == sIdx)
	    i += nChar;
	else
	    answer.push_back(str[i++]);
    }
    return answer;
}

int main(int argc, const char *argv[]) {
    std::string s0 = "abcdefghi";
    cout << delete_substr(s0, 3, 3) << endl;
    cout << delStr(s0, 3, 3) << endl;
    return 0;
}
