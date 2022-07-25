#include "base/stringView.h"

int main(){
    const char* str = "hello world!";
    haha::StringView view(str, 5);
    haha::StringView view1(str, strlen(str));
    std::cout << view << std::endl;
    std::cout << view1 << std::endl;
    std::cout << view.starts_with("hello") << std::endl;
    std::cout << view.starts_with("worldd") << std::endl;
    std::cout << view1.ends_with("world!") << std::endl;
    haha::StringView view2(view1);
    view2.remove_prefix(4);
    std::cout << view1 << std::endl;
    std::cout << view2 << std::endl;
    haha::StringView view3 = view1.substr(2);
    std::cout << view3 << std::endl;
    std::cout << view3.find("wor") << std::endl;
    std::cout << view3.find("wor", 4) << std::endl;
    std::cout << view3.find("wor", 5) << std::endl;

    char buffer[100] = "you are very \r\n good haha \r \n xixi\r\n\r\n";
    haha::StringView view4(buffer, strlen(buffer));
    haha::StringView view5(buffer, buffer+10);
    std::cout << view4.find("\r\n") << std::endl;
    std::cout << view5.find("\r\n") << std::endl;
    return 0;
}