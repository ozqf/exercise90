#pragma once

struct Foo
{
    int a;
    int b;
    int c;

    Foo()
    {
        printf("ctor Foo\n");
    }

    ~Foo()
    {
        printf("dtor Foo\n");
    }

    int AccessA()
    {
        return a;
    }
    
    int AccessB()
    {
        return b;
    }
    
    void Set(int newA, int newB, int newC)
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
    {
        int i = 7;
        int j = 6;
        printf("Block entered: %d\n", (7 + 6));
    }
}
