#include <import/except.h>
#include <import/io.h>
#include <import/tiff.h>
#include <fstream>

using namespace sys;

int main(int argc, char **argv)
{
    try
    {
        io::StandardOutStream st;
        tiff::IFDEntry entry(254, 1);
        entry.print(st);
        entry.print(st);

        tiff::IFDEntry entry2(255, 1);
        entry2.print(st);
        entry2.print(st);
    }
    catch (except::Throwable& t)
    {
        std::cerr << "Caught throwable: " << t.toString() << std::endl;
        exit( EXIT_FAILURE);
    }
    catch (...)
    {
        std::cerr << "Caught unnamed exception" << std::endl;
    }
    return 0;
}
