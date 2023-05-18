// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>
#include <filesystem>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

template<class Clock = std::chrono::high_resolution_clock>
class StopWatch
{
public:
    StopWatch()
        : start_tp_(Clock::now())
        , last_tp_(start_tp_)
    { }

    auto now() const
    {
        std::atomic_thread_fence(std::memory_order_relaxed);
        auto current_tp = Clock::now();
        std::atomic_thread_fence(std::memory_order_relaxed);
        return current_tp;
    }

    auto mark()
    {
        auto current_tp = now();
        auto elapsed = current_tp - last_tp_;
        last_tp_ = current_tp;
        return elapsed;
    }

    template<class Units = typename Clock::duration>
    auto elapsed_duration()
    {
        auto elapsed = mark();
        return std::chrono::duration_cast<Units>(elapsed);
    }
    
    template<class Units = typename Clock::duration>
    auto elapsed_time()
    {
        auto elapsed = mark();
        return std::chrono::duration_cast<Units>(elapsed).count();
    }
    
private:
    typename Clock::time_point start_tp_;
    typename Clock::time_point last_tp_;
};

using std::cout, std::endl;

int main(int argc, const char *argv[]) {
    const auto filename = "/tmp/x.out";
    size_t nth = argc >= 2 ? atoi(argv[1]) : 1;
    size_t nmsg = argc >= 3 ? atoi(argv[2]) : 1'000'000;
    size_t msg_per_thread = nmsg / nth;

    std::filesystem::remove(std::filesystem::path(filename));
    auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("/tmp/x.out", true);
    spdlog::logger logger("basic", sink);
    
    StopWatch timer;
    timer.mark();
    std::vector<std::thread> threads;
    for (auto i = 0; i < nth; ++i)
        threads.emplace_back([&]() {
            for (auto i = 0; i < msg_per_thread; ++i) {
                logger.info("This is a log message\n");
                logger.flush();
            }
        });

    for (auto& thread : threads)
        thread.join();
    
    auto ms = timer.elapsed_duration<std::chrono::milliseconds>().count();
    std::cout << ms << " ms" << std::endl;
    
    return 0;
}
