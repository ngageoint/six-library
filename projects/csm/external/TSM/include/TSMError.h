//#####################################################################
//
//    FILENAME:   TSMError.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for the error structure used by the TSM.
//
//    LIMITATIONS:       None
// 
//                            Date          Author       Comment    
//    SOFTWARE HISTORY:   1 June 2004  Kevin Lam       CCB Change
//                       24 June 2005  Len Tomko       CCB Change Added
//                                                     DATA_NOT_AVAILABLE 
//
//    NOTES:
//
//#####################################################################

#ifndef __TSMERROR_H
#define __TSMERROR_H
 
#include <string>
#include "TSMMisc.h"

#ifdef WIN32
#pragma warning( disable : 4291 )
#pragma warning( disable : 4251 )
#endif

class TSM_EXPORT_API TSMError
{
   public:

      //-----------------------------------------------------------
      // Errors
      //-------------------------------------------------------------
 
      enum ErrorType 
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
         UNKNOWN_SUPPORT_DATA,
         DATA_NOT_AVAILABLE
      };

      TSMError()
      {
      }
 
      TSMError(
         const ErrorType&   aErrorType,
         const std::string& aMessage,
         const std::string& aFunction)
      {
         setTSMError( aErrorType, aMessage, aFunction );
      }

      ErrorType          getError()    { return theError; }
      const std::string& getMessage()  { return theMessage; }
      const std::string& getFunction() { return theFunction; }

      void setTSMError(
         const ErrorType&    aErrorType,
         const std::string&  aMessage,
         const std::string&  aFunction)
      {
         theError    = aErrorType;
         theMessage  = aMessage;
         theFunction = aFunction;
      }

   private:
 
      ErrorType   theError;
      //> enumeration of the error (for application control),
      //<
      std::string theMessage;
      //> string describing the error.
      //<
      std::string theFunction;
      //> string identifying the function in which the error occurred.
      //<
};
 
#endif // __TSMERROR_H 
