
#include <iostream>
#include <exception>
#include <import/mt.h>
#include <import/sys.h>

sys::Mutex globalMutex;

/*-- except::Exception --*/
void print(char a)
{
	globalMutex.lock();
    for(int i=0; i<5; ++i)
    {
        std::cout << a << std::endl;
    }
    globalMutex.unlock();
    throw except::Exception(Ctxt("char exception"));
}

class PrintChar : public sys::Runnable
{
public:
    char a;
    PrintChar(char in): a(in)
    {
    }
    virtual ~PrintChar()
    {
    }

    virtual void run() override
    {
        print(a);
    }
};

/*-- std::exception --*/
struct stdExcept: std::exception
{
	const char* what() const noexcept override { return "std::exception in add"; }
};

void add(sys::Uint32_T x, sys::Uint32_T y)
{
	globalMutex.lock();
    std::cout << "x: " << x << " y: " << y << std::endl;
    std::cout << "x + y = " << x+y << std::endl;
    globalMutex.unlock();
    throw stdExcept();
}

class AddInts : public sys::Runnable
{
public:
    sys::Uint32_T x, y;
    AddInts(sys::Uint32_T x_in, sys::Uint32_T y_in): x(x_in), y(y_in)
    {
    }
    virtual ~AddInts()
    {
    }

    virtual void run() override
    {
        add(x,y);
    }
};

/*-- (...) exception --*/
void printConcat(const std::string& x, const std::string& y)
{
	globalMutex.lock();
	std::cout << x+y << std::endl;
	globalMutex.unlock();
	throw double();
}

class ConcatStr: public sys::Runnable
{
public:
	std::string x;
	std::string y;
	ConcatStr(std::string x_in, std::string y_in): x(x_in), y(y_in)
    {
    }
    virtual ~ConcatStr()
    {
    }

    virtual void run() override
    {
    	printConcat(x,y);
    }
};


int main()
{
    try
    {
        mt::ThreadGroup threads;
        threads.createThread(new PrintChar('A'));
        threads.createThread(new AddInts(3,4));
        threads.createThread(new ConcatStr("hello,", "world!"));
        threads.joinAll();

    }
    catch (except::Throwable& t)
    {
        std::cout << "Exception Caught(main): " << t.toString() << std::endl;
    }
    catch (...)
    {
        std::cout << "Exception Caught(main)!" << std::endl;
    }
    return 0;
}
