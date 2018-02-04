/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <vector>
#include <utility>
#include <memory>

#include <sys/Conf.h>
#include <nitf/System.hpp>
#include <nitf/Record.hpp>
#include <nitf/ImageBlocker.hpp>

namespace nitf
{
// For multi-segment cases, only handles vertical stacking
// where # cols matches (checked)
// # bytes/pixel matches (checked)
// Handles image segments and DESs
// Does not handle graphics or text
class ByteProvider
{
public:
    /*!
     * \return ImageBlocker with settings in sync with how the image will be
     * blocked in the NITF
     */
    std::auto_ptr<const ImageBlocker> getImageBlocker() const;

protected:
    typedef std::pair<const void*, size_t> PtrAndLength;

    ByteProvider();

    void initialize(nitf::Record& record,
                    const std::vector<PtrAndLength>& desData =
                            std::vector<PtrAndLength>(),
                    size_t numRowsPerBlock = 0,
                    size_t numColsPerBlock = 0);

private:
    void getFileLayout(nitf::Record& inRecord,
                       const std::vector<PtrAndLength>& desData);

private:
    struct SegmentInfo
    {
        SegmentInfo() :
            firstRow(0),
            numRows(0)
        {
        }

        size_t endRow() const
        {
            return (firstRow + numRows);
        }

        size_t firstRow;
        size_t numRows;
    };

    size_t mNumCols;
    size_t mOverallNumRowsPerBlock;

    // Per segment
    std::vector<size_t> mNumRowsPerBlock;
    size_t mNumColsPerBlock;
    size_t mNumBytesPerRow;
    size_t mNumBytesPerPixel;

    // Per segment
    std::vector<SegmentInfo> mImageSegmentInfo;

    std::vector<sys::byte> mFileHeader;

    // Per segment
    std::vector<std::vector<sys::byte> > mImageSubheaders;

    // All DES subheaders and data together contiguously
    std::vector<sys::byte> mDesSubheaderAndData;

    // Per segment
    std::vector<nitf::Off> mImageSubheaderFileOffsets;
    nitf::Off mDesSubheaderFileOffset;
    nitf::Off mFileNumBytes;
};
}
