
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
	
	while(counter <= 20) {
	    {
		std::unique_lock<std::mutex> lck(mutex_);
		cv0_.wait(lck, [this]{ return ready_ == false; });
		ready_ = true;
		std::cout << "ping counter: " << counter << std::endl;
	    }
	    ++counter;
	    cv1_.notify_one();
	}
    }
    
    void pong() {
	
	while(counter < 20) {  
	    {
		std::unique_lock<std::mutex> lck(mutex_);
		cv1_.wait(lck, [this]{ return ready_ == true; });
		ready_ = false;
		std::cout << "pong counter: " << counter << std::endl;
	    }
	    cv0_.notify_one();
	}
    }

private:
    bool ready_{false};
    std::mutex mutex_;
    std::condition_variable cv0_, cv1_; 
    std::atomic<int> counter{};
    std::thread t0_, t1_;
};

int main(){
    PingPong p{};
}
