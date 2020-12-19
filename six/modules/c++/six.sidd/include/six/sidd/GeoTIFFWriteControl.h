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
#pragma once

#if !defined(SIX_TIFF_DISABLED)


#include "six/Types.h"
#include "six/Container.h"
#include "six/WriteControl.h"
#include "six/XMLControlFactory.h"
#include "six/sidd/DerivedData.h"
#include <import/tiff.h>

namespace six
{
namespace sidd
{
/*!
 *  \class GeoTIFFWriteControl
 *  \brief Write a SIDD GeoTIFF
 *
 *  This class uses the tiff-c++ library to write out a GeoTIFF that
 *  can be up to 4GB.  If the imagery exceeds the limit, this instance
 *  of WriteControl will throw an exception.
 *
 *  Image is currently stripped, and contains the required TIFF, GeoTIFF and
 *  private SICD/SIDD keys described in the File Format Description document.
 *
 *  Containers must represent derived products!
 *
 *  \todo Support tiles using Options?
 */
class GeoTIFFWriteControl : public WriteControl
{

    std::vector<Data*> mComplexData;
    std::vector<Data*> mDerivedData;
public:
    GeoTIFFWriteControl();


    /*!
     *  Init the GeoTIFF writer.  Throws if we are a SICD container
     */
    virtual void initialize(std::shared_ptr<Container> container) override;

    using WriteControl::save;

    /*
     *  \func  save
     *  \brief writes the product to disk
     *  \param imageData   List of image segments
     *  \param outputFile  Output path to write
     *  \param schemaPaths Directories or files of schema locations
     */
    void save(const SourceList& imageData, const std::string& outputFile,
              const std::vector<std::string>& schemaPaths) override;

    /*
     *  \func  save
     *  \brief writes the product to disk
     *  \param imageData   List of image segments
     *  \param outputFile  Output path to write
     *  \param schemaPaths Directories or files of schema locations
     */
    void save(const BufferList& sources, const std::string& outputFile,
              const std::vector<std::string>& schemaPaths) override;

    /*!
     *  We are a GeoTIFF handler
     */
    std::string getFileType() const override { return "GeoTIFF"; }

private:
    static
    void addCharArray(tiff::IFD* ifd,
                      const std::string &tag,
                      const char* cstr,
                      int tiffType = tiff::Const::Type::ASCII);

    static
    void addStringArray(tiff::IFD* ifd,
                        const std::string &tag,
                        const std::string &str,
                        int tiffType = tiff::Const::Type::ASCII);

    void setupIFD(const DerivedData* data,
                  tiff::IFD* ifd,
                  const std::string& toFilePrefix,
                  const std::vector<std::string>& schemaPaths);

    void addGeoTIFFKeys(const GeographicProjection& projection,
                        size_t numRows,
                        size_t numCols,
                        tiff::IFD* ifd,
                        const std::string& tfwPathname);
};

}
}
#endif //if SIX_TIFF_ENABLED


