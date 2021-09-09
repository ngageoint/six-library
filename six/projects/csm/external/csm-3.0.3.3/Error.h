//#####################################################################
//
//    FILENAME:          Error.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for the error class used by the CSM.
//
//    LIMITATIONS:       None
//
//    SOFTWARE HISTORY:
//     Date          Author      Comment
//     -----------   ------      -------
//      1 Jun 2004   Kevin Lam   CCB Change
//     24 Jun 2005   Len Tomko   CCB Change Added
//                                DATA_NOT_AVAILABLE
//     09 Mar 2010   Don Leonard CCB Change Deleted DATA_NOT_AVAILABLE
//     02 Mar 2012   SCM         Refactored interface.
//     30-Oct-2012   SCM         Renamed to Error.h
//     17-Dec-2012   BAH         Documentation updates.
//
//    NOTES:
//
//#####################################################################

#ifndef __CSM_ERROR_H
#define __CSM_ERROR_H

#include "csm.h"
#include <string>

namespace csm
{

class CSM_EXPORT_API Error : public std::exception
{
public:
   enum ErrorType
   //>
   {
      ALGORITHM = 1,
      BOUNDS,
      FILE_READ,
      FILE_WRITE,
      ILLEGAL_MATH_OPERATION,
      INDEX_OUT_OF_RANGE,
      INVALID_SENSOR_MODEL_STATE,
      INVALID_USE,
      ISD_NOT_SUPPORTED,
      MEMORY,
      SENSOR_MODEL_NOT_CONSTRUCTIBLE,
      SENSOR_MODEL_NOT_SUPPORTED,
      STRING_TOO_LONG,
      UNKNOWN_ERROR,
      UNSUPPORTED_FUNCTION,
      UNKNOWN_SUPPORT_DATA
   };
   //<

   Error() : theError(UNKNOWN_ERROR), theMessage(), theFunction() {}
   Error(const ErrorType&   aErrorType,
         const std::string& aMessage,
         const std::string& aFunction)
      : theError(aErrorType), theMessage(aMessage), theFunction(aFunction) {}
   virtual ~Error() throw () {}

   ErrorType          getError() const    { return theError; }
   const std::string& getMessage() const  { return theMessage; }
   const std::string& getFunction() const { return theFunction; }

   virtual const char* what() const throw() { return theMessage.c_str(); }
      //> Implement the std::exception interface.
      //<

   void setError(const ErrorType&   aErrorType,
                 const std::string& aMessage,
                 const std::string& aFunction)
   {
      theError    = aErrorType;
      theMessage  = aMessage;
      theFunction = aFunction;
   }

private:
   ErrorType   theError;
      //> This enumeration of the error is used for application control.
      //<

   std::string theMessage;
      //> This string describes the error.
      //<

   std::string theFunction;
      //> This string identifies the function in which the error occurred.
      //<
};

} // namespace csm

#endif
