//#############################################################################
//
//    FILENAME:          RasterGM.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for abstract class that is to provide a common interface from
//    which CSM raster geometric models will inherit.  It is derived from the
//    GeometricModel class.
//
//    LIMITATIONS:       None
//
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     27-Jun-2003   LMT    Initial version.
//     01-Jul-2003   LMT    Remove constants, error/warning
//                          and make methods pure virtual.
//                          CharType enum.
//     31-Jul-2003   LMT    Change calls with a "&" to a "*",
//                          combined CharType with ParamType
//                          to create Param_CharType, //reordered
//                          methods to match API order, added
//                          systematic error methods.
//     06-Aug-2003   LMT    Removed all Characteristic calls.
//     08-Oct 2003   LMT    Added getImageSize calls
//     06-Feb-2004   KRW    Incorporates changes approved by
//                          January and February 2004
//                          configuration control board.
//     30-Jul-2004   PW     Initail API 3.1 version
//     01-Nov-2004   PW     October 2004 CCB
//     22 Oct 2010   DSL    CCB Change add getCurrentCrossCovarianceMatrix
//                                     and getOriginalCrossCovarianceMatrix
//     22 Oct 2010   DSL    CCB Change add getCurrentCrossCovarianceMatrix
//                                     and getOriginalCrossCovarianceMatrix
//     25 Oct 2010   DSL    CCB Change add getNumGeometricCorrectionSwitches,
//                                         getGeometricCorrectionName,
//                                         getCurrentGeometricCorrectionSwitch,
//                                     and setCurrentGeometricCorrectionSwitch
//     25 Oct 2010   DSL    CCB Change add getNumGeometricCorrectionSwitches,
//                                         getGeometricCorrectionName,
//                                         getCurrentGeometricCorrectionSwitch,
//                                     and setCurrentGeometricCorrectionSwitch
//     02-Mar-2012   SCM    Refactored interface.
//     02-Jul-2012   SCM    Made getUnmodeledError() be implemented inline.
//     26-Sep-2012   JPK    Split SensorModel class into GeometricModel and
//                          RasterGM classes.  RasterGM is now the
//                          equivalent class to the previous SensorModel class.
//     26-Sep-2012   SCM    Moved all sensor partials to this class.
//     30-Oct-2012   SCM    Renamed to RasterGM.h
//     31-Oct-2012   SCM    Moved getTrajectoryIdentifier() to Model.  Moved
//                          unmodeled error methods to GeometricModel.  Made
//                          compute partial methods const.
//     01-Nov-2012   SCM    Moved unmodeled error methods back to RasterGM.
//     27-Nov-2012   JPK    Cleaned up some comments, variable names and
//                          changed return type for getCovarianceModel() from
//                          pointer to const reference.  Removed unused
//                          testAPIVersionSubclass().
//     29-Nov-2012   JPK    Modified computeAllSensorPartials to return
//                          results for a specified ParamSet.
//     06-Dec-2012   JPK    Changed ParamSet to param::Set.  De-inlined
//                          destructor and getFamily() methods.
//                          Replaced vector<double> with EcefLocus for
//                          imageTo*Locus methods.  Added inline method
//                          getCovarianceMatrix().  Provided reference
//                          implementations for computeAllSensorPartials()
//                          methods.  Made getCovarianceModel() pure
//                          virtual.
//     17-Dec-2012   BAH    Documentation updates.
//     12-Feb-2013   JPK    Renamed CovarianceModel to CorrelationModel.
//
//    NOTES:
//
//#############################################################################

#ifndef __CSM_RASTERGM_H
#define __CSM_RASTERGM_H

#include "GeometricModel.h"

#define CSM_RASTER_FAMILY "Raster"

namespace csm
{
class CorrelationModel;

class CSM_EXPORT_API RasterGM : public GeometricModel
{
public:
   RasterGM() {}
   virtual ~RasterGM();

   virtual std::string getFamily() const;
      //> This method returns the Family ID for the current model.
      //<

