/* =========================================================================
 * This file is part of sio.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sio.lite-c++ is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_sio_lite_ReadUtils_h_
#define CODA_OSS_sio_lite_ReadUtils_h_

#include <string>
#include <memory>
#include <vector>

#include <gsl/gsl.h>
#include <sys/Conf.h>
#include <except/Exception.h>
#include <types/RowCol.h>
#include <mem/ScopedArray.h>
#include <sio/lite/FileReader.h>
#include <sio/lite/FileHeader.h>
#include <sio/lite/ElementType.h>
#include <sys/filesystem.h>

namespace sio
{
namespace lite
{
/*
 *  \function readSIO
 *  \brief Opens an SIO of a templated data type.
 *
 *  \param pathname The location of the sio.
 *  \param dims Output for the size of the sio.
 *  \param image Output for the data.
 */
template <typename InputT>
void readSIO(const std::string& pathname,
             types::RowCol<size_t>& dims,
             std::unique_ptr<InputT[]>& image)
{
    sio::lite::FileReader reader(pathname);
    const sio::lite::FileHeader* const header(reader.getHeader());
    dims.row = gsl::narrow<size_t>(header->getNumLines());
    dims.col = gsl::narrow<size_t>(header->getNumElements());

    if (header->getElementSize() != sizeof(InputT) ||
        header->getElementType() != sio::lite::ElementType<InputT>::Type)
    {
        throw except::Exception(Ctxt("Unexpected format"));
    }

    const size_t numPixels(dims.row * dims.col);
    image.reset(new InputT[numPixels]);
    reader.read(image.get(), numPixels * sizeof(InputT), true);
}
template <typename InputT>
void readSIO(const sys::filesystem::path& pathname,
             types::RowCol<size_t>& dims,
             std::vector<InputT>& image)
{
    sio::lite::FileReader reader(pathname.string());
    const sio::lite::FileHeader* const header(reader.getHeader());
    dims.row = gsl::narrow<size_t>(header->getNumLines());
    dims.col = gsl::narrow<size_t>(header->getNumElements());

    if (header->getElementSize() != sizeof(InputT) ||
        header->getElementType() != sio::lite::ElementType<InputT>::Type)
    {
        throw except::Exception(Ctxt("Unexpected format"));
    }

    image.resize(dims.area());
    reader.read(coda_oss::span<InputT>(image.data(), image.size()), true /*verifyFullRead*/);
}

/*
 *  \function readSIOVerifyDimensions
 *  \brief Opens an sio and ensures it is the same size as a passed in dims.
 *
 *  \param pathname The location of the sio.
 *  \param dims The dimensions to compare to.
 *  \param image Output for the data.
 */
template <typename InputT>
void readSIOVerifyDimensions(const std::string& pathname,
                             const types::RowCol<size_t>& dims,
                             std::unique_ptr<InputT[]>& image)
{
    types::RowCol<size_t> theseDims;
    readSIO<InputT>(pathname, theseDims, image);

    if (theseDims != dims)
    {
        throw except::Exception(Ctxt(pathname + " not sized as expected"));
    }
}
}
}

#endif  // CODA_OSS_sio_lite_ReadUtils_h_
