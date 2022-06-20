#include <functional>
#include <memory>
#include <iostream>
#include <string>
#include <string_view>
#include <filesystem>

#define varName(x) #x
#define typeName(x) typeid(x).name()

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
    std::filesystem::path p("./resource/index.html");
    bool ret = std::filesystem::exists(p);
    if(!ret){
        std::cout << "no such file" << std::endl;
    }
    else{
        auto fsz = std::filesystem::file_size(p);
        std::cout << "size: " << fsz << std::endl;
        std::cout << "full path: " << p << std::endl;
        std::cout << "file name: " << p.stem() << std::endl;
        std::cout << "file extention: " << p.extension() << std::endl;
    }
}

void test_stringview(){
    std::string str("haha");
    std::string_view view(str);
    std::cout << view << std::endl;
    str = "xixixixixixixixixixixixi";
    std::cout << view << std::endl;
}

void test_var_type_name(){
    Data d;
    auto vname = varName(1);
    auto tname = typeName(d);
    std::cout << vname << std::endl;
    std::cout << tname << std::endl;
}


int main(){
    
    // test_stringview();
    // test_filesystem();
    test_var_type_name();

    return 0;
}