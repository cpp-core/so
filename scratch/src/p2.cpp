// Copyright (C) 2022, 2023 by Mark Melton
//

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;
using std::cout, std::endl;

int main(int argc, const char *argv[]) {

    std::atomic<int> num_tasks{};

    auto th = std::thread([&]() {
	cout << "waiting on tasks" << endl;
	num_tasks.wait(0);
	cout << "one task ready" << endl;
	num_tasks.wait(1);
	cout << "two tasks ready" << endl;
    });

    std::this_thread::sleep_for(250ms);
    cout << "adding task" << endl;
    num_tasks = num_tasks + 1;
    num_tasks.notify_one();
    
    std::this_thread::sleep_for(250ms);
    cout << "adding task" << endl;
    num_tasks = num_tasks + 1;
    num_tasks.notify_one();

    if (th.joinable())
	th.join();
    return 0; 
}
