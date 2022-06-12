#include "http/HttpUtil.h"
#include "magic_enum/magic_enum.hpp"
#include <iostream>

int main(){
    for(auto &[k, v] : haha::Ext2HttpContentType){
        std::cout << k << ": ";
        std::cout << magic_enum::enum_name(v) << std::endl;
    }
    std::cout << "********************************************************************" << std::endl;
    for(auto &[k, v] : haha::HttpContentType2Ext){
        std::cout << magic_enum::enum_name(k) << ": ";
        std::cout << v << std::endl;
    }
    return 0;
}