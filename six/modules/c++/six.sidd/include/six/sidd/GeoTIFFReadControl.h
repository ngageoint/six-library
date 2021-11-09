/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_SIDD_GEOTIFF_READ_CONTROL_H__
#define __SIX_SIDD_GEOTIFF_READ_CONTROL_H__

#include "six/ReadControl.h"
#include "six/ReadControlFactory.h"
#include "six/Adapters.h"
#include <import/tiff.h>

namespace six
{
namespace sidd
{

struct GeoTIFFReadControl : public ReadControl
{
    GeoTIFFReadControl() = default;

    GeoTIFFReadControl(const GeoTIFFReadControl&) = delete;
    GeoTIFFReadControl operator=(const GeoTIFFReadControl&) = delete;

    virtual DataType getDataType(const std::string& fromFile) const;

    using ReadControl::load;

    /*!
     *  Fulfills our obligations to the parent class, using the IOInterface
     *  method provided with the same name
     *  \param fromFile    Input filepath
     *  \param schemaPaths Directories or files of schema locations
     */
    virtual void load(const std::string& fromFile,
                      const std::vector<std::string>& schemaPaths);

    using ReadControl::interleaved;
    virtual UByte* interleaved(Region& region, size_t imageNumber) override;
    virtual void interleaved(Region& region, size_t imageNumber, std::byte* &result) override;

    virtual std::string getFileType() const
    {
        return "TIFF";
    }

protected:

    tiff::FileReader mReader;

};

struct GeoTIFFReadControlCreator final : public ReadControlCreator
{
    six::ReadControl* newReadControl() const override;
    void newReadControl(std::unique_ptr<six::ReadControl>& result) const override;

    bool supports(const std::string& filename) const override;

};

}
}

#endif

