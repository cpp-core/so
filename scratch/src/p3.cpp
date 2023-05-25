// Copyright (C) 2022, 2023 by Mark Melton
//

#include <algorithm>
#include <iostream>
#include <vector>

using std::cout, std::endl;

void subSort(std::vector<int>& array) {
    int seqStart = 0;
    for (int i = 0; i < array.size() - 1; i++) {
        if (array[i] < array[i + 1]) {
	    std::sort(&array[seqStart], &array[i + 1]);
            seqStart = i + 1;
        }
    }
    std::sort(&array[seqStart], &array[array.size()]);
}

int main(int argc, const char *argv[]) {
    // std::vector<int> data = { 53, 50, 41, 8, 64, 35, 17, 76, 58, 3, 75, 1, 99, 56, 2 };
    std::vector<int> data = { 1, 2, 3, 3, 2, 1 };
    subSort(data);
    
    // int ldx{};
    // bool was_up{}, was_down{};
    // for (auto i = 0; i < data.size() - 1; ++i) {
    // 	bool up = data[i] < data[i+1];
    // 	bool down = data[i] > data[i+1];
    // 	if ((was_up and down) or (was_down and up)) {
    // 	    std::sort(&data[ldx], &data[i+1]);
    //         ldx = i + 1;
    // 	    was_down = false;
    // 	    was_up = false;
    // 	} else {
    // 	    was_down = down;
    // 	    was_up = up;
    // 	}
    // }
    // std::sort(&data[ldx], &data[data.size()]);

    for (auto elem : data)
	cout << elem << " ";
    
    return 0;
}
