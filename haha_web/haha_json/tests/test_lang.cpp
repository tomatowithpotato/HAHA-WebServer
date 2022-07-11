#include <stdio.h>
#include <string_view>
#include <memory>
#include <iostream>
#include <string>
#include <fstream>

class A{
public:
    A(const std::string &name = ""):name_(name){ std::cout << "construct A:" << name_ << std::endl; }
    A(const A& a) { 
        name_ = a.name_;
        std::cout << "copy construct A:" << name_ << std::endl; 
    }
    ~A(){ std::cout << "deconstruct A:" << name_ << std::endl; }
private:
    std::string name_;
};

void test_utf(){
    char utf_str[6] = {0};
    std::string_view view("\u7814ss");

    sprintf(utf_str, "u%04x", *(unsigned char *)view.data());

    printf("%s\n", view.data());
    printf("%s\n", utf_str);
}

void test_shared_ptr(){
    A a1("a1");
    std::shared_ptr<A> p1 = std::make_shared<A>(a1);
    std::shared_ptr<A> p2(new A("a2"));
    auto a2 = *p2;
}

void test_fstream(){
    std::ofstream ofs("./test_fstream.txt");
    ofs << "hehe\n";
}

int main(){
    // test_utf();
    // test_shared_ptr();
    test_fstream();
    return 0;
}