// Copyright (C) 2022, 2023 by Mark Melton
//
#include <any>
#include <functional>
#include <unordered_map>
#include <string>
#include <iostream>
#include <memory>
 
class Base;

class myFactory
{
public:
    typedef std::unordered_map<std::string, std::any> registry_map;
 
    virtual ~myFactory() = default;
 
    static registry_map & registry()
    {
        static registry_map impl;
        return impl;
    }

    template<typename ...Ts>
    static std::shared_ptr<Base> instantiate(std::string const & name, const Ts&...args)
    {
	using CreateFunc = std::function<std::shared_ptr<Base>(Ts...)>;
        auto it = registry().find(name);
        if ( it == registry().end()) return nullptr;
        auto& create_fun = std::any_cast<CreateFunc&>(it->second);
        return create_fun(args...);
    }

    template<typename F>
    static bool sign(const std::string& name, F* func)
    {
        registry()[name] = std::function<F>(func);
        return true;
    }
};
 
class Base: public myFactory
{
    public:
        Base(int a):var1(a){}
        Base(int a, int b):var1(a+b){}
        virtual ~Base() = default;
        virtual void f() = 0;
        int var(){return var1;}
    
    private:
        int var1;
};
 
class DerivedExample : public Base
{
private:
    static bool sign1;
    static bool sign2;

public:
    DerivedExample(int a):Base(a){std::cout << var()<< std::endl;}
    DerivedExample(int a, int b):Base(a,b){std::cout << a << b << std::endl;}
    
    static std::shared_ptr<Base> create(int a) { return std::make_shared<DerivedExample>(a);}
    static std::shared_ptr<Base> create(int a, int b) { return std::make_shared<DerivedExample>(a,b);}

    virtual void f() override { std::cout << "DerivedExample" << std::endl; }
};

bool DerivedExample::sign1 = DerivedExample::myFactory::sign("DerivedExample1", static_cast<std::shared_ptr<Base> (*)(int)>(&DerivedExample::create));
bool DerivedExample::sign2 = DerivedExample::myFactory::sign("DerivedExample1", static_cast<std::shared_ptr<Base> (*)(int, int)>(&DerivedExample::create));


int main()
{
    std::shared_ptr<Base> p1 = Base::instantiate("DerivedExample1", 10);
    std::shared_ptr<Base> p2 = Base::instantiate("DerivedExample1", 1, 2);
    p1->f();
    p2->f();
    return 0;
}
