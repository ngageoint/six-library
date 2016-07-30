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

namespace six
{
namespace sicd
{
class SICDWriteControl : public six::NITFWriteControl
{
public:
    SICDWriteControl(const std::string& outputFile,
                     const std::vector<std::string>& schemaPaths);

    virtual void initialize(Container* container);

    using NITFWriteControl::save;

    void save(void* imageData,
              const types::RowCol<size_t>& offset,
              const types::RowCol<size_t>& dims,
              bool restoreData = true);

private:
    void setComplexityLevelIfRequired();

private:
    std::auto_ptr<nitf::IOInterface> mIO;
    const std::vector<std::string> mSchemaPaths;

    std::vector<nitf::Off> mImageDataStart;
};
}
}

#endif
