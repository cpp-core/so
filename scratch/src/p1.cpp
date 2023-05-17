// Copyright (C) 2022, 2023 by Mark Melton
//

#include <atomic>
#include <thread>
#include <iostream>

class AtomicTest {
 public:
  int AsyncTask() {
    std::thread timer([this](){
      while (not stop_.load(std::memory_order_acquire)) {
        counter_.fetch_add(1, std::memory_order_relaxed);
        std::this_thread::sleep_for(std::chrono::microseconds(10));
      }
      std::cout << "counter = " << counter_ << std::endl;
    });
    
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    stop_.store(true, std::memory_order_release);
    timer.join();
    return 0;
  }

 private:
  std::atomic<int> counter_{0};
  std::atomic<bool> stop_{false};
};

int main(int argc, const char *argv[]) {
    AtomicTest test;
    test.AsyncTask();
    return 0;
}
