#include <sys/Backtrace.h>

#include <iostream>

void function0()
{
    std::cout << sys::getBacktrace() << std::endl;
}

void function1()
{
    function0();
}

void function2()
{
    function1();
}

void function3()
{
    function2();
}

int main()
{
    function3();
    return 0;
}
