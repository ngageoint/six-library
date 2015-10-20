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
     * Given a SICD path name and a list of schema, this function reads
     * and parses the SICD in order to provide the wideband data as well
     * as the ComplexData associated with the image..
     *
     * \param sicdPathname SICD NITF pathname
     * \param schemaPaths One or more files or directories containing SICD 
     * schemas
     * \param ComplexData associated with the SICD NITF
     * \param widebandData, vector containing SICD wideband data
     *
     * \throws See six::sicd::Utilities::getComplexData and 
     *           six::sicd::Utilities::getWidebandData
     *
     */
    static void readSicd(const std::string& sicdPathname,
                                 const std::vector<std::string>& schemaPaths,
                                 std::auto_ptr<ComplexData>& complexData,
                                 std::vector<std::complex<float> >& widebandData);

    /*
     * Given a reference to a loaded NITFReadControl, this function
     * parses the SICD's XML and returns a new ComplexData object.
     *
     * \param reader A NITFReadControl loaded with the desired SICD
     * \param complexData reference to a smart pointer to complexData,
     * to be filled in with the loaded complexData
     *
     * \throws except::Exception if the provided reader is not a SICD
     *
     */
    static void getComplexData(NITFReadControl& reader,
                               std::auto_ptr<ComplexData>& complexData);

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
     * \param buffer A pointer to the buffer to load data into
     *
     * \return a pointer to the loaded data.
     *
     * \throws except::Exception if the pixel type of the SICD is not a
     *           complex float32 or complex int64, or
     *         the buffer pointer is null
     *
     */
    static void getWidebandData(
            NITFReadControl& reader,
            const ComplexData& complexData,
            std::complex<float>* buffer
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
     * This interface is more suitable if you need just
     * the complexData, or if you conditionally need the
     * wideband data.
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

