#include "pch.h"
#include "CppUnitTest.h"

#include <import/io.h>
#include <io/TempFile.h>

namespace io
{
TEST_CLASS(test_stream_splitter){ public:
#include "io/unittests/test_stream_splitter.cpp"
};

TEST_CLASS(test_streams){ public:
#include "io/unittests/test_streams.cpp"
};

TEST_CLASS(test_tempfile){ public:
#include "io/unittests/test_tempfile.cpp"
};

}