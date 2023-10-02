/* =========================================================================
 * This file is part of hdf5.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * hdf5.lite-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include "import/std.h"
#include <std/filesystem>

#include <TestCase.h>

#include "sys/FileFinder.h"
#include "types/RowCol.h"
#include "mem/ComplexView.h"

#include "hdf5/lite/highfive.h"
#include "hdf5/lite/SpanRC.h"

static std::filesystem::path find_unittest_file(const std::filesystem::path& name)
{
    static const auto unittests = std::filesystem::path("modules") / "c++" / "hdf5.lite" / "unittests";
    return sys::test::findGITModuleFile("coda-oss", unittests, name);
}

TEST_CASE(test_highfive_load)
{
    static const auto path = find_unittest_file("example.h5");

    const H5Easy::File file(path.string());

    {
        const auto lat = H5Easy::load<std::vector<double>>(file, "/g4/lat");
        TEST_ASSERT_EQ(lat.size(), 19);
        TEST_ASSERT_ALMOST_EQ(lat[0], -90.0);
        TEST_ASSERT_ALMOST_EQ(lat[0], -lat[18]);
    }
    {
        std::vector<double> lat;
        const auto rc = hdf5::lite::loadDataSet(file, "/g4/lat", lat);
        TEST_ASSERT_EQ(lat.size(), 19);
        TEST_ASSERT_EQ(lat.size(), rc.area());
        TEST_ASSERT_EQ(rc.dims().row, 19);
        TEST_ASSERT_EQ(rc.dims().col, 1);
        TEST_ASSERT_ALMOST_EQ(lat[0], -90.0);
        TEST_ASSERT_ALMOST_EQ(lat[0], -lat[18]);
    }
}

TEST_CASE(test_highfive_FileException)
{
    static const std::filesystem::path path = "does not exist . h5";
    HighFive::SilenceHDF5 silencer; // no need for diagnostics, we're expecting a failure
    TEST_SPECIFIC_EXCEPTION(H5Easy::File(path.string()), HighFive::FileException);
}

TEST_CASE(test_highfive_nested)
{
    /*
    Group '/' 
    Group '/1' 
        Group '/1/bar' 
            Group '/1/bar/cat' 
                Dataset 'i' 
                    Size:  10x1
                    MaxSize:  10x1
                    Datatype:   H5T_IEEE_F64LE (double)
                    ChunkSize:  []
                    Filters:  none
                    FillValue:  0.000000
    */
    static const auto path = find_unittest_file("123_barfoo_catdog_cx.h5");

    const H5Easy::File file(path.string());
    const auto i = hdf5::lite::vv_load<double>(file, "/1/bar/cat/i");
    TEST_ASSERT_EQ(i.size(), 1);
    TEST_ASSERT_EQ(i[0].size(), 10);

    const auto r = hdf5::lite::vv_load<double>(file, "/1/bar/dog/r");
    TEST_ASSERT_EQ(r.size(), 1);
    TEST_ASSERT_EQ(r[0].size(), 10);
    
    TEST_ASSERT_EQ(i.size(), r.size());
    TEST_ASSERT_EQ(i[0].size(), r[0].size());
}

template<typename T>
static auto read_complex(const HighFive::DataSet& r, const HighFive::DataSet& i) 
{
    std::vector<T> r_result;
    hdf5::lite::readDataSet(r, r_result);
    std::vector<T> i_result;
    hdf5::lite::readDataSet(i, i_result);
    return std::make_pair(r, i);
}
template<typename T>
static auto read_complex(const HighFive::Group& group)
{
    const auto i = group.getDataSet("i");   
    const auto r = group.getDataSet("r");
    return read_complex<T>(r, i);
}
template<typename T>
static auto load_complex(const H5Easy::File& file,
                         const std::string& r_path, const std::string& i_path)
{
    const auto r = hdf5::lite::v_load<T>(file, r_path);
    const auto i = hdf5::lite::v_load<T>(file, i_path);
    return std::make_pair(r, i);
}

