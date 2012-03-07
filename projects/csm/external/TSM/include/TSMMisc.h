//#############################################################################
//
//    FILENAME:   TSMMisc.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for the constants and other definitions used by the TSM.
//
//    LIMITATIONS:       None
// 
//                       Date          Author   Comment    
//    SOFTWARE HISTORY:  01-Jul-2003   LMT      Initial version.
//
//    NOTES:
//
//#############################################################################
#ifndef __TSMMISC_H
#define __TSMMISC_H
#ifdef _WIN32
# ifdef TSM_LIBRARY
#  define TSM_EXPORT_API __declspec(dllexport)
# else
#  define TSM_EXPORT_API __declspec(dllimport)
# endif
#else
#  define TSM_EXPORT_API
#endif

   static const int MAX_NAME_LENGTH = 40;
   static const int MAX_FUNCTION_NAME_LENGTH = 80;
   static const int MAX_MESSAGE_LENGTH = 512;

class TSM_EXPORT_API TSMMisc
{
   public:

//--------------------------------------------------------------------------    
// Enumeratons    
//-------------------------------------------------------------------------- 
   enum Param_CharType    
{       NONE,
       FICTITIOUS,
       REAL,
       EXACT    }; 
//>       
//  This enumeration lists the possible parameter or characteristic        
//  types as follows.       
//        
//  NONE       - Parameter value has not yet been initialized.       
//  FICTITIOUS - Parameter value has been calculated by resection       
//               or other means.       
//  REAL       - Parameter value has been measured or read from        
//               support data.       
//  EXACT      - Parameter value has been specified and is assumed to        
//               have no uncertainty.       
//< 


};

#endif

