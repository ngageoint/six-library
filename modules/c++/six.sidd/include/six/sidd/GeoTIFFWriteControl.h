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
#ifndef __SIX_GEO_TIFF_WRITE_CONTROL_H__
#define __SIX_GEO_TIFF_WRITE_CONTROL_H__

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
    virtual ~GeoTIFFWriteControl() {}
    

    /*!
     *  Init the GeoTIFF writer.  Throws if we are a SICD container
     */
    virtual void initialize(Container* container);
    /*!
     *  Write out a set of images from a source list
     */
    virtual void save(SourceList& sources, std::string toFile);
    /*!
     *  Write out a set of images from a memory buffer source list
     */
    virtual void save(BufferList& sources, std::string toFile);
    
    /*!
     *  We are a GeoTIFF handler
     */
    std::string getFileType() const { return "GeoTIFF"; }

private:

    void addCharArray(tiff::IFD* ifd,
                      std::string tag,
                      char* cstr, 
                      int tiffType);
    

    char* addStringArray(tiff::IFD* ifd,
                         std::string tag,
                         std::string str, 
                         int tiffType = tiff::Const::Type::ASCII);
    
/*     void addCharArray(tiff::IFD* ifd, */
/*                       int tag, */
/*                       char* cstr,  */
/*                       int tiffType); */
    
/*     char* addStringArray(tiff::IFD* ifd, */
/*                          int tag, */
/*                          std::string str,  */
/*                          int tiffType = tiff::Const::Type::ASCII); */

    std::vector<char*> setupIFD(DerivedData* data, tiff::IFD* ifd);
   
    void addGeoTIFFKeys(tiff::IFD* ifd, 
                        const std::vector<LatLon>& corners, 
                        unsigned long numRows, unsigned long numCols);

    void addTiepoint(tiff::IFDEntry* entry, double ties[6])
    {
        for (unsigned int i = 0; i < 6; ++i)
        {
            entry->addValue(
                tiff::TypeFactory::create((unsigned char*)&ties[i],
                                          tiff::Const::Type::DOUBLE));
        }
        
    }

};
    
}
}
#endif //if SIX_TIFF_ENABLED
#endif
