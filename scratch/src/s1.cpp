
#include <iostream>
#include <condition_variable>
#include <atomic>
#include <thread>

class PingPong {
public:
    PingPong() {
	t0_ = std::thread(&PingPong::ping, this);
	t1_ = std::thread(&PingPong::pong, this);
    }

    ~PingPong() {
	if (t0_.joinable())
	    t0_.join();
	if (t1_.joinable())
	    t1_.join();
    }
    
    void ping() {
	
	while(counter_ <= 20) {
	    {
		std::unique_lock<std::mutex> lck(mutex_);
		cv0_.wait(lck, [this]{ return ready_ == false; });
		ready_ = true;
		std::cout << "ping counter: " << counter_ << std::endl;
	    }
	    ++counter_;
	    cv1_.notify_one();
	}
    }
    
    void pong() {
	
	while(counter_ < 20) {  
	    {
		std::unique_lock<std::mutex> lck(mutex_);
		cv1_.wait(lck, [this]{ return ready_ == true; });
		ready_ = false;
		std::cout << "pong counter: " << counter_ << std::endl;
	    }
	    cv0_.notify_one();
	}
    }

private:
    bool ready_{false};
    std::mutex mutex_;
    std::condition_variable cv0_, cv1_; 
    std::atomic<int> counter_{};
    std::thread t0_, t1_;
};

class PingPongAtomicFlag {
public:
    PingPongAtomicFlag() {
	t0_ = std::thread([this]() { ping(); });
	t1_ = std::thread([this]() { pong(); });
    }

    ~PingPongAtomicFlag() {
	if (t0_.joinable())
	    t0_.join();
	if (t1_.joinable())
	    t1_.join();
    }
    
    void ping() {
	
	while(counter_ <= 20) {
	    potato_.wait(true);
	    std::cout << "ping counter: " << counter_ << std::endl;
	    potato_.test_and_set();
	    ++counter_;
	    potato_.notify_one();
	}
    }
    
    void pong() {
	
	while(counter_ < 20) {
	    potato_.wait(false);
	    std::cout << "pong counter: " << counter_ << std::endl;
	    potato_.clear();
	    potato_.notify_one();
	}
    }

private:
    std::atomic_flag potato_;
    std::atomic<int> counter_{};
    std::thread t0_, t1_;
};

int main(){
    PingPong p{};
    // PingPongAtomicFlag paf{};
}
