#include <thread>
#include <string>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include "signal.h"
#include <atomic>

class Class
{
public:
    Class(const std::string& s, const int& i) : m_data(s), threadName(i),counter(0) { }
    ~Class() { 
        if (m_thread.joinable())
        {
            m_thread.join();
        }
        if (m_thread2.joinable()){
            m_thread2.join();
        }

        std::cout << "destructor called" << std::endl;
        }
    void runThread() { 
        m_thread = std::thread(&Class::printThreadWrite, this); 
        m_thread2 = std::thread(&Class::printThreadRead,this);
    }

private:
    std::mutex m_printmutex;
    std::condition_variable m_conditionvar;
    std::string m_data;
    std::thread m_thread;
    std::thread m_thread2;
    std::atomic<bool> signalScheduleSwitch;
    int threadName;

    std::atomic<int> counter;
    void printThreadWrite()  { 
        
            while(1){
                {
                std::lock_guard<std::mutex> lg(m_printmutex);
                std::cout << "thread # " << std::this_thread::get_id() << " "  << m_data << '\n'; 
                counter ++;
                signalScheduleSwitch = true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                m_conditionvar.notify_one();
            }
        
    }
    
    void printThreadRead()  { 
    
        while (1)
        {
            std::unique_lock<std::mutex> uLock(m_printmutex);
            m_conditionvar.wait(uLock,[this](){ return signalScheduleSwitch == true;});
            std::cout << "counter value is:" << counter << "\n";
            signalScheduleSwitch = false;

        }
    }
};

void signalHandler(int s){
    printf("caught signal %d Exiting\n", s);
    exit(1);

}

int main(int argc, char** argv)
{
    signal(SIGINT, signalHandler);
    try
    {
        Class c("Hello, world!",1);
        c.runThread();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
