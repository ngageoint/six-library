#include <fstream>

#include <sys/OS.h>
#include <sys/Path.h>
#include "TestCase.h"

namespace
{
void createFile(const std::string& pathname)
{
    std::ofstream oss(pathname.c_str());
    oss.write(pathname.c_str(), pathname.length());
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
    try
    {
        os.remove(subdir1);
    }
    catch (...)
    {
        TEST_ASSERT(false);
    }
    TEST_ASSERT( !os.exists(subdir1) );
}

TEST_CASE(testForcefulMove)
{
    // This assumes the user has write permissions in their current directory
    const sys::OS os;
    const sys::Path subdir1("subdir1");
    TEST_ASSERT( os.makeDirectory(subdir1) );
    createFile(subdir1.join("tempFile1"));
    createFile(subdir1.join("tempFile2"));
    createFile(subdir1.join("tempFile3"));

    const sys::Path subdir2(subdir1.join("subdir2"));
    TEST_ASSERT( os.makeDirectory(subdir2) );

    // regular move
    if (!os.move(subdir1.join("tempFile2"), subdir2.join("tempFile2")))
    {
        TEST_ASSERT(false);
    }

    // forceful move
    if (!os.move(subdir1.join("tempFile3"), subdir1.join("tempFile1")))
    {
        TEST_ASSERT(false);
    }

    // Try to recursively remove from the top level
    try
    {
        os.remove(subdir1);
    }
    catch (...)
    {
        TEST_ASSERT(false);
    }
    TEST_ASSERT( !os.exists(subdir1) );
}

TEST_CASE(testEnvVariables)
{
    sys::OS os;
    const std::string testvar = "TESTVARIABLE";
    const std::string testvalue = "TESTVALUE";
    const std::string testvalue2 = "TESTVALUE2";

    // Start by clearing the environment variable, if set.
    os.unsetEnv(testvar);    
    TEST_ASSERT_FALSE(os.isEnvSet(testvar));

    // Check getEnv throws an sys::SystemException exception when trying unset var
    
    TEST_SPECIFIC_EXCEPTION(os.getEnv(testvar),sys::SystemException);

    // Test getEnvIfSet doesn't update value and returns false on unset var.
    std::string candidatevalue="Unset";

    TEST_ASSERT_FALSE(os.getEnvIfSet(testvar, candidatevalue));
    TEST_ASSERT_EQ(candidatevalue,"Unset");

    // Set the environment variable
    os.setEnv(testvar, testvalue, true);

    TEST_ASSERT(os.isEnvSet(testvar));

    TEST_ASSERT(os.getEnvIfSet(testvar, candidatevalue));
    TEST_ASSERT_EQ(candidatevalue,testvalue);
    std::string getEnvVar = os.getEnv(testvar);
    TEST_ASSERT_EQ(getEnvVar,testvalue);

    // Set the environment variable without overwrite. (Should not update).
    os.setEnv(testvar, testvalue2,  false);

    TEST_ASSERT(os.getEnvIfSet(testvar, candidatevalue));
    TEST_ASSERT_EQ(candidatevalue,testvalue);
    getEnvVar = os.getEnv(testvar);
    TEST_ASSERT_EQ(getEnvVar,testvalue);

    // Finally unset the variable again.
    os.unsetEnv(testvar);
    
    TEST_ASSERT_FALSE(os.isEnvSet(testvar));
}

}

int main(int, char**)
{
    TEST_CHECK(testRecursiveRemove);
    TEST_CHECK(testForcefulMove);
    TEST_CHECK(testEnvVariables);
    return 0;
}
