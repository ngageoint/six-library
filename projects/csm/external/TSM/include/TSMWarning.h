//####################################################################
//
//    FILENAME:   TSMWarning.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for the warning structure used by the TSM.
//
//    LIMITATIONS:       None
// 
//                       Date          Author   Comment    
//    SOFTWARE HISTORY:  1 June 2004   Kevin Lam  CCB Change
//
//    NOTES: 
//
//###################################################################

#ifndef __TSMWARNING_H
#define __TSMWARNING_H
 
#include <string>
#include "TSMMisc.h"
#ifdef WIN32
#pragma warning( disable : 4291 )
#pragma warning( disable : 4251 )
#endif
class TSM_EXPORT_API TSMWarning
{
   public:

      //-------------------------------------------------------------
      // Warnings
      //---------------------------------------------------------------
 
      enum WarningType 
      {
         UNKNOWN_WARNING = 1,
         DATA_NOT_AVAILABLE,
         PRECISION_NOT_MET,
         NEGATIVE_PRECISION,
         IMAGE_COORD_OUT_OF_BOUNDS,
         IMAGE_ID_TOO_LONG,
         NO_INTERSECTION
      };

      TSMWarning()
      {
      }
 
      TSMWarning(
         const WarningType& aWarningType,
         const std::string& aMessage,
         const std::string& aFunction)
      {
         setTSMWarning( aWarningType, aMessage, aFunction );
      }

      WarningType        getWarning()   { return theWarning; }
      const std::string& getMessage()  { return theMessage; }
      const std::string& getFunction() { return theFunction; }
      
      void setTSMWarning(
         const WarningType& aWarningType,
         const std::string& aMessage,
         const std::string& aFunction)
	 {
	    theWarning  = aWarningType;
	    theMessage  = aMessage;
	    theFunction = aFunction;
	 }

   private:
 
      WarningType theWarning;
      //> enumeration of the warning (for application control),
      //<
      std::string theMessage;
      //> string describing the warning.
      //<
      std::string theFunction;
      //> string identifying the function in which the warning occurred.
      //<
};
 
#endif // __TSMWARNING_H 