   //---
   // Core Photogrammetry
   //---
   virtual ImageCoord groundToImage(const EcefCoord& groundPt,
                                    double desiredPrecision = 0.001,
                                    double* achievedPrecision = NULL,
                                    WarningList* warnings = NULL) const = 0;
      //> This method converts the given groundPt (x,y,z in ECEF meters) to a
      //  returned image coordinate (line, sample in full image space pixels).
      //
      //  Iterative algorithms will use desiredPrecision, in meters, as the
      //  convergence criterion, otherwise it will be ignored.
      //
      //  If a non-NULL achievedPrecision argument is received, it will be
      //  populated with the actual precision, in meters, achieved by iterative
      //  algorithms and 0.0 for deterministic algorithms.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   virtual ImageCoordCovar groundToImage(const EcefCoordCovar& groundPt,
                                         double desiredPrecision = 0.001,
                                         double* achievedPrecision = NULL,
                                         WarningList* warnings = NULL) const = 0;
      //> This method converts the given groundPt (x,y,z in ECEF meters and
      //  corresponding 3x3 covariance in ECEF meters squared) to a returned
      //  image coordinate with covariance (line, sample in full image space
      //  pixels and corresponding 2x2 covariance in pixels squared).
      //
      //  Iterative algorithms will use desiredPrecision, in meters, as the
      //  convergence criterion, otherwise it will be ignored.
      //
      //  If a non-NULL achievedPrecision argument is received, it will be
      //  populated with the actual precision, in meters, achieved by iterative
      //  algorithms and 0.0 for deterministic algorithms.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   virtual EcefCoord imageToGround(const ImageCoord& imagePt,
                                   double height,
                                   double desiredPrecision = 0.001,
                                   double* achievedPrecision = NULL,
                                   WarningList* warnings = NULL) const = 0;
      //> This method converts the given imagePt (line,sample in full image
      //  space pixels) and given height (in meters relative to the WGS-84
      //  ellipsoid) to a returned ground coordinate (x,y,z in ECEF meters).
      //
      //  Iterative algorithms will use desiredPrecision, in meters, as the
      //  convergence criterion, otherwise it will be ignored.
      //
      //  If a non-NULL achievedPrecision argument is received, it will be
      //  populated with the actual precision, in meters, achieved by iterative
      //  algorithms and 0.0 for deterministic algorithms.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   virtual EcefCoordCovar imageToGround(const ImageCoordCovar& imagePt,
                                        double height,
                                        double heightVariance,
                                        double desiredPrecision = 0.001,
                                        double* achievedPrecision = NULL,
                                        WarningList* warnings = NULL) const = 0;
      //> This method converts the given imagePt (line, sample in full image
      //  space pixels and corresponding 2x2 covariance in pixels squared)
      //  and given height (in meters relative to the WGS-84 ellipsoid) and
      //  corresponding heightVariance (in meters) to a returned ground
      //  coordinate with covariance (x,y,z in ECEF meters and corresponding
      //  3x3 covariance in ECEF meters squared).
      //
      //  Iterative algorithms will use desiredPrecision, in meters, as the
      //  convergence criterion, otherwise it will be ignored.
      //
      //  If a non-NULL achievedPrecision argument is received, it will be
      //  populated with the actual precision, in meters, achieved by iterative
      //  algorithms and 0.0 for deterministic algorithms.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   virtual EcefLocus imageToProximateImagingLocus(
      const ImageCoord& imagePt,
      const EcefCoord& groundPt,
      double desiredPrecision = 0.001,
      double* achievedPrecision = NULL,
      WarningList* warnings = NULL) const = 0;
      //> This method, for the given imagePt (line, sample in full image space
      //  pixels), returns the position and direction of the imaging locus
      //  nearest the given groundPt (x,y,z in ECEF meters).
      //
      //  Note that there are two opposite directions possible.  Both are
      //  valid, so either can be returned; the calling application can convert
      //  to the other as necessary.
      //
      //  Iterative algorithms will use desiredPrecision, in meters, as the
      //  convergence criterion for the locus position, otherwise it will be
      //  ignored.
      //
      //  If a non-NULL achievedPrecision argument is received, it will be
      //  populated with the actual precision, in meters, achieved by iterative
      //  algorithms and 0.0 for deterministic algorithms.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   virtual EcefLocus imageToRemoteImagingLocus(
      const ImageCoord& imagePt,
      double desiredPrecision = 0.001,
      double* achievedPrecision = NULL,
      WarningList* warnings = NULL) const = 0;
      //> This method, for the given imagePt (line, sample in full image space
      //  pixels), returns the position and direction of the imaging locus
      //  at the sensor.
      //
      //  Note that there are two opposite directions possible.  Both are
      //  valid, so either can be returned; the calling application can convert
      //  to the other as necessary.
      //
      //  Iterative algorithms will use desiredPrecision, in meters, as the
      //  convergence criterion for the locus position, otherwise it will be
      //  ignored.
      //
      //  If a non-NULL achievedPrecision argument is received, it will be
      //  populated with the actual precision, in meters, achieved by iterative
      //  algorithms and 0.0 for deterministic algorithms.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //
      //  Notes:
      //
      //  The remote imaging locus is only well-defined for optical sensors.
      //  It is undefined for SAR sensors and might not be available for
      //  polynomial and other non-physical models.  The
      //  imageToProximateImagingLocus method should be used instead where
      //  possible.
      //<

   //---
   // Monoscopic Mensuration
   //---
   virtual ImageCoord getImageStart() const = 0;
      //> This method returns the starting coordinate (line, sample in full
      //  image space pixels) for the imaging operation.  Typically (0,0).
      //<

   virtual ImageVector getImageSize() const = 0;
      //> This method returns the number of lines and samples in full image
      //  space pixels for the imaging operation.
      //
      //  Note that the model might not be valid over the entire imaging
      //  operation.  Use getValidImageRange() to get the valid range of image
      //  coordinates.
      //<

   virtual std::pair<ImageCoord,ImageCoord> getValidImageRange() const = 0;
      //> This method returns the minimum and maximum image coordinates
      //  (line, sample in full image space pixels), respectively, over which
      //  the current model is valid.  The image coordinates define opposite
      //  corners of a rectangle whose sides are parallel to the line and
      //  sample axes.
      //
      //  The valid image range does not always match the full image
      //  coverage as returned by the getImageStart and getImageSize methods.
      //
      //  Used in conjunction with the getValidHeightRange method, it is
      //  possible to determine the full range of ground coordinates over which
      //  the model is valid.
      //<

   virtual std::pair<double,double> getValidHeightRange() const = 0;
      //> This method returns the minimum and maximum heights (in meters
      //  relative to WGS-84 ellipsoid), respectively, over which the model is
      //  valid.  For example, a model for an airborne platform might not be
      //  designed to return valid coordinates for heights above the aircraft.
      //
      //  If there are no limits defined for the model, (-99999.0,99999.0)
      //  will be returned.
      //<

   virtual EcefVector getIlluminationDirection(const EcefCoord& groundPt) const = 0;
      //> This method returns a vector defining the direction of
      //  illumination at the given groundPt (x,y,z in ECEF meters).
      //  Note that there are two opposite directions possible.  Both are
      //  valid, so either can be returned; the calling application can convert
      //  to the other as necessary.
      //<

   //---
   // Time and Trajectory
   //---
   virtual double getImageTime(const ImageCoord& imagePt) const = 0;
      //> This method returns the time in seconds at which the pixel at the
      //  given imagePt (line, sample in full image space pixels) was captured
      //
      //  The time provided is relative to the reference date and time given
      //  by the Model::getReferenceDateAndTime method.
      //<

   virtual EcefCoord getSensorPosition(const ImageCoord& imagePt) const = 0;
      //> This method returns the position of the physical sensor
      // (x,y,z in ECEF meters) when the pixel at the given imagePt
      // (line, sample in full image space pixels) was captured.
      //
      // A csm::Error will be thrown if the sensor position is not available.
      //<

   virtual EcefCoord getSensorPosition(double time) const = 0;
      //> This method returns the position of the physical sensor
      //  (x,y,z meters ECEF) at the given time relative to the reference date
      //  and time given by the Model::getReferenceDateAndTime method.
      //<

   virtual EcefVector getSensorVelocity(const ImageCoord& imagePt) const = 0;
      //> This method returns the velocity of the physical sensor
      // (x,y,z in ECEF meters per second) when the pixel at the given imagePt
      // (line, sample in full image space pixels) was captured.
      //<

   virtual EcefVector getSensorVelocity(double time) const = 0;
      //> This method returns the velocity of the physical sensor
      //  (x,y,z in ECEF meters per second ) at the given time relative to the
      //  reference date and time given by the Model::getReferenceDateAndTime
      //  method.
      //<

   //---
   // Uncertainty Propagation
   //---
   typedef std::pair<double,double> SensorPartials;
      //> This type is used to hold the partial derivatives of line and
      //  sample, respectively, with respect to a model parameter.
      //  The units are pixels per the model parameter units.
      //<

   virtual SensorPartials computeSensorPartials(
                int index,
                const EcefCoord& groundPt,
                double desiredPrecision   = 0.001,
                double* achievedPrecision = NULL,
                WarningList* warnings     = NULL) const = 0;
      //> This is one of two overloaded methods.  This method takes only
      //  the necessary inputs.  Some effieciency can be obtained by using the
      //  other method.  Even more efficiency can be obtained by using the
      //  computeAllSensorPartials method.
      //
      //  This method returns the partial derivatives of line and sample
      //  (in pixels per the applicable model parameter units), respectively,
      //  with respect to the model parameter given by index at the given
      //  groundPt (x,y,z in ECEF meters).
      //
      //  Derived model implementations may wish to implement this method by
      //  calling the groundToImage method and passing the resulting image
      //  coordinate to the other computeSensorPartials method.
      //
      //  If a non-NULL achievedPrecision argument is received, it will be
      //  populated with the highest actual precision, in meters, achieved by
      //  iterative algorithms and 0.0 for deterministic algorithms.
      //
      //  If a non-NULL achievedPrecision argument is received, it will be
      //  populated with the actual precision, in meters, achieved by iterative
      //  algorithms and 0.0 for deterministic algorithms.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   virtual SensorPartials computeSensorPartials(
                int index,
                const ImageCoord& imagePt,
                const EcefCoord& groundPt,
                double desiredPrecision   = 0.001,
                double* achievedPrecision = NULL,
                WarningList* warnings     = NULL) const = 0;
      //> This is one of two overloaded methods.  This method takes
      //  an input image coordinate for efficiency.  Even more efficiency can
      //  be obtained by using the computeAllSensorPartials method.
      //
      //  This method returns the partial derivatives of line and sample
      //  (in pixels per the applicable model parameter units), respectively,
      //  with respect to the model parameter given by index at the given
      //  groundPt (x,y,z in ECEF meters).
      //
      //  The imagePt, corresponding to the groundPt, is given so that it does
      //  not need to be computed by the method.  Results are unpredictable if
      //  the imagePt provided does not correspond to the result of calling the
      //  groundToImage method with the given groundPt.
      //
      //  Implementations with iterative algorithms (typically ground-to-image
      //  calls) will use desiredPrecision, in meters, as the convergence
      //  criterion, otherwise it will be ignored.
      //
      //  If a non-NULL achievedPrecision argument is received, it will be
      //  populated with the highest actual precision, in meters, achieved by
      //  iterative algorithms and 0.0 for deterministic algorithms.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   virtual std::vector<SensorPartials> computeAllSensorPartials(
                const EcefCoord& groundPt,
                param::Set pSet           = param::VALID,
                double desiredPrecision   = 0.001,
                double* achievedPrecision = NULL,
                WarningList* warnings     = NULL) const;
      //> This is one of two overloaded methods.  This method takes only
      //  the necessary inputs.  Some effieciency can be obtained by using the
      //  other method.
      //
      //  This method returns the partial derivatives of line and sample
      //  (in pixels per the applicable model parameter units), respectively,
      //  with respect to to each of the desired model parameters at the given
      //  groundPt (x,y,z in ECEF meters).  Desired model parameters are
      //  indicated by the given pSet.
      //
      //  Implementations with iterative algorithms (typically ground-to-image
      //  calls) will use desiredPrecision, in meters, as the convergence
      //  criterion, otherwise it will be ignored.
      //
      //  If a non-NULL achievedPrecision argument is received, it will be
      //  populated with the highest actual precision, in meters, achieved by
      //  iterative algorithms and 0.0 for deterministic algorithms.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //
      //  The value returned is a vector of pairs with line and sample partials
      //  for one model parameter in each pair.  The indices of the
      //  corresponding model parameters can be found by calling the
      //  getParameterSetIndices method for the given pSet.
      //
      //  Derived models may wish to implement this directly for efficiency,
      //  but an implementation is provided here that calls the
      //  computeSensorPartials method for each desired parameter index.
      //<

   virtual std::vector<SensorPartials> computeAllSensorPartials(
                const ImageCoord& imagePt,
                const EcefCoord& groundPt,
                param::Set pSet           = param::VALID,
                double desiredPrecision   = 0.001,
                double* achievedPrecision = NULL,
                WarningList* warnings     = NULL) const;
      //> This is one of two overloaded methods.  This method takes
      //  an input image coordinate for efficiency.
      //
      //  This method returns the partial derivatives of line and sample
      //  (in pixels per the applicable model parameter units), respectively,
      //  with respect to to each of the desired model parameters at the given
      //  groundPt (x,y,z in ECEF meters).  Desired model parameters are
      //  indicated by the given pSet.
      //
      //  The imagePt, corresponding to the groundPt, is given so that it does
      //  not need to be computed by the method.  Results are unpredictable if
      //  the imagePt provided does not correspond to the result of calling the
      //  groundToImage method with the given groundPt.
      //
      //  Implementations with iterative algorithms (typically ground-to-image
      //  calls) will use desiredPrecision, in meters, as the convergence
      //  criterion, otherwise it will be ignored.
      //
      //  If a non-NULL achievedPrecision argument is received, it will be
      //  populated with the highest actual precision, in meters, achieved by
      //  iterative algorithms and 0.0 for deterministic algorithms.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //
      //  The value returned is a vector of pairs with line and sample partials
      //  for one model parameter in each pair.  The indices of the
      //  corresponding model parameters can be found by calling the
      //  getParameterSetIndices method for the given pSet.
      //
      //  Derived models may wish to implement this directly for efficiency,
      //  but an implementation is provided here that calls the
      //  computeSensorPartials method for each desired parameter index.
      //<

   virtual std::vector<double> computeGroundPartials(const EcefCoord& groundPt) const = 0;
      //> This method returns the partial derivatives of line and sample
      //  (in pixels per meter) with respect to the given groundPt
      //  (x,y,z in ECEF meters).
      //
      //  The value returned is a vector with six elements as follows:
      //
      //-  [0] = line wrt x
      //-  [1] = line wrt y
      //-  [2] = line wrt z
      //-  [3] = sample wrt x
      //-  [4] = sample wrt y
      //-  [5] = sample wrt z
      //<

   virtual const CorrelationModel& getCorrelationModel() const = 0;
      //> This method returns a reference to a CorrelationModel.
      //  The CorrelationModel is used to determine the correlation between
      //  the model parameters of different models of the same type.
      //  These correlations are used to establish the "a priori" cross-covariance
      //  between images. While some applications (such as generation of a 
      //  replacement sensor model) may wish to call this method directly,
      //  it is reccommended that the inherited method 
      //  GeometricModel::getCrossCovarianceMatrix() be called instead.
      //<

   inline std::vector<double> getUnmodeledError(const ImageCoord& imagePt) const
   { return getUnmodeledCrossCovariance(imagePt, imagePt); }

   /**
    * Returns six::ErrorStatistics::Unmodeled values, if any.
    *
    * \return A vector of zero, three or seven elements:
    * [0] = Xrow
    * [1] = Ycol
    * [2] = XrowYcol
    * [3] = Xrow.CorrCoefZero
    * [4] = Xrow.DecorrRate
    * [5] = Ycol.CorrCoefZero
    * [6] = Ycol.DecorrRate
    */
   virtual std::vector<double> getSIXUnmodeledError() const = 0;

      //> This method returns the 2x2 line and sample covariance (in pixels
      //  squared) at the given imagePt for any model error not accounted for
      //  by the model parameters.
      //
      //  The value returned is a vector of four elements as follows:
      //
      //-  [0] = line variance
      //-  [1] = line/sample covariance
      //-  [2] = sample/line covariance
      //-  [3] = sample variance
      //<

   virtual std::vector<double> getUnmodeledCrossCovariance(
                const ImageCoord& pt1,
                const ImageCoord& pt2) const = 0;
      //> This method returns the 2x2 line and sample cross covariance
      //  (in pixels squared) between the given imagePt1 and imagePt2 for any
      //  model error not accounted for by the model parameters.  The error is
      //  reported as the four terms of a 2x2 matrix, returned as a 4 element
      //  vector.
      //<
};

} // namespace csm

#endif
