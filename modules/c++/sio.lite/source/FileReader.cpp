#include "sio/lite/FileReader.h"

sys::Off_T sio::lite::FileReader::seek( sys::Off_T offset, Whence whence )
{
    if (whence == START)
        offset = offset + headerLength;
    
    ( (io::FileInputStream*)inputStream )->seek(offset, whence);
    return this->tell();
}

sys::Off_T sio::lite::FileReader::tell()
{
    return ( (io::FileInputStream*)inputStream )->tell() - headerLength;
}

void sio::lite::FileReader::killStream()
{
    if (inputStream && own)
    {
        io::FileInputStream* file = (io::FileInputStream*)inputStream;
        if (file->isOpen()) file->close();
        delete inputStream;
    }
}
