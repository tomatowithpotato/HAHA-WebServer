#include <functional>
#include <memory>
#include <iostream>

class Heyhey{
public:
    Heyhey() = default;
    explicit Heyhey(int x) {x_ = x; std::cout << "int init" << std::endl;}
    explicit Heyhey(bool hehe) {x_ = 12345; std::cout << "bool init" << std::endl;}
    void oh(){
        std::cout << "who are you ? " << x_ << std::endl;
    }

    void shit(){
        std::function<void()> func = std::bind(&Heyhey::oh, this);
        func();
    }

private:
    int x_;
};


int main(){
    Heyhey hey;
    Heyhey hey1(3);
    Heyhey hey2(true);
    std::unique_ptr<Heyhey> ptr = std::make_unique<Heyhey>(5);
    std::function<void()> func = std::bind(&Heyhey::shit, ptr.get());
    hey.shit();
    func();
    std::shared_ptr<Heyhey> hey3;
    std::cout << (hey3 == nullptr) << std::endl;
    return 0;
}