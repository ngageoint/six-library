//#############################################################################
//
//    FILENAME:   TSMImageSupportData.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for the ISD base class. ISD is encapsulated in a C++ class for 
//    transfer through the TSM interface. ISD is passed as a pointer to a 
//    simple ISD base class (for example, tsm_ISD *isd). i
//    
//    LIMITATIONS:       None
// 
//                       Date          Author   Comment    
//    SOFTWARE HISTORY:  01-Jul-2003   LMT      Initial version.
//                       06-Feb-2004   KRW      Incorporates changes approved by 
//                                               January and February 2004
//                                               Configuration control board.
//
//    NOTES:
//
//#############################################################################
#ifndef __TSMIMAGESUPORTDATA_H
#define __TSMIMAGESUPORTDATA_H

#include <string>
#include "TSMMisc.h"
#ifdef _WIN32
#pragma warning( disable : 4291 )
#pragma warning( disable : 4251 )
#endif
class TSM_EXPORT_API tsm_ISD
{
public:
   tsm_ISD() { _format = "UNKNOWN";  }
   virtual   ~tsm_ISD(){ _format.erase(); }

   void getFormat( std::string &format ) const { format = _format; }

protected:
   std::string _format;
};


#endif
