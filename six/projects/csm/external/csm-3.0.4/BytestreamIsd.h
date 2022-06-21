//#############################################################################
//
//    FILENAME:          BytestreamIsd.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for the byte stream ISD class derived from the Isd base class.
//    This class holds ISD in a string of unspecified format.
//    The field "data" is set with the ISD.
//
//    LIMITATIONS:       None
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     01-Jul-2003   LMT      Initial version.
//     06-Feb-2004   KRW      Incorporates changes approved by
//                            January and February 2004
//                            Configuration control board.
//     02-Mar-2012   SCM      Refactored interfaces.
//     30-Oct-2012   SCM      Renamed to BytestreamIsd.h
//     17-Dec-2012   BAH      Documentation updates.
//
//    NOTES:
//
//#############################################################################

#ifndef __CSM_BYTESTREAMISD_H
#define __CSM_BYTESTREAMISD_H

#include "Isd.h"

namespace csm
{

class CSM_EXPORT_API BytestreamIsd : public Isd
{
public:
   explicit BytestreamIsd(const std::string& data,
                          const std::string& filename = "")
      : Isd("BYTESTREAM", filename), theData(data) {}
   virtual ~BytestreamIsd();

   const std::string data() const { return theData; }
      //> This method returns the raw bytestream data associated with the image
      //  support data.  This should not be empty.  The string can contain null
      //  characters, so care must be taken when calling std::string::c_str().
      //<

   void setData(const std::string& data) { theData = data; }
      //> This method returns the raw bytestream data associated with the image
      //  support data.  This should not be empty.
      //<

private:
   std::string theData;
      //> This data member is the ISD data to be used in construting the CSM
      //  model.  It must be supplied.
      //<
};

} // namespace csm

#endif