TEST_CASE(test_highfive_nested_small)
{
    // top group: Data
    // outer groups: 1, 2, 3, 4, 5
    // sub groups: bar, foo
    // sub-sub groups: cat, dog
    // sub-sub-sub groups: a, b, c, d
    // data: i (float array), r (float array)
    static const auto path = find_unittest_file("nested_complex_float32_data_small.h5");

    const H5Easy::File file(path.string());

    const auto i = hdf5::lite::v_load<float>(file, "/Data/1/bar/cat/a/i");
    TEST_ASSERT_EQ(i.size(), 10);
    auto actual = std::accumulate(i.begin(), i.end(), 0.0f);
    TEST_ASSERT_EQ(actual, 0.0f);

    const auto r = hdf5::lite::v_load<float>(file, "/Data/5/foo/dog/d/r");
    TEST_ASSERT_EQ(r.size(), 10);
    actual = std::accumulate(r.begin(), r.end(), 0.0f);
    TEST_ASSERT_EQ(actual, 10.0f);

    const auto a = load_complex<float>(file, "/Data/1/bar/cat/a/r",  "/Data/1/bar/cat/a/i");
    const auto cx_view = mem::make_ComplexParallelView(a.first, a.second);
    const auto cx_actual = std::accumulate(cx_view.begin(), cx_view.end(), std::complex<float>(0.0));
    TEST_ASSERT_EQ(cx_actual.real(), 10.0f);
    TEST_ASSERT_EQ(cx_actual.imag(), 0.0f);
}

TEST_CASE(test_highfive_nested_small_wrongType)
{
    static const auto path = find_unittest_file("nested_complex_float32_data_small.h5");

    const H5Easy::File file(path.string());
    HighFive::SilenceHDF5 silencer; // no need for diagnostics, we're expecting a failure
    TEST_SPECIFIC_EXCEPTION(
        H5Easy::load<std::vector<std::complex<float>>>(file, "/Data/1/bar/cat/a/i"),
        HighFive::DataSetException);
}


//*******************************************************************************

TEST_CASE(test_highfive_info)
{
    static const auto path = find_unittest_file("example.h5");
    const H5Easy::File file(path.string());

    // https://www.mathworks.com/help/matlab/ref/h5info.html
    /*
    info = struct with fields:
          Filename: '/mathworks/devel/bat/Bdoc22b/build/matlab/toolbox/matlab/demos/example.h5'
              Name: '/'
            Groups: [4x1 struct]
          Datasets: []
         Datatypes: []
             Links: []
        Attributes: [2x1 struct]
    */
    
    TEST_ASSERT_EQ(path.string(), file.getName());  
    TEST_ASSERT_EQ("/",  file.getPath());
    TEST_ASSERT_EQ(file.getNumberObjects(), 4); // 4 groups
    const auto objectNames = file.listObjectNames();
    TEST_ASSERT_EQ(objectNames.size(), 4);  // 4 groups
    for (auto&& name : objectNames)
    {
        const auto type = file.getObjectType(name);
        TEST_ASSERT(type == HighFive::ObjectType::Group);
        
        const auto group = file.getGroup(name);
        TEST_ASSERT_EQ(group.getPath(), "/" + name);   
    }
    TEST_ASSERT_EQ(file.getNumberAttributes(), 2);
}

TEST_CASE(test_highfive_groupinfo)
{
    static const auto path = find_unittest_file("example.h5");
    const H5Easy::File file(path.string());

    // https://www.mathworks.com/help/matlab/ref/h5info.html
    const auto g4 = file.getGroup("g4");
    /*
    info = struct with fields:
      Filename: '/mathworks/devel/bat/Bdoc22b/build/matlab/toolbox/matlab/demos/example.h5'
          Name: '/g4'
        Groups: []
      Datasets: [4x1 struct]
     Datatypes: []
         Links: []
    Attributes: []
    */    
    TEST_ASSERT_EQ(path.string(), g4.getFile().getName());
    const std::string groupPath("/g4");
    TEST_ASSERT_EQ(groupPath, g4.getPath());
    TEST_ASSERT_EQ(g4.getNumberObjects(), 4);  // 4 dataSets
    const auto objectNames = g4.listObjectNames();
    TEST_ASSERT_EQ(objectNames.size(), 4);  // 4 dataSets
    for (auto&& name : objectNames)
    {
        const auto type = g4.getObjectType(name);
        TEST_ASSERT(type == HighFive::ObjectType::Dataset);

        const auto dataset = g4.getDataSet(name);
        TEST_ASSERT_EQ(dataset.getPath(), groupPath + "/" +  name);
    }
}

