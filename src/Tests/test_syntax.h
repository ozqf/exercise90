#pragma once

struct Foo
{
    int a;
    int b;
    int c;

    inline Foo()
    {
        printf("ctor Foo\n");
    }

    inline ~Foo()
    {
        printf("dtor Foo\n");
    }

    inline int AccessA()
    {
        return a;
    }
    
    inline int AccessB()
    {
        return b;
    }
    
    inline void Set(int newA, int newB, int newC)
    {
        a = newA;
        b = newB;
        c = newC;
    }
};

void Test_Syntax()
{
    Foo f = {};
    f.Set(3, 7, 2);

    Foo* ptrF = &f;
    int r = ptrF->AccessA();
    int r2 = ptrF->AccessB();
    printf("ptrFoo results: %d and %d\n", r, r2);
}
