/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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

#ifndef __SIX_SICD_TEST_UTILITIES_H__
#define __SIX_SICD_TEST_UTILITIES_H__

#include <iostream>
#include <vector>

#include <sys/OS.h>
#include <io/ReadUtils.h>
#include <six/sicd/Utilities.h>

// Template specialization to get appropriate pixel type
template <typename DataTypeT>
struct GetPixelType
{
};

template <>
struct GetPixelType<float>
{
    static six::PixelType getPixelType()
    {
        return six::PixelType::RE32F_IM32F;
    }
};

template <>
struct GetPixelType<int16_t>
{
    static six::PixelType getPixelType()
    {
        return six::PixelType::RE16I_IM16I;
    }
};

// Create dummy SICD data
template <typename DataTypeT>
std::unique_ptr<six::sicd::ComplexData>
createData(const types::RowCol<size_t>& dims)
{
    std::unique_ptr<six::sicd::ComplexData> data =
            six::sicd::Utilities::createFakeComplexData();
    setExtent(*data, dims);
    data->setPixelType(GetPixelType<DataTypeT>::getPixelType());
    return data;
}

// Note that this will work because SIX is forcing the NITF date/time to match
// what's in the SICD XML and we're writing the same SICD XML in all our files
class CompareFiles
{
public:
    CompareFiles(const std::string& lhsPathname)
    {
        io::readFileContents(lhsPathname, mLHS);
    }

    bool operator()(const std::string& prefix,
                    const std::string& rhsPathname) const
    {
        io::readFileContents(rhsPathname, mRHS);

        if (mLHS == mRHS)
        {
            std::cout << prefix << " matches" << std::endl;
            return true;
        }
        else if (mLHS.size() != mRHS.size())
        {
            std::cerr << prefix << " DOES NOT MATCH: file sizes are "
                      << mLHS.size() << " vs. " << mRHS.size() << " bytes"
                      << std::endl;
        }
        else
        {
            size_t ii;
            for (ii = 0; ii < mLHS.size(); ++ii)
            {
                if (mLHS[ii] != mRHS[ii])
                {
                    break;
                }
            }

            std::cerr << prefix << " DOES NOT MATCH at byte " << ii
                      << std::endl;
        }

        return false;
    }

private:
    std::vector<sys::byte> mLHS;
    mutable std::vector<sys::byte> mRHS;
};

// Makes sure a file gets removed
// Both makes sure we start with a clean slate each time and that there are no
// leftover files if an exception occurs
class EnsureFileCleanup
{
public:
    EnsureFileCleanup(const std::string& pathname) :
        mPathname(pathname)
    {
        removeIfExists();
    }

    ~EnsureFileCleanup()
    {
        try
        {
            removeIfExists();
        }
        catch (...)
        {
        }
    }

private:
    void removeIfExists()
    {
        sys::OS os;
        if (os.exists(mPathname))
        {
            os.remove(mPathname);
        }
    }

private:
    const std::string mPathname;
};

#endif
