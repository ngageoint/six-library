#ifndef __ZIP_GZIP_INPUT_STREAM_H__
#define __ZIP_GZIP_INPUT_STREAM_H__

#include "zip/Types.h"

namespace zip
{
/*!
 *  \class GZipInputStream
 *  \brief Read from a gzip file
 *
 *  This class lacks the knowledge of how many uncompressed bytes
 *  are available from a stream, and so, like the NetConnection, it
 *  does not override the available() function.  For this reason,
 *  the user should only call streamTo() if the optional buffer size
 *  argument is given, and in a loop.  On the last run, the buffer
 *  size should will probably smaller than the amount requested.
 */
class GZipInputStream: public io::InputStream
{
    gzFile mFile;
public:

    //!  Constructor requires initialization
    GZipInputStream(std::string file);

    //!  Destructor
    virtual ~GZipInputStream()
    {
    }

    /*!
     *  Close the gzip stream.  You must call this
     *  afterward (it is not done automatically);
     */
    virtual void close();

    /*!
     *  Read len bytes into the buffer from the stream.
     *  This is a little tricky since we do not know the
     *  length of the read.
     */
    virtual sys::SSize_T read(sys::byte* b, sys::Size_T len);

};
}

#endif
