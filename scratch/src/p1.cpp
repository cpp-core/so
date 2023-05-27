// Copyright (C) 2022, 2023 by Mark Melton
//

#include <chrono>
#include <iostream>
#include <mutex>
#include <queue>
#include <set>
#include <thread>
#include <vector>

using std::cin, std::cout, std::endl;
using namespace std::chrono_literals;

using PlayerId = int;
using Queue = std::queue<PlayerId>;
using Players = std::set<PlayerId>;

int main(int argc, const char *argv[]) {
    int initial_number_players{};
    cin >> initial_number_players;

    Players players;
    for (auto i = 0; i < initial_number_players; ++i)
        players.insert(i);

    while (players.size() > 1) {
        Queue queue;
        std::vector<std::thread> threads;

        std::atomic<int> chairs{}, loser{};
        for (auto pid : players) {
            threads.emplace_back([&,pid]() {
                std::this_thread::sleep_for(250ms);
                auto cid = chairs.fetch_add(1);
                if (cid == players.size() / 2)
                    loser = pid;
            });
        }
        for (auto& th : threads)
            if (th.joinable())
                th.join();

        cout << "Player " << loser << " did not capture a chair" << endl;
        players.erase(loser);
    }

    cout << "Player " << *players.begin() << " won" << endl;

    return 0;
}
