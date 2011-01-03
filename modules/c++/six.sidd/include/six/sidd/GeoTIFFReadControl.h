/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
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

class GeoTIFFReadControl : public ReadControl
{
public:

    //!  Constructor
    GeoTIFFReadControl()
    {
    }

    //!  Destructor
    virtual ~GeoTIFFReadControl()
    {
    }

    virtual DataType getDataType(std::string fromFile);

    virtual void load(std::string fromFile,
                      XMLControlRegistry* xmlRegistry = NULL);

    virtual UByte* interleaved(Region& region, int imageNumber);

    virtual std::string getFileType() const
    {
        return "TIFF";
    }

protected:

    tiff::FileReader mReader;

};

struct GeoTIFFReadControlCreator : public ReadControlCreator
{
    six::ReadControl* newReadControl() const;

    bool supports(const std::string& filename) const;

};

}
}

#endif
