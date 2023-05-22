// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>
struct My1
{
    My1()
    {
        std::cout << "My1 constructor" << std::endl;
    }
    
    ~My1()
    {
        std::cout << "My1 destructor" << std::endl;
    }
    
};

void* thread1(void* pParam)
{
    std::shared_ptr<My1>* pp1 = (std::shared_ptr<My1>*)pParam;
    
    std::cout << "thread1 ref count: " << pp1->use_count() << std::endl;
    
    delete pp1;

    return 0;
}

int main(int argc, const char * argv[])
{
    if(1)
    {
        std::shared_ptr<My1> p1 = std::make_shared<My1>();
        
        std::cout << "ref count: " << p1.use_count() << std::endl;
        
        std::shared_ptr<My1>* pp1 = new std::shared_ptr<My1>(p1);
        pthread_t thr1;
        if(pthread_create(&thr1, nullptr, thread1, (void*)pp1) != 0)
        {
            //Failed to start thread
            delete pp1;
        }
        
        std::cout << "ref count: " << p1.use_count() << std::endl;
    }
    
    int i;
    std::cin >> i;

    return 0;
}
