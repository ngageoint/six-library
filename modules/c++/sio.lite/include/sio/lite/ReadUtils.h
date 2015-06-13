/******************************************************************************
 * (C) Copyright 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/

#ifndef __SIO_LITE_SIO_READER_H__
#define __SIO_LITE_SIO_READER_H__

#include <string>
#include <sys/Conf.h>
#include <except/Exception.h>
#include <types/RowCol.h>
#include <mem/ScopedArray.h>
#include <sio/lite/FileReader.h>
#include <sio/lite/FileHeader.h>
#include <sio/lite/ElementType.h>

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
             mem::ScopedArray<InputT>& image)
{
    sio::lite::FileReader reader(pathname);
    const sio::lite::FileHeader* const header(reader.getHeader());
    dims.row = header->getNumLines();
    dims.col = header->getNumElements();

    if (header->getElementSize() != sizeof(InputT) ||
        header->getElementType() != sio::lite::ElementType<InputT>::Type)
    {
        throw except::Exception(Ctxt("Unexpected format"));
    }

    const size_t numPixels(dims.row * dims.col);
    image.reset(new InputT[numPixels]);
    reader.read(reinterpret_cast<sys::byte*>(image.get()),
        numPixels * sizeof(InputT));
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
                             mem::ScopedArray<InputT>& image)
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

#endif
