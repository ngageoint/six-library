//#############################################################################
//
//    FILENAME:          RasterGM.cpp
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    This file provides implementation for methods declared in the
//    RasterGM class.
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//      06-Dec-2012   JPK  Initial Coding
//                         Changed ParamSet to param::Set.  De-inlined
//                         destructor and getFamily() methods.
//                         Replaced vector<double> with EcefLocus for
//                         imageTo*Locus methods.  Added inline method
//                         getCovarianceMatrix().  Provided reference
//                         implementations for computeAllSensorPartials()
//                         methods.
//     17-Dec-2012   BAH   Documentation updates.
// 
//    NOTES:
//
//    Refer to RasterGM.h for more information.
//
//#############################################################################

#ifndef CSM_LIBRARY
#define CSM_LIBRARY
#endif
#include "RasterGM.h"

#include <stdexcept>

namespace csm {

//*****************************************************************************
// RasterGM::~RasterGM()
//*****************************************************************************
RasterGM::~RasterGM()
{}

//*****************************************************************************
// RasterGM::getFamily()
//*****************************************************************************
std::string RasterGM::getFamily() const
{
   return (GeometricModel::getFamily() + CSM_RASTER_FAMILY);
}

//*****************************************************************************
// RasterGM::computeAllSensorPartials()
//*****************************************************************************
std::vector<RasterGM::SensorPartials>
RasterGM::computeAllSensorPartials(const EcefCoord& groundPt,
                                   param::Set       pSet,
                                   double           desiredPrecision,
                                   double*          achievedPrecision,
                                   WarningList*     warnings) const
{
   
   const std::vector<int> indices = getParameterSetIndices(pSet);
   
   std::vector<RasterGM::SensorPartials> val;

   const int NUM_PARAMS = indices.size();

   if (NUM_PARAMS)
   {
      val.resize(NUM_PARAMS);

      //***
      // The achieved precision should be the MAXIMUM of the achieved
      // precision values found for each desired index.
      //***
      if (achievedPrecision) *achievedPrecision = 0.0;

      for(int i = 0; i < NUM_PARAMS; ++i)
      {
         double prec = 0.0;
         val[i] = computeSensorPartials(indices[i],
                                        groundPt,
                                        desiredPrecision,
                                        &prec,
                                        warnings);
         
         if (achievedPrecision && (prec > *achievedPrecision))
         {
            *achievedPrecision = prec;
         }
      }
   }
   return val;
}


//*****************************************************************************
// RasterGM::computeAllSensorPartials()
//*****************************************************************************
std::vector<RasterGM::SensorPartials>
RasterGM::computeAllSensorPartials(const ImageCoord& imagePt,
                                   const EcefCoord&  groundPt,
                                   param::Set        pSet,
                                   double            desiredPrecision,
                                   double*           achievedPrecision,
                                   WarningList*      warnings) const
{
  const std::vector<int> indices = getParameterSetIndices(pSet);
   
   std::vector<RasterGM::SensorPartials> val;

   const int NUM_PARAMS = indices.size();

   if (NUM_PARAMS)
   {
      val.resize(NUM_PARAMS);

      //***
      // The achieved precision should be the MAXIMUM of the achieved
      // precision values found for each desired index.
      //***
      if (achievedPrecision) *achievedPrecision = 0.0;

      for(int i = 0; i < NUM_PARAMS; ++i)
      {
         double prec = 0.0;
         val[i] = computeSensorPartials(indices[i],
                                        imagePt,
                                        groundPt,
                                        desiredPrecision,
                                        &prec,
                                        warnings);
         
         if (achievedPrecision && (prec > *achievedPrecision))
         {
            *achievedPrecision = prec;
         }
      }
   }
   return val;
}

//*****************************************************************************
// RasterGM::getUnmodeledError()
//*****************************************************************************
std::vector<double> RasterGM::convertUnmodeledError(const std::vector<double>& sixUnmodeledError) const
{
    if (sixUnmodeledError.empty())
    {
        return {};
    }

    if (!((sixUnmodeledError.size() == 3) || (sixUnmodeledError.size() == 7)))
    {
        throw std::logic_error("sixUnmodeledError should contain 3 or 7 values.");
    }

    // From Bill: Here is the mapping from the UnmodeledError to the 2x2 covariance matrix:
    //    [0][0] = Xrow; [1][1] = Ycol; 
    //    [1][0] = [0][1] = XrowYcol * Xrow * Ycol
    const auto line_variance = sixUnmodeledError[0] /*Xrow*/;
    const auto sample_variance = sixUnmodeledError[1] /*Ycol*/;
    const auto linesample_covariance = sixUnmodeledError[2] /*XrowYcol*/ * line_variance * sample_variance;
    const auto sampleline_covariance = linesample_covariance;
    return { line_variance, linesample_covariance, sampleline_covariance, sample_variance }; // see getUnmodeledCrossCovariance()
}
std::vector<double> RasterGM::getUnmodeledError(const ImageCoord& imagePt) const
{
    auto sixUnmodeledError = convertUnmodeledError(getSIXUnmodeledError());
    if (!sixUnmodeledError.empty())
    {
        return sixUnmodeledError;
    }

    // Nothing in the XML, use existing code
    return getUnmodeledCrossCovariance(imagePt, imagePt);
}

} // namespace csm
