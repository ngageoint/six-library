#include "zip/GZipInputStream.h"

using namespace zip;

GZipInputStream::GZipInputStream(std::string file)
{
    mFile = gzopen(file.c_str(), "rb");
    if (mFile == NULL)
        throw except::IOException(Ctxt(FmtX("Failed to open gzip stream [%s]",
                file.c_str())));
}

void GZipInputStream::close()
{
    gzclose( mFile);
    mFile = NULL;
}

sys::SSize_T GZipInputStream::read(sys::byte* b, sys::Size_T len)
{
    int rv = gzread(mFile, b, len);
    if (rv == -1)
    {
        std::string err(gzerror(mFile, &rv));
        throw except::IOException(Ctxt(err));
    }
    else if (rv == 0)
        return io::InputStream::IS_EOF;

    return (sys::SSize_T) rv;
}
