#include "pch.h"
#include "CppUnitTest.h"

#include <mem/ComplexView.h>
#include <import/hdf5/lite.h>
#pragma comment(lib, "hdf5-c++.lib")

#include <highfive/H5DataSet.hpp>
#include <highfive/H5DataSpace.hpp>
#include <highfive/H5File.hpp>
#include <highfive/H5Group.hpp>
#include <highfive/H5Reference.hpp>
#include <highfive/H5Utility.hpp>

namespace hdf5_lite
{

TEST_CLASS(test_highfive){ public:
#include "hdf5.lite/unittests/test_highfive.cpp"
};

//TEST_CLASS(test_high_five_base){ public:
//#include "highfive/unittests/tests_high_five_base.cpp"
//};

}