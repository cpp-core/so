// Copyright (C) 2022, 2023 by Mark Melton
//

#include <condition_variable>
#include <chrono>
#include <iostream>
#include <thread>

template<class F>
class DelayedExecution {
public:
    DelayedExecution(std::chrono::milliseconds ms, F&& func)
	: ms_(ms)
	, func_(std::forward<F>(func)) {
    }

    ~DelayedExecution() {
	if (thread_.joinable())
	    thread_.join();
    }

    bool started() const {
	return started_.load();
    }
    
    void start() {
	thread_ = std::thread{ [this]() {
	    std::unique_lock<std::mutex> lock(mutex_);
	    cv_.wait_for(lock, ms_, [this]() { return not enabled_.load(); });
	    if (enabled_) {
		enabled_.store(false);
		started_.store(true);
		lock.unlock();
                func_();
	    }
        }};
    }

    bool stop() {
        enabled_.store(false);
        cv_.notify_one();
	std::unique_lock lock(mutex_);
	return started();
    }
    
private:
    std::chrono::milliseconds ms_;
    F func_;
    std::thread thread_;
    std::condition_variable cv_;
    std::mutex mutex_;
    std::atomic<bool> enabled_{false}, started_{false};
};

using std::cout, std::endl;

int main(int argc, const char *argv[]) {
    DelayedExecution task1(std::chrono::milliseconds{250}, []() { cout << "task1" << endl; });
    DelayedExecution task2(std::chrono::milliseconds{250}, []() { cout << "task2" << endl; });

    task1.start();
    task2.start();
    task1.stop();
    
    return 0;
}
