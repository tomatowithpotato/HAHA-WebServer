#include <functional>
#include <memory>
#include <iostream>
#include <string>
#include <string_view>
#include <filesystem>

class Data{
public:
    ~Data(){
        std::cout << "~Data" << std::endl;
    }
private:
    int cc = 0;
};

class Heyhey{
public:
    Heyhey() = default;
    ~Heyhey(){
        std::cout << "~Heyhey" << std::endl;
    }
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
    Data data_;
};

void test_pointer(){
    // std::string s("haha");
    // Heyhey hey;
    // Heyhey hey1(3);
    // Heyhey hey2(true);
    // std::unique_ptr<Heyhey> ptr = std::make_unique<Heyhey>(5);
    // std::function<void()> func = std::bind(&Heyhey::shit, ptr.get());
    // hey.shit();
    // func();
    // std::shared_ptr<Heyhey> hey3;
    // std::cout << (hey3 == nullptr) << std::endl;

    // Heyhey *hey = new Heyhey();
    // std::shared_ptr<void> eeee;
    // eeee.reset(hey);
    // std::shared_ptr<Heyhey> oh;
    // oh = std::static_pointer_cast<Heyhey>(eeee);
    // std::cout << eeee.use_count() << std::endl;
}

void test_filesystem(){
    bool ret = std::filesystem::exists("./index.html");
    if(ret == -1){
        std::cout << "no such file" << std::endl;
    }
    else{
        auto fsz = std::filesystem::file_size("./index.html");
        std::cout << "size: " << fsz << std::endl;
    }
}

void test_stringview(){
    std::string str("haha");
    std::string_view view(str);
    std::cout << view << std::endl;
    str = "xixixixixixixixixixixixi";
    std::cout << view << std::endl;
}


int main(){
    
    test_stringview();

    return 0;
}