//#############################################################################
//
//    FILENAME:   TSMCovarianceModel.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for the image-to-image cross covariance model base class.
//    The Covariance Model is encapsulated in a C++ class for
//    transfer through the TSM interface. The Covariance Model is passed as
//    a pointer to a simple Covariance Model base class (for example, 
//    tsm_CovarianceModel *covModel).
//
//    LIMITATIONS:       None
//
//                       Date          Author   Comment
//    SOFTWARE HISTORY:  01-Nov-2004   KRW      Initial version.
//
//    NOTES:
//
//#############################################################################

#ifndef __TSMCOVARIANCE_MODEL_H
#define __TSMCOVARIANCE_MODEL_H

#include <string>
#include "TSMMisc.h"

class TSM_EXPORT_API tsm_CovarianceModel
{
public:
   tsm_CovarianceModel ()         { _format = "UNKNOWN"; }
   virtual ~tsm_CovarianceModel (){ _format.erase(); }

   void getFormat( std::string &format ) const { format = _format; }

protected:

   std::string _format;

};

#endif

