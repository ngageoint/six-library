#include <fstream>

#include <sys/OS.h>
#include <sys/Path.h>

#include "TestCase.h"

namespace
{
void createFile(const std::string& pathname)
{
    std::ofstream oss(pathname.c_str());
    oss.close();
}

TEST_CASE(testRecursiveRemove)
{
    // This assumes the user has write permissions in their current directory
    const sys::OS os;
    const sys::Path subdir1("subdir1");
    TEST_ASSERT( os.makeDirectory(subdir1) );
    createFile(subdir1.join("tempFile1"));
    createFile(subdir1.join("tempFile2"));

    const sys::Path subdir2(subdir1.join("subdir2"));
    TEST_ASSERT( os.makeDirectory(subdir2) );

    const sys::Path subdir3(subdir2.join("subdir3"));
    TEST_ASSERT( os.makeDirectory(subdir3) );
    createFile(subdir3.join("tempFile3"));
    createFile(subdir3.join("tempFile4"));
    createFile(subdir3.join("tempFile5"));

    // Try to recursively remove from the top level
    TEST_ASSERT( os.remove(subdir1) );
    TEST_ASSERT( !os.exists(subdir1) );
}
}

int main(int argc, char** argv)
{
    TEST_CHECK(testRecursiveRemove);

    return 0;
}
