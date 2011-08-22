//#############################################################################
//
//    FILENAME:   tsm_ISDByteStream.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for the ByteStream ISD class derived from the tsm_ISD base class. 
//    ISD is encapsulated in a C++ class for transfer through the TSM 
//    interface. This class is designed to hold ISD in a std::string of unspecified 
//    format. The field _isd is set with the ISD. 
//    
//    LIMITATIONS:       None
// 
//                       Date          Author   Comment    
//    SOFTWARE HISTORY:  01-Jul-2003   LMT      Initial version.
//                       06-Feb-2004   KRW      Incorporates changes approved by
//                                                January and February 2004
//                                               Configuration control board.
//    NOTES:
//
//#############################################################################
#ifndef __tsm_ISDBYTESTREAM_H
#define __tsm_ISDBYTESTREAM_H

#include "TSMImageSupportData.h"
#include <string>
#include "TSMMisc.h"
#ifdef _WIN32
#pragma warning( disable : 4291 )
#pragma warning( disable : 4251 )
#endif

class TSM_EXPORT_API bytestreamISD : public tsm_ISD
{
public:
   bytestreamISD() { _format = "BYTESTREAM"; } 
   bytestreamISD(std::string filename);
   ~bytestreamISD() {_format.erase(); _isd.erase();}

   std::string _isd;
};

#endif
