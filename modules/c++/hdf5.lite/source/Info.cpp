/* =========================================================================
 * This file is part of hd5.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * hd5.lite-c++ is free software; you can redistribute it and/or modify
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

#include "hdf5/lite/Info.h"

#include <tuple> // std::ignore
#include <stdexcept>

#include "H5.h"
#include "hdf5.lite.h"


// https://docs.hdfgroup.org/archive/support/HDF5/doc1.8/cpplus_RM/h5group_8cpp-example.html
static herr_t group_info(hid_t loc_id, const char *name, const H5L_info_t* /*linfo*/, void *opdata)
{
    // only interested in groups
    const auto obj = H5Gopen2(loc_id, name, H5P_DEFAULT);
    if (obj > 0)
    {
        H5Gclose(obj);

        hdf5::lite::GroupInfo info;
        info.name = name;

        auto pRetval = static_cast<std::vector<hdf5::lite::GroupInfo>*>(opdata);
        pRetval->push_back(info);
    }

    return 0;
}
static std::vector<hdf5::lite::GroupInfo> getGroups(H5::Group& group)
{
    std::vector<hdf5::lite::GroupInfo> retval;

    const auto herr = H5Literate(group.getId(), H5_INDEX_NAME, H5_ITER_INC, nullptr /*idx*/, group_info,  &retval);
    if (herr != 0)
    {
        // How can this happen?
        throw std::logic_error("H5Literate failed.");
    }

    return retval;
 }

// https://docs.hdfgroup.org/archive/support/HDF5/doc1.8/cpplus_RM/h5group_8cpp-example.html
static herr_t dataset_info(hid_t loc_id, const char *name, const H5L_info_t* /*linfo*/, void *opdata)
{
    // only interested in Datasets
    const auto obj = H5Dopen2(loc_id, name, H5P_DEFAULT);
    if (obj > 0)
    {
        H5Dclose(obj);

        hdf5::lite::DataSetInfo info;
        info.name = name;

        auto pRetval = static_cast<std::vector<hdf5::lite::DataSetInfo>*>(opdata);
        pRetval->push_back(info);
    }

    return 0;
}
static std::vector<hdf5::lite::DataSetInfo> getDataSets(H5::Group& group)
 {
    std::vector<hdf5::lite::DataSetInfo> retval;

    const auto herr = H5Literate(group.getId(), H5_INDEX_NAME, H5_ITER_INC, nullptr /*idx*/, dataset_info,  &retval);
    if (herr != 0)
    {
        // How can this happen?
        throw std::logic_error("H5Literate failed.");
    }

    return retval;
 }

// https://docs.hdfgroup.org/archive/support/HDF5/doc1.8/cpplus_RM/h5group_8cpp-example.html
static herr_t datatype_info(hid_t loc_id, const char *name, const H5L_info_t* /*linfo*/, void *opdata)
{
    // only interested in Datatypes
    const auto obj = H5Topen2(loc_id, name, H5P_DEFAULT);
    if (obj > 0)
    {
        H5Tclose(obj);

        hdf5::lite::DataTypeInfo info;
        info.name = name;

        auto pRetval = static_cast<std::vector<hdf5::lite::DataTypeInfo>*>(opdata);
        pRetval->push_back(info);
    }

    return 0;
}
static std::vector<hdf5::lite::DataTypeInfo> getDataTypes(H5::Group& group)
 {
    std::vector<hdf5::lite::DataTypeInfo> retval;

    const auto herr = H5Literate(group.getId(), H5_INDEX_NAME, H5_ITER_INC, nullptr /*idx*/, datatype_info,  &retval);
    if (herr != 0)
    {
        // How can this happen?
        throw std::logic_error("H5Literate failed.");
    }

    return retval;
 }

// https://docs.hdfgroup.org/archive/support/HDF5/doc1.8/cpplus_RM/readdata_8cpp-example.html
static hdf5::lite::GroupInfo groupInfo_(const coda_oss::filesystem::path& filename, const std::string& loc)
{
    hdf5::lite::GroupInfo retval;
    retval.filename = filename.string();
    retval.name = loc;

    /*
     * Open the specified file and the specified dataset in the file.
     */
    H5::H5File file(retval.filename, H5F_ACC_RDONLY);
    auto group = file.openGroup(retval.name);

    retval.groups = getGroups(group);
    retval.dataSets = getDataSets(group);
    retval.dataTypes = getDataTypes(group);

    return retval;
}
hdf5::lite::GroupInfo hdf5::lite::groupInfo(const coda_oss::filesystem::path& filename, const std::string& loc)
{
    return details::try_catch_H5Exceptions(groupInfo_, __FILE__, __LINE__, filename, loc);
}
hdf5::lite::FileInfo hdf5::lite::fileInfo(const coda_oss::filesystem::path& filename)
{
    hdf5::lite::FileInfo retval;
    hdf5::lite::GroupInfo& retval_ = retval;
    retval_= groupInfo(filename, "/" /*loc*/);
    return retval;
}

static hdf5::lite::Class H5T_class_to_Class(H5T_class_t type_class)
{
    switch (type_class)
    {
        case H5T_NO_CLASS: return hdf5::lite::Class::NoClass;
        case H5T_INTEGER: return hdf5::lite::Class::Integer;
        case H5T_FLOAT: return hdf5::lite::Class::Float;
        case H5T_TIME: return hdf5::lite::Class::Time;
        case H5T_STRING: return hdf5::lite::Class::String;
        case H5T_BITFIELD: return hdf5::lite::Class::Bitfield;
        case H5T_OPAQUE: return hdf5::lite::Class::Opaque;
        case H5T_COMPOUND: return hdf5::lite::Class::Compound;
        case H5T_REFERENCE: return hdf5::lite::Class::Reference;
        case H5T_ENUM: return hdf5::lite::Class::Enum;
        case H5T_VLEN: return hdf5::lite::Class::Vlen;
        case H5T_ARRAY: return hdf5::lite::Class::Array;

        case H5T_NCLASSES: /*shouldn't be used*/
            throw std::logic_error("Unexpected H5T_class_t.");    
        default:
            throw std::logic_error("Unknown H5T_class_t.");    
    }
}
static hdf5::lite::Class H5T_class_to_Class(const H5::DataSet& dataset)
{
    return H5T_class_to_Class(dataset.getTypeClass());
}

// https://docs.hdfgroup.org/archive/support/HDF5/doc1.8/cpplus_RM/readdata_8cpp-example.html
static hdf5::lite::DataSetInfo dataSetInfo_(const coda_oss::filesystem::path& filename, const std::string& loc)
{
    hdf5::lite::DataSetInfo retval;
    retval.filename = filename.string();

    /*
     * Open the specified file and the specified dataset in the file.
     */
    H5::H5File file(retval.filename, H5F_ACC_RDONLY);
    const auto dataset = file.openDataSet(loc);

    const coda_oss::filesystem::path name = dataset.getObjName(); // e.g., "/g4/time"
    retval.name = name.filename().string(); // "time"

    /*
     * Get the class of the datatype that is used by the dataset.
     */
    retval.dataType.h5Class = H5T_class_to_Class(dataset);

    return retval;
}
hdf5::lite::DataSetInfo hdf5::lite::dataSetInfo(const coda_oss::filesystem::path& filename, const std::string& loc)
{
    return details::try_catch_H5Exceptions(dataSetInfo_, __FILE__, __LINE__, filename, loc);
}
