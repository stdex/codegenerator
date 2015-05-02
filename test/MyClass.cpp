#include "MyClass.h"

int MyClass::min(int n1, int n2)
{
    return n1 < n2 ? n1 : n2;
}

int MyClass::max(int n1, int n2)
{
    return n1 > n2 ? n1 : n2;
}

int MyClass::dump(char name)
{
    return name;
}