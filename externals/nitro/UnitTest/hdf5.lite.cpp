#include "pch.h"
#include "CppUnitTest.h"

#include <import/hdf5/lite.h>
#include <hdf5/lite/HDF5Exception.h>
#include <hdf5/lite/Info.h>
#include <hdf5/lite/Read.h>
#include <hdf5/lite/Write.h>

namespace hdf5_lite
{

TEST_CLASS(test_hdf5info){ public:
#include "hdf5.lite/unittests/test_hdf5info.cpp"
};

TEST_CLASS(test_hdf5read){ public:
#include "hdf5.lite/unittests/test_hdf5read.cpp"
};

TEST_CLASS(test_hdf5write){ public:
#include "hdf5.lite/unittests/test_hdf5write.cpp"
};

}