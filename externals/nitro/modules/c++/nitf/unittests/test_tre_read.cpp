
#include <vector>
#include <iostream>
#include <string>

#include <import/nitf.hpp>

#include "TestCase.h"

#if _MSC_VER
#pragma warning(disable: 4459) // declaration of 'testName' hides global declaration
#endif

/*

1. In the `readTRE` function in `modules/c/nitf/Reader.c`, the `tre` pointer on
the stack is not initialized upon creation. Then later, if the function hits an
error condition before it is assigned a value, the `tre` pointer will be used
without being initialized. Specifically, it will be passed to the
`nitf_TRE_destruct` function which will dereference it and then call a function
pointer from within the dereferenced value. As such, if an attacker can control
the previous stack contents, by using the NITF file contents to direct the
execution flow, they may be able to exploit this vulnerability. Depending on the
exact use case and deployment of NITRO, exploit mitigations, such as ASLR, may
prevent exploitation.

2. In the `nitf_Record_unmergeTREs` function in `modules/c/nitf/Record.c`, the
`overflow` pointer on the stack is not initialized upon creation. Then later,
within the `UNMERGE_SEGMENT` macro, the `overflow` pointer is dereferenced in a
call to the `moveTREs` function as the destination extension list to which a TRE
struct is appended. While this vulnerability has a less obvious path to
exploitation than the `readTRE` vulnerability, given the right use case and
stack alignment, it may also be exploited.

3. The [`defaultRead`
function](https://github.com/mdaus/nitro/blob/5c8c30b1c95/modules/c/nitf/source/DefaultTRE.c#L88)
in `modules/c/nitf/source/DefaultTRE.c` does not properly check for overflows
within the `length` parameter, which is an unsigned 32-bit integer. This parameter is
taken directly from the NITF file without being checked. If the read length is
`UINT32_MAX`, i.e. `2**32 - 1`, the `NITF_MALLOC(length + 1)` call will overflow
and `malloc` will allocate a 0-byte buffer. Later, in the
`nitf_TREUtils_readField` function, the 0-byte buffer is used in a call to
`memset` with the original length. Then, the NITF TRE contents are copied to
the buffer via `nitf_IOInterface_read`. As the `memset` call uses the original
length, it will crash once the code writes beyond the end of the heap.

4. The [`readBandInfo`
function](https://github.com/mdaus/nitro/blob/5c8c30b1c95/modules/c/nitf/source/NitfReader.c#L1412)
in `modules/c/nitf/source/Reader.c` (since renamed to NitfReader.c) does not
properly check for overflows in the calculation of the allocation for the band
information. This function reads the number of lookup tables and bands from the
NITF file, multiplies them, and allocates the result. However, if the two
unsigned 32-bit integers multiply to a value larger than `UINT32_MAX`, i.e.
`2**32 - 1`, then the value will overflow and the function will allocate an
incorrectly sized buffer. This buffer is then passed to the `readField`
function, which initializes it via `memset`. Then, the NITF band info is
copied to the buffer via `nitf_IOInterface_read`. As the `memset` call uses the
original length, it will crash once the code writes beyond the end of the heap.

5. The [`readRESubheader`
function](https://github.com/mdaus/nitro/blob/5c8c30b1c95/modules/c/nitf/source/NitfReader.c#L970)
in `modules/c/nitf/Reader.c` (since renamed to NitfReader.c) does not allocate
the `subhdr` variable's `subheaderFields` field before trying to read into it
via the call to `readField`. As such, the `subheaderFields` field is `NULL` and
ends up causing a crash via a `NULL` pointer dereference in `readField` during a
`memset` call. Depending on how NITRO is deployed, this bug may allow an
attacker to cause a denial of service. However, as the attacker cannot control
the value of the `subheaderFields` field, it's unlikely they could utilize this
bug to obtain code execution.

6. The [`nitf_Record_unmergeTREs`
function](https://github.com/mdaus/nitro/blob/5c8c30b1c95/modules/c/nitf/source/Record.c#L2182)
iterates over each of the scanned NITF's labels, unmerging the extension
sections. However, the `while` loop used to iterate over the labels does not
increment the list pointer (typically done via a call to
`nitf_ListIterator_increment`). Thus, this loop will never terminate and NITRO
will hang forever. Depending on how NITRO is deployed, an attacker may able to
use this bug to cause a denial of service.

7. The [`defaultRead`
function](https://github.com/mdaus/nitro/blob/5c8c30b1c95/modules/c/nitf/source/DefaultTRE.c#L88)
in `modules/c/nitf/source/DefaultTRE.c` contains a memory leak. At the beginning
of the function, a buffer with a user controlled length is allocated to the
`data` variable. However, if an error occurs, execution will jump
to the `CATCH_ERROR` label, which does not free that buffer. In deployments
where NITRO is used in a long-lived process that scans multiple NITF
files, this bug may allow an attacker to cause a denial of service.

*/

static std::string testName;
const std::string output_file = "test_writer_3++.nitf";

namespace fs = std::filesystem;

static std::string argv0;

static bool is_vs_gtest()
{
    return argv0.empty(); // no argv[0] in VS w/GTest
}

static fs::path findInputFile(const std::string& name)
{
    const auto inputFile = fs::path("modules") / "c++" / "nitf" / "unittests" / name;

    if (is_vs_gtest()) // running Google Test in Visual Studio
    {
        const auto root= fs::current_path().parent_path().parent_path();
        return root / inputFile;
    }

    const auto exe = fs::absolute(argv0);
    fs::path root = exe.parent_path();
    do
    {
        auto retval = root / inputFile;
        if (fs::exists(retval))
        {
            return retval;
        }
        root = root.parent_path();
    } while (!root.empty());

    return inputFile;
}

TEST_CASE(test_nitf_Record_unmergeTREs_crash)
{
    ::testName = testName;
    const auto input_file = findInputFile("bug2_crash.ntf").string();

    nitf_Error error;
    nitf_IOHandle io = nitf_IOHandle_create(input_file.c_str(), NITF_ACCESS_READONLY,
        NITF_OPEN_EXISTING, &error);
    if (NITF_INVALID_HANDLE(io))
    {
        TEST_ASSERT_FALSE(true);
    }

    /*  We need to make a reader so we can parse the NITF */
    nitf_Reader* reader = nitf_Reader_construct(&error);
    TEST_ASSERT_NOT_EQ(nullptr, reader);

    /*  This parses all header data within the NITF  */
    nitf_Record* record = nitf_Reader_read(reader, io, &error);
    TEST_ASSERT_NOT_EQ(nullptr, record);

    /* Open the output IO Handle */
    nitf_IOHandle output = nitf_IOHandle_create("bug2_crash_out.ntf", NITF_ACCESS_WRITEONLY, NITF_CREATE, &error);
    if (NITF_INVALID_HANDLE(output))
    {
        TEST_ASSERT_FALSE(true);
    }

    nitf_Writer* writer = nitf_Writer_construct(&error);
    TEST_ASSERT_NOT_EQ(nullptr, writer);
    (void)nitf_Writer_prepare(writer, record, output, &error);

    nitf_IOHandle_close(io);
    nitf_Record_destruct(&record);
    nitf_Reader_destruct(&reader);

    TEST_ASSERT_TRUE(true);
}

TEST_CASE(test_nitf_Record_unmergeTREs_hangs)
{
    ::testName = testName;
    const auto input_file = findInputFile("bug6_hangs.ntf").string();

    nitf_Error error;
    nitf_IOHandle io = nitf_IOHandle_create(input_file.c_str(), NITF_ACCESS_READONLY,
        NITF_OPEN_EXISTING, &error);
    if (NITF_INVALID_HANDLE(io))
    {
        TEST_ASSERT_FALSE(true);
    }

    /*  We need to make a reader so we can parse the NITF */
    nitf_Reader* reader = nitf_Reader_construct(&error);
    TEST_ASSERT_NOT_EQ(nullptr, reader);

    /*  This parses all header data within the NITF  */
    nitf_Record* record = nitf_Reader_read(reader, io, &error);
    TEST_ASSERT_NOT_EQ(nullptr, record);

    /* Open the output IO Handle */
    nitf_IOHandle output = nitf_IOHandle_create("bug6_hangs_out.ntf", NITF_ACCESS_WRITEONLY, NITF_CREATE, &error);
    if (NITF_INVALID_HANDLE(output))
    {
        TEST_ASSERT_FALSE(true);
    }

    nitf_Writer* writer = nitf_Writer_construct(&error);
    TEST_ASSERT_NOT_EQ(nullptr, writer);
    (void)nitf_Writer_prepare(writer, record, output, &error);

    nitf_IOHandle_close(io);
    nitf_Record_destruct(&record);
    nitf_Reader_destruct(&reader);

    TEST_ASSERT_TRUE(true);
}

TEST_CASE(test_defaultRead_crash)
{
    ::testName = testName;
    const auto input_file = findInputFile("bug3_crash.ntf").string();

    nitf_Error error;
    nitf_IOHandle io = nitf_IOHandle_create(input_file.c_str(), NITF_ACCESS_READONLY,
        NITF_OPEN_EXISTING, &error);
    if (NITF_INVALID_HANDLE(io))
    {
        TEST_ASSERT_FALSE(true);
    }

    /*  We need to make a reader so we can parse the NITF */
    nitf_Reader* reader = nitf_Reader_construct(&error);
    TEST_ASSERT_NOT_EQ(nullptr, reader);

    /*  This parses all header data within the NITF  */
    (void)nitf_Reader_read(reader, io, &error);
    TEST_ASSERT_TRUE(true);
}


TEST_CASE(test_readBandInfo_crash)
{
    ::testName = testName;
    const auto input_file = findInputFile("bug4_crash.ntf").string();

    nitf_Error error;
    nitf_IOHandle io = nitf_IOHandle_create(input_file.c_str(), NITF_ACCESS_READONLY,
        NITF_OPEN_EXISTING, &error);
    if (NITF_INVALID_HANDLE(io))
    {
        TEST_ASSERT_FALSE(true);
    }

    /*  We need to make a reader so we can parse the NITF */
    nitf_Reader* reader = nitf_Reader_construct(&error);
    TEST_ASSERT_NOT_EQ(nullptr, reader);

    /*  This parses all header data within the NITF  */
    (void) nitf_Reader_read(reader, io, &error);
    TEST_ASSERT_TRUE(true);
}

TEST_CASE(test_readRESubheader_crash)
{
    ::testName = testName;
    const auto input_file = findInputFile("bug5_crash.ntf").string();

    nitf_Error error;
    nitf_IOHandle io = nitf_IOHandle_create(input_file.c_str(), NITF_ACCESS_READONLY,
        NITF_OPEN_EXISTING, &error);
    if (NITF_INVALID_HANDLE(io))
    {
        TEST_ASSERT_FALSE(true);
    }

    /*  We need to make a reader so we can parse the NITF */
    nitf_Reader* reader = nitf_Reader_construct(&error);
    TEST_ASSERT_NOT_EQ(nullptr, reader);

    /*  This parses all header data within the NITF  */
    (void)nitf_Reader_read(reader, io, &error);
    TEST_ASSERT_TRUE(true);
}

TEST_MAIN(
    (void)argc;
argv0 = argv[0];

TEST_CHECK(test_nitf_Record_unmergeTREs_crash); // 2
TEST_CHECK(test_defaultRead_crash); // 3
TEST_CHECK(test_readBandInfo_crash); // 4
TEST_CHECK(test_readRESubheader_crash); // 5
TEST_CHECK(test_nitf_Record_unmergeTREs_hangs); // 6
)