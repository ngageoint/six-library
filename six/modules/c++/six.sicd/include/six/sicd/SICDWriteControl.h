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
/*!
 * \class SICDWriteControl
 * \brief Specialized NITF write control that allows for writing a SICD out
 * in pieces
 *
 * Normally to write out a SICD, you'd use NITFWriteControl.  This works fine
 * when you either a) have all the pixels in memory or b) can get the pixels
 * on demand (perhaps from a file) via an io::InputStream.  If you instead have
 * a use case where you will be getting/generating pixels gradually rather
 * than all at once, and you may get/generate them in an order other than the
 * order they'll be written to disk, you can use this class instead.
 */
class SICDWriteControl : public six::NITFWriteControl
{
public:
    /*!
     * Constructor
     *
     * \param outputPathname Full path to the output file to write
     * \param schemaPaths Directories or files of schema locations
     */
    SICDWriteControl(const std::string& outputPathname,
                     const std::vector<std::string>& schemaPaths);

    using NITFWriteControl::initialize;

    /*!
     * Initializes the control.  Either this or the base class's initialize()
     * function must be called prior to any of the save() methods or doing
     * anything to manipulate the underlying Record object.  This one is a
     * convenience function since we know a container for SICD will always
     * contain exactly one 'data' object.
     *
     * \param data Representation of the complex data
     */
    void initialize(const ComplexData& data);

    using NITFWriteControl::save;

    /*!
     * Writes a portion of the pixels to the file.  The first time this is
     * called, the headers will be written to the file.  This may be called
     * as many times as desired with different AOIs in any order.
     *
     * TODO: No sanity checks are done that the offset and dims are within the
     * global image dimensions.
     *
     * \param imageData The image data pixels to write.  The underlying type
     *     will be complex short or complex float based on the complex data
     *     sent in during initialize()
     * \param offset The global offset in pixels as to where these pixels are
     *     in the image.  If this is a multi-segment NITF, this is still simply
     *     the global pixel location (this class will take care of writing it
     *     to the appropriate image segment).
     * \param dims The dimensions of the image data pixels.
     * \param restoreData Unless the OPT_BYTE_SWAP option has been set or this
     *     is a big endian system, the incoming data needs to be endian swapped.
     *     For memory efficiency, this is done in-place.  This flag controls
     *     if the incoming data should be swapped back afterwards.  By default,
     *     the data will be swapped back.
     */
    void save(void* imageData,
              const types::RowCol<size_t>& offset,
              const types::RowCol<size_t>& dims,
              bool restoreData = true);

    /*!
     * Closes the underlying IO interface.  This will occur implicitly in the
     * destructor if it's not called.
     */
    void close();

private:
    void writeHeaders();

    void write(const std::vector<sys::byte>& data);

private:
    std::auto_ptr<nitf::IOInterface> mIO;
    const std::vector<std::string> mSchemaPaths;

    std::vector<nitf::Off> mImageDataStart;
    std::vector<NITFSegmentInfo> mImageSegmentInfo;
    bool mHaveWrittenHeaders;
};
}
}

#endif
