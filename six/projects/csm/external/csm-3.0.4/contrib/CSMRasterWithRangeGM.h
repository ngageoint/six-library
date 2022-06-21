//#############################################################################
//
//    FILENAME:          CSMRasterWithRangeGM.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for abstract base class that is to provide a common interface from
//    which all imaging sensors providing range data will derive from
//
//    LIMITATIONS:       None
//
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment   
//     -----------   ------   -------
//
//     26-Sep-2012   JPK      Initial Creation
//
//    NOTES:
//
//#############################################################################

#ifndef __CSMRASTERWITHRANGEGM_H
#define __CSMRASTERWITHRANGEGM_H

#include "CSMRasterGM.h"
#include <vector>
#include <string>

#define CSM_RASTER_RANGE_FAMILY "WithRange"

namespace csm
{

class CovarianceModel;

class CSM_EXPORT_API RasterWithRangeGM : public RasterGM
{
public:
   RasterWithRangeGM() {}
   virtual ~RasterWithRangeGM() { }

   virtual std::string getFamily() const { return (RasterGM::getFamily() +
                                                   CSM_RASTER_RANGE_FAMILY); }

   //---
   // Core Photogrammetry
   //---
   virtual EcefCoord imageRangeToGround(const ImageCoord& imagePt,
                                        double slantRange,
                                        double desired_precision = 0.001,
                                        double* achieved_precision = NULL,
                                        WarningList* warnings = NULL) const = 0;
      //> This method converts a given line and sample (pixels) in image
      //  space to a ground point given a slant range in meters.
      //<
   virtual EcefCoordCovar imageRangeToGround(const ImageCoordCovar& imagePt,
                                             double slantRange,
                                             double slantRangeVariance,
                                             double desired_precision = 0.001,
                                             double* achieved_precision = NULL,
                                             WarningList* warnings = NULL) const = 0;
      //> This method converts a given line and sample (pixels) in
      //  image space (given a slant range in meters) to a ground point and 
      //  returns accuracy information associated with the ground coordinate.
      //<
   
   virtual EcefCoordCovar imageRangeToGround(double desired_precision = 0.001,
                                             double* achieved_precision = NULL,
                                             WarningList* warnings = NULL) const = 0;
      //> This method determines the ground coordinate at the center of the
      //  image using the available range information and its associated 
      //  accuracy. The resulting location should be equvialent to that
      //  returned by getReferencePoint() assuming the user has not adjusted
      //  it.
      //<
};

} // namespace csm

#endif

