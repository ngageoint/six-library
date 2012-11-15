#include "sio/lite/FileWriter.h"

void sio::lite::FileWriter::write(sio::lite::FileHeader* header, std::vector<io::InputStream*> bandStreams)
{
    header->to(bandStreams.size(), *mStream); //write header
    //write band-sequential
    for (std::vector<io::InputStream*>::iterator iter = bandStreams.begin(), end = bandStreams.end();
            iter != end; ++iter)
        (*iter)->streamTo(*mStream);
}


void sio::lite::FileWriter::write(int numLines, int numElements, int elementSize,
                                  int elementType, std::vector<io::InputStream*> bandStreams)
{
    sio::lite::FileHeader hdr(numLines, numElements, elementSize, elementType, 1);
    write(&hdr, bandStreams);
}


void sio::lite::FileWriter::write(sio::lite::FileHeader* header, const sys::byte* data, int numBands)
{
    header->to(numBands, *mStream); //write header
    mStream->write(data, static_cast<sys::Size_T>(header->getNumLines())
                    * static_cast<sys::Size_T>(header->getNumElements())
                    * static_cast<sys::Size_T>(header->getElementSize())
                    * static_cast<sys::Size_T>(numBands));
}

void sio::lite::FileWriter::write(int numLines, int numElements, int elementSize,
                                  int elementType, const sys::byte* data, int numBands)
{
    sio::lite::FileHeader hdr(numLines, numElements, elementSize, elementType);
    write(&hdr, data, numBands);
}
