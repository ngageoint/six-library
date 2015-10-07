/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#ifndef __SIX_SICD_UTILITIES_H__
#define __SIX_SICD_UTILITIES_H__

#include <memory>
#include <string>
#include <vector>
#include <utility>

#include <scene/SceneGeometry.h>
#include <scene/ProjectionModel.h>
#include <six/sicd/ComplexData.h>

#include <six/NITFReadControl.h>

namespace six
{
namespace sicd
{
class Utilities
{
public:
    static scene::SceneGeometry* getSceneGeometry(const ComplexData* data);

    static scene::ProjectionModel* getProjectionModel(const ComplexData* data, 
            const scene::SceneGeometry* geom);

    /*
     * SicdContents is a helper object for readSicdContents
     *
     * This class is a simple wrapper around SICD information 
     * designed to allow safe access to the multiple variables 
     * returned by readSicdContents.
     *
     */
    struct SicdContents {
    
        SicdContents()
            : complexData(0),
              widebandData()
        {
        }

        SicdContents(const SicdContents& other)
            : complexData(0),
              widebandData(other.widebandData)
        {
            if(other.complexData.get())
            {
                complexData.reset(reinterpret_cast<ComplexData*>
                                    (other.complexData->clone()));
            }
        }

        SicdContents& operator=(const SicdContents& other)
        {
            if(other.complexData.get())
            {
                complexData.reset(reinterpret_cast<ComplexData*>
                                    (other.complexData->clone()));
                widebandData = other.widebandData;
            }
            return *this;
        }

        std::auto_ptr<ComplexData> complexData;
        std::vector<std::complex<float> > widebandData;
    };

    /*
     * Given a SICD path name and a list of schema, this function reads
     * and parses the SICD in order to provide the wideband data as well
     * as the ComplexData associated with the image..
     *
     * \param sicdPathname SICD NITF pathname
     * \param schemaPaths One or more files or directories containing SICD 
     * schemas
     *
     * \return ComplexData associated with the SICD NITF,
     *           allocated with new
     *         std::complex<float> buffer containing SICD wideband data,
     *           allocated with new[]
     *
     * \throws See six::sicd::Utilities::getComplexData and 
     *           six::sicd::Utilities::getWidebandData
     *
     */
    static SicdContents readSicd(const std::string& sicdPathname,
                                 const std::vector<std::string>& schemaPaths);

    /*
     * Given a reference to a loaded NITFReadControl, this function
     * parses the SICD's XML and returns a new ComplexData object.
     *
     * \param reader A NITFReadControl loaded with the desired SICD
     *
     * \return ComplexData new ComplexData associated with the SICD NITF
     *
     * \throws except::Exception if the provided reader is not a SICD
     *
     */
    static std::auto_ptr<ComplexData> getComplexData(NITFReadControl& reader);

    /*
     * Given a loaded NITFReadControl and a ComplexData object, this 
     * function loads the wideband data associated with the reader 
     * and ComplexData object.
     *
     * Optionally, a buffer can be provided to read the data into.
     * If no buffer is provided, this function will allocate its 
     * own memory with new[].
     *
     * \param reader A loaded NITFReadControl associated with the SICD
     * \param complexData complexData associated with the SICD
     * \param bufferNumBytes Optional, The size of the given buffer in bytes
     * \param buffer Optional, A pointer to the buffer to load data into
     *
     * \return a pointer to the loaded data.
     *
     * \throws except::Exception if the pixel type of the SICD is not a
     *           complex float32 or complex int64, or
     *         if the user provided buffer is not large enough to store
     *           the whole image in, or
     *         if a buffer size was provided but the buffer pointer is null
     *
     */
    static std::complex<float>* getWidebandData(
            NITFReadControl& reader,
            const ComplexData& complexData,
            size_t bufferNumBytes = 0,
            std::complex<float>* buffer = 0
            );

    /*
     *
     * Given a loaded NITFReadControl and a ComplexData object, this 
     * function loads the wideband data associated with the reader 
     * and ComplexData object.
     *
     * This function allows the user to provide a vector to be resized
     * to fit the whole image in.
     *
     * \param reader A loaded NITFReadControl associated with the SICD
     * \param complexData complexData associated with the SICD
     * \param buffer The functions output, will contain the image
     *
     */
    static void getWidebandData(
            NITFReadControl& reader,
            const ComplexData& complexData,
            std::vector<std::complex<float> >& buffer
            );

    /*
     * From here down is the legacy interface for 
     * getComplexData/getWidebandData. This is kept
     * here because attempting to wrap the above in 
     * swig would add a bunch of extra classes to the 
     * python interface, some of which are nontrivial
     * to wrap. Also, I'm sure these are used
     * in a lot of places that I'd rather not break.
     *
     * TODO: wrap the necessary things to get rid of this
     *
     */

    /*
     * Given a SICD pathname and list of schemas, provides a representation
     * of the SICD XML as a ComplexData object
     *
     * \param sicdPathname SICD NITF pathname
     * \param schemaPaths One or more files or directories containing SICD
     * schemas
     *
     * \return ComplexData associated with the SICD NITF
     *
     * \throws except::Exception if file is not a SICD
     */
    static
    std::auto_ptr<ComplexData> getComplexData(
            const std::string& sicdPathname,
            const std::vector<std::string>& schemaPaths);

    /*
     * Given a SICD pathname and list of schemas, provides a representation
     * of the SICD pixel data in a buffer
     *
     * \param sicdPathname SICD NITF pathname
     * \param schemaPaths One or more files or directories containing SICD
     * schemas
     * \param complexData The ComplexData object associated with the SICD
     * \param buffer The pre-sized buffer to be read into
     *
     * \throws except::Exception if the pixel type of the SICD is not a complex
     * float32 or complex int16
     */
    static
    void getWidebandData(
            const std::string& sicdPathname,
            const std::vector<std::string>& schemaPaths,
            const ComplexData& complexData,
            std::complex<float>* buffer);

};
}
}
#endif

