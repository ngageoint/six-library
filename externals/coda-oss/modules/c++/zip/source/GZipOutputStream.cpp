#include "zip/GZipOutputStream.h"

using namespace zip;

GZipOutputStream::GZipOutputStream(std::string file)
{
    mFile = gzopen(file.c_str(), "wb");
    if (mFile == NULL)
        throw except::IOException(Ctxt(FmtX("Failed to open gzip stream [%s]",
                file.c_str())));

}

void GZipOutputStream::write(const sys::byte* b, sys::Size_T len)
{
    sys::Size_T written = 0;
    int rv = 0;
    do
    {
        rv = gzwrite(mFile, (const voidp)&(b[written]), len - written);
        if (rv < 0)
        {
            std::string err(gzerror(mFile, &rv));
            throw except::Exception(Ctxt(err));
        }
        if (!rv)
            break;
        written += rv;
    }
    while (written < len);

}

void GZipOutputStream::close()
{
    gzclose( mFile);
    mFile = NULL;
}

