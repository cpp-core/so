// Copyright (C) 2022, 2023 by Mark Melton
//

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

class H2O {
    std::mutex m;
    std::condition_variable m_cond;
    int idx;
public:
    H2O() : idx(0) {
    }

    void hydrogen(std::function<void()> releaseHydrogen) {
        std::unique_lock<std::mutex> mlock(m);
        m_cond.wait(mlock, [this](){return idx != 0;});
        // releaseHydrogen() outputs "H". Do not change or remove this line.
        releaseHydrogen();
        idx = (idx+1) % 3;
        m_cond.notify_all();
    }

    void oxygen(std::function<void()> releaseOxygen) {
        std::unique_lock<std::mutex> mlock(m);
        m_cond.wait(mlock, [this](){return idx == 0;});
        // releaseOxygen() outputs "O". Do not change or remove this line.
        releaseOxygen();
        idx = (idx+1)%3;
        m_cond.notify_all();
    }
};

class H2OAtomic {
    /* state is one of {00, 01, 10, 20, 21} where the first digit represents the number of hydrogen atoms acquires and the second digit is the number of oxygen atoms acquired */
    std::atomic<int> state_{0};
    /* stores the number of atoms that we have finished processing, increments from 0 to 3 and resets back to 3*/
    std::atomic<int> completedCount_{0};

public:
    H2OAtomic() {}

    void acquireHydrogen(){
        int curState = state_.load();
        do{
            while(curState/10 == 2){
                // full, 2 hydrogen atoms have already been acquired
                curState = state_.load();
            }
        } while(!state_.compare_exchange_weak(curState, curState + 10));
            // modify the state to show that 1 more hydrogen has been acquired
    }

    void acquireOxygen(){
        int curState = state_.load();
        do{
            while(curState % 10 == 1){
                // full, 1 oxygen has already been acquired
                curState = state_.load();
            }
        } while(!state_.compare_exchange_weak(curState, curState + 1));
            // modify the state to show that 1 oxygen has been acquired
    }

    void complete(){
        // increment count of completed
        completedCount_.fetch_add(1);
        int expected = 3;
        /* The thread that resets the completed count back to 0 is responsible for resetting the acquired state as well.
        If more than 1 acquired thread tries to reset state, in between 2 of these resets a new set of atoms might already be acquired which we don't want to write over. */
        if(completedCount_.compare_exchange_strong(expected, 0)){
            state_.store(0);
        }
    }
    void hydrogen(std::function<void()> releaseHydrogen) {
        acquireHydrogen();
        releaseHydrogen(); // prints "H"
        complete();
    }

    void oxygen(std::function<void()> releaseOxygen) {
        acquireOxygen();
        releaseOxygen(); // prints "O"
        complete();
    }
};

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

void release_hydrogen() {
    // cout << "H";
}

void release_oxygen() {
    // cout << "O";
}

template<class Builder, class T, class U>
void build_water(int n, T&& h, U&& o) {
    Builder builder;
    auto h0th = std::thread([&]() {
        for (auto i = 0; i < n; ++i)
            builder.hydrogen(h);
    });
    auto h1th = std::thread([&]() {
        for (auto i = 0; i < n; ++i)
            builder.hydrogen(h);
    });
    auto oth = std::thread([&]() {
        for (auto i = 0; i < n; ++i)
            builder.oxygen(o);
    });

    h0th.join();
    h1th.join();
    oth.join();
}

template<class Work>
void measure(std::string_view desc, Work&& work) {
    StopWatch timer;
    timer.mark();
    work();
    auto n = timer.elapsed_duration<std::chrono::microseconds>().count();
    cout << desc << " : " << n << endl;
}

int main(int argc, const char *argv[]) {
    measure("mutex", [&]() {
        build_water<H2O>(3000, release_hydrogen, release_oxygen);
    });
    measure("atomic", [&]() {
        build_water<H2OAtomic>(3000, release_hydrogen, release_oxygen);
    });
    
    return 0;
}