TEST_CASE(test_highfive_datasetinfo)
{
    static const auto path = find_unittest_file("example.h5");
    const H5Easy::File file(path.string());

    // https://www.mathworks.com/help/matlab/ref/h5info.html
    const auto time = file.getDataSet("/g4/time");
    /*
    info = struct with fields:
      Filename: '/mathworks/devel/bat/Bdoc22b/build/matlab/toolbox/matlab/demos/example.h5'
          Name: 'time'
      Datatype: [1x1 struct]
     Dataspace: [1x1 struct]
     ChunkSize: 10
     FillValue: 0
       Filters: []
    Attributes: [2x1 struct]    
    */
    TEST_ASSERT_EQ(path.string(), time.getFile().getName());
    const std::string dsPath("/g4/time");
    TEST_ASSERT_EQ(dsPath, time.getPath());

    const auto dataType = time.getDataType();
    TEST_ASSERT(dataType.getClass() == HighFive::DataTypeClass::Float);

    auto dims = time.getDimensions();
    TEST_ASSERT_EQ(dims.size(), 1);
    TEST_ASSERT_EQ(10, dims[0]); // ChunkSize ???

    const auto dataSpace = time.getSpace();
    dims = dataSpace.getDimensions();
    TEST_ASSERT_EQ(dims.size(), 1);
    TEST_ASSERT_EQ(10, dims[0]);  // ChunkSize ???

    TEST_ASSERT_EQ(time.listAttributeNames().size(), 2);
}

static void read_complex(const std::string& testName, const HighFive::DataSet& r, const HighFive::DataSet& i) 
{   
    TEST_ASSERT(r.getDataType().getClass() == HighFive::DataTypeClass::Float);
    TEST_ASSERT_EQ(r.getElementCount(), 10);
    TEST_ASSERT(i.getDataType().getClass() == HighFive::DataTypeClass::Float);
    TEST_ASSERT_EQ(i.getElementCount(), 10);
    std::ignore = read_complex<double>(r, i);
}
static void read_complex(const std::string& testName, const HighFive::Group& group)
{
    read_complex(testName, group.getDataSet("r"), group.getDataSet("i"));
    std::ignore = read_complex<double>(group);
}
TEST_CASE(test_highfive_info_nested)
{
    /*
    Group '/' 
    Group '/1' 
        Group '/1/bar' 
            Group '/1/bar/cat' 
                Dataset 'i' 
                    Size:  10x1
                    MaxSize:  10x1
                    Datatype:   H5T_IEEE_F64LE (double)
                    ChunkSize:  []
                    Filters:  none
                    FillValue:  0.000000
    */

    static const auto path = find_unittest_file("123_barfoo_catdog_cx.h5");
    const H5Easy::File file(path.string());

    TEST_ASSERT_EQ(path.string(), file.getName());
    TEST_ASSERT_EQ("/", file.getPath());

    TEST_ASSERT_EQ(file.getNumberObjects(), 3);  // 3 groups
    auto objectNames = file.listObjectNames();
    const std::vector<std::string> expectedOuterGroupNames{"1", "2", "3"};
    TEST_ASSERT_EQ(objectNames.size(), expectedOuterGroupNames.size());
    for (auto&& outer : expectedOuterGroupNames)
    {
        const auto groupPath = "/" + outer;
        const auto group = file.getGroup(groupPath);
        TEST_ASSERT_EQ(group.getPath(), groupPath);

        TEST_ASSERT_EQ(group.getNumberObjects(), 2);  // 2 groups
        objectNames = group.listObjectNames();
        const std::vector<std::string> expectedSubGroupNames{"bar", "foo"};
        TEST_ASSERT_EQ(objectNames.size(), expectedSubGroupNames.size());
        for (auto&& subGroupName : expectedSubGroupNames)
        {
            const auto subGroup = group.getGroup(subGroupName);
            const auto subGroupPath = groupPath + "/" + subGroupName;
            TEST_ASSERT_EQ(subGroup.getPath(), subGroupPath);


            TEST_ASSERT_EQ(subGroup.getNumberObjects(), 2);  // 2 groups
            objectNames = subGroup.listObjectNames();
            const std::vector<std::string> expectedSubSubGroupNames{"cat", "dog"};
            TEST_ASSERT_EQ(objectNames.size(), expectedSubSubGroupNames.size());
            for (auto&& subSubGroupName : expectedSubSubGroupNames)
            {
                const auto subSubGroup = subGroup.getGroup(subSubGroupName);
                const auto subSubGroupPath = subGroupPath + "/" + subSubGroupName;
                TEST_ASSERT_EQ(subSubGroup.getPath(), subSubGroupPath);

                read_complex(testName, subSubGroup);
            }
        }
    }
}

//*******************************************************************************

template<typename T>
static auto make_data(const types::RowCol<size_t>& dims)
{
    std::vector<std::vector<T>> retval(dims.row);
    int d = 0;
    for (auto&& r : retval)
    {
        r.resize(dims.col);
        for (auto&& v : r)
        {
            v = static_cast<T>(d++);
        }
    }
    return retval;
}

TEST_CASE(test_highfive_dump)
{  
    static const std::string dataset_name("/DS1");
    using dataset_t = float;
    static const auto path_ = find_unittest_file("example.h5");
    const auto path = path_.parent_path() / "TEST_highfive_create_TMP.h5";

    {
        const std::vector<dataset_t> data{1, 2, 3, 4, 5};
        H5Easy::File file(path.string(), H5Easy::File::Overwrite);
        //std::ignore = H5Easy::dump(file, dataset_name, data);
        TEST_SUCCESS;
    }

    const types::RowCol<size_t> dims{10, 20};
    const auto data = make_data<dataset_t>(dims);
    {
        H5Easy::File file(path.string(), H5Easy::File::Overwrite);
        //std::ignore = H5Easy::dump(file, dataset_name, data);
        TEST_SUCCESS;
    }
    
    // Be sure we can read the file just written
    const H5Easy::File file(path.string(), H5Easy::File::ReadOnly);
    //const auto DS1 = hdf5::lite::vv_load<dataset_t>(file, dataset_name);
    //TEST_ASSERT_EQ(DS1.size(), dims.row);
    //TEST_ASSERT_EQ(DS1[0].size(), dims.col);
    //for (size_t r = 0; r < DS1.size(); r++)
    //{
    //    for (size_t c = 0; c < DS1[r].size(); c++)
    //    {
    //        const auto expected = data[r][c];
    //        const auto actual = DS1[r][c];
    //        TEST_ASSERT_EQ(actual, expected);
    //    }
    //}
}

TEST_CASE(test_highfive_write)
{
    static const auto path_ = find_unittest_file("example.h5");
    static const auto path = path_.parent_path() / "TEST_highfive_write_TMP.h5";

    const types::RowCol<size_t> dims{10, 20};
    std::vector<double> data_(dims.area());
    const hdf5::lite::SpanRC<double> data(data_.data(), dims);
    double d = 0.0;
    for (size_t r = 0; r<dims.row; r++)
    {
        for (size_t c = 0; c < dims.col; c++)
        {
            data(r, c) = d++;
        }    
    }    
    {
        H5Easy::File file(path.string(), H5Easy::File::Overwrite);
        const auto ds = hdf5::lite::writeDataSet(file, "DS1", data);
        const auto dimensions = ds.getDimensions();
        TEST_ASSERT_EQ(dimensions.size(), 2);
        TEST_ASSERT_EQ(dims.row, dimensions[0]);
        TEST_ASSERT_EQ(dims.col, dimensions[1]);
        TEST_ASSERT_EQ(ds.getElementCount(), data.area());
    }
    // Be sure we can read the file just written
    {
        const H5Easy::File file(path.string());

        const auto DS1 = hdf5::lite::vv_load<double>(file, "/DS1");
        TEST_ASSERT_EQ(DS1.size(), dims.row);
        TEST_ASSERT_EQ(DS1[0].size(), dims.col);

        for (size_t r = 0; r < DS1.size(); r++)
        {
            for (size_t c = 0; c < DS1[r].size(); c++)
            {
                const auto expected = data(r, c);
                const auto actual = DS1[r][c];
                TEST_ASSERT_EQ(actual, expected);
            }
        }
    }
    {
        H5Easy::File file(path.string());

        std::vector<double> result;
        const auto rc = hdf5::lite::loadDataSet(file, "/DS1", result);
        TEST_ASSERT(rc.dims() == dims);
        TEST_ASSERT_EQ(dims.area(), result.size());
        for (size_t i = 0; i < result.size(); i++)
        {
            const auto expected = static_cast<double>(i);
            TEST_ASSERT_EQ(result[i], expected);
        }
    }
}

TEST_CASE(test_highfive_getDataType)
{
    static const auto path = find_unittest_file("example.h5");
    const H5Easy::File file(path.string(), H5Easy::File::ReadOnly);
    
    const auto time = file.getDataSet("/g4/time");
    TEST_ASSERT(time.getType() == HighFive::ObjectType::Dataset);
    TEST_ASSERT_EQ(time.getElementCount(), 10);
    const auto dims = time.getDimensions();
    TEST_ASSERT_EQ(dims.size(), 1);
    TEST_ASSERT_EQ(dims[0], 10);

    const auto dataType = time.getDataType();
    TEST_ASSERT(dataType.getClass() == HighFive::DataTypeClass::Float);
    TEST_ASSERT_EQ(dataType.string(), "Float64");
    TEST_ASSERT_EQ(dataType.getSize(), sizeof(double));
    const auto doubleDataType = HighFive::create_and_check_datatype<double>();
    TEST_ASSERT(doubleDataType == dataType);

    TEST_ASSERT_FALSE(dataType.isVariableStr());
    TEST_ASSERT_FALSE(dataType.isFixedLenStr());
    TEST_ASSERT_FALSE(dataType.isReference());
}

template <typename THighFive>
static auto getAttribute(const std::string& testName,
                         const THighFive& obj, const std::string& name,
                         HighFive::DataTypeClass typeClass, const std::string& typeName)
{
    auto attribute = obj.getAttribute(name);
    TEST_ASSERT_EQ(attribute.getName(), name);
    TEST_ASSERT(attribute.getType() == HighFive::ObjectType::Attribute);
    TEST_ASSERT(attribute.getDataType().getClass() == typeClass);
    TEST_ASSERT_EQ(attribute.getDataType().string(), typeName);
    return attribute;
}
TEST_CASE(test_highfive_getAttribute)
{
    static const auto path = find_unittest_file("example.h5");
    const H5Easy::File file(path.string(), H5Easy::File::ReadOnly);

    {
        const auto attribute = getAttribute(testName, file, "attr1", HighFive::DataTypeClass::Integer, "Integer8");
        const auto memSpace = attribute.getMemSpace();
        const auto elements = memSpace.getElementCount();
        TEST_ASSERT_EQ(elements, 10);
        std::vector<int8_t> v;
        attribute.read(v);
        TEST_ASSERT_EQ(v.size(), elements);
    }
    {
        const auto attribute = getAttribute(testName, file, "attr2", HighFive::DataTypeClass::Integer, "Integer32");
        const auto memSpace = attribute.getMemSpace();
        const auto elements = memSpace.getElementCount();
        TEST_ASSERT_EQ(elements, 4);
        std::vector<std::vector<int32_t>> v;
        attribute.read(v);
        TEST_ASSERT_EQ(v[0][0], 0);
        TEST_ASSERT_EQ(v[0][1], 1);
        TEST_ASSERT_EQ(v[1][0], 2);
        TEST_ASSERT_EQ(v[1][1], 3);        
    }

    const auto time = file.getDataSet("/g4/time");
    {
        const auto attribute = getAttribute(testName, time, "NAME", HighFive::DataTypeClass::String, "String40");
        // throw DataSetException("Can't output std::string as fixed-length. Use raw arrays or FixedLenStringArray");
        // std::string value;
        // attribute.read(value);
        // HighFive::FixedLenStringArray<16> arr;
        // attribute.read(arr);
        const auto value = hdf5::lite::read<std::string>(attribute);
        TEST_ASSERT_EQ(value, "time");
    }
    {
        const auto attribute = getAttribute(testName, time, "CLASS", HighFive::DataTypeClass::String, "String128");
        // attribute.read(value);
        const auto value = hdf5::lite::read<std::string>(attribute);
        TEST_ASSERT_EQ(value, "DIMENSION_SCALE");
    }

     const auto lat = file.getDataSet("/g4/lat");
    {
        const auto attribute = getAttribute(testName, lat, "units", HighFive::DataTypeClass::String, "String104");
        //HighFive::FixedLenStringArray<104> value;
        //attribute.read(value);
        const auto value = hdf5::lite::read<std::string>(attribute);
        TEST_ASSERT_EQ(value, "degrees_north");
    }
}

TEST_MAIN(
    TEST_CHECK(test_highfive_load);
    TEST_CHECK(test_highfive_FileException);
    TEST_CHECK(test_highfive_nested);
    TEST_CHECK(test_highfive_nested_small);
    TEST_CHECK(test_highfive_nested_small_wrongType);

    TEST_CHECK(test_highfive_info);
    TEST_CHECK(test_highfive_groupinfo);
    TEST_CHECK(test_highfive_datasetinfo);
    TEST_CHECK(test_highfive_info_nested);

    TEST_CHECK(test_highfive_dump);
    TEST_CHECK(test_highfive_write);

    TEST_CHECK(test_highfive_getDataType);
    TEST_CHECK(test_highfive_getAttribute);
 )
