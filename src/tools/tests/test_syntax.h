#pragma once

#include "../../common/common.h"

class Foo
{
    public:
    i32 memory = 0;

    Foo(i32 value)
    {
        memory = value;
        printf("Hello I'm Foo %d\n", memory);
    }

    ~Foo()
    {
        printf("Goodbye, I was Foo %d\n", memory);
    }
};

void Test_Syntax()
{
    //char* timerBuf[sizeof(Foo)];
    //Foo* timerPtr = new (timerBuf) Foo(42);
    Foo foo(42);
    
}