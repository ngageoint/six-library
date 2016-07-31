/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

#ifndef __SIX_SICD_WRITE_CONTROL_H__
#define __SIX_SICD_WRITE_CONTROL_H__

#include <vector>

#include <types/RowCol.h>
#include <six/NITFWriteControl.h>
#include <six/sicd/ComplexData.h>

namespace six
{
namespace sicd
{
class SICDWriteControl : public six::NITFWriteControl
{
public:
    SICDWriteControl(const std::string& outputFile,
                     const std::vector<std::string>& schemaPaths);

    // So the implementation for this is kind of hokey
    // TODO: If WriteControl holds onto the Container by SharedPtr,
    //       this goes away
    // You only have to call one of these two methods
    void initialize(const ComplexData& data);

    virtual void initialize(Container* container);

    using NITFWriteControl::save;

    void save(void* imageData,
              const types::RowCol<size_t>& offset,
              const types::RowCol<size_t>& dims,
              bool restoreData = true);

    void close();

private:
    void setComplexityLevelIfRequired();

private:
    // TODO: You probably want to use a buffered IO for the initial small
    //       writes of the file header but then a non-buffered IO for the image
    //       writes (otherwise you're going to do a needless memcpy() for the
    //       cases where you have partial rows).  Not sure how you could cleanly
    //       handle this since you need to keep the same file descriptor open.
    std::auto_ptr<nitf::IOInterface> mIO;
    const std::vector<std::string> mSchemaPaths;

    std::vector<nitf::Off> mImageDataStart;

    std::auto_ptr<six::Container> mOurContainer;
};
}
}

#endif
