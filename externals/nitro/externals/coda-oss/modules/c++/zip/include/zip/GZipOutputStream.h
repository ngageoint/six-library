#ifndef __ZIP_GZIP_OUTPUT_STREAM_H__
#define __ZIP_GZIP_OUTPUT_STREAM_H__

#include "zip/Types.h"

namespace zip
{
/*!
 *  \class GZipOutputStream
 *  \brief IO wrapper for zlib API
 *
 */
class GZipOutputStream: public io::OutputStream
{
    gzFile mFile;
public:
    //!  Constructor requires initialization
    GZipOutputStream(std::string file);

    //!  Destructor
    virtual ~GZipOutputStream()
    {
    }

    /*!
     *  Write len (or less) bytes into the gzip stream.
     *  If an error occurs, this function throws.  If
     *  the underlying gzip stream ever returns zero bytes
     *  the call returns.
     *
     */
    virtual void write(const sys::byte* b, sys::Size_T len);

    /*!
     *  Close the gzip stream.  You must call this
     *  afterward (it is not done automatically).
     */
    virtual void close();

};

}

#endif
