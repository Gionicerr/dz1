#include <iostream>
#include "aaaa.h"

int main(){
    int a = 15;
    int* per = &a;

    UniquePtr<int> ptr(per);
    UniquePtr<int> ptr1;

    std::cout<<ptr.get()<<std::endl;
    std::cout<<*ptr<<std::endl;
}