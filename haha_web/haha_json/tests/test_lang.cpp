#include <stdio.h>
#include <string_view>

int main(){
    char utf_str[6] = {0};
    std::string_view view("\u7814ss");

    sprintf(utf_str, "u%04x", *(unsigned char *)view.data());

    printf("%s\n", view.data());
    printf("%s\n", utf_str);
    return 0;
}