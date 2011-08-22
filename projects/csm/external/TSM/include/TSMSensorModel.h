//#############################################################################
//
//    FILENAME:          TSMSensorModel.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for abstract base class that is to provide a common interface from
//    which all Tactical Sensor Model (TSM) plugin models will inherit.
//
//    LIMITATIONS:       None
// 
//                       Date          Author   Comment    
//    SOFTWARE HISTORY:  27-Jun-2003   LMT      Initial version.
//                       01-Jul-2003   LMT      Remove constants, error/warning
//                                              and make methods pure virtual.
//                                              CharType enum.
//                       31-Jul-2003   LMT      Change calls with a "&" to a "*",
//                                              combined CharType with ParamType
//                                              to create Param_CharType, //reordered
//                                              methods to match API order, added
//                                              systematic error methods.
//                       06-Aug-2003   LMT      Removed all Characteristic calls.
//                       08-Oct 2003   LMT      Added getImageSize calls
//                       06-Feb-2004   KRW      Incorporates changes approved by
//                                              January and February 2004
//                                              configuration control board.
//                       30-Jul-2004   PW       Initail API 3.1 version
//                       01-Nov-2004   PW       October 2004 CCB
//    NOTES:
//
//#############################################################################
#ifdef _WIN32
#pragma warning( disable : 4290 )
#endif

#ifndef __TSMSENSORMODEL_H
#define __TSMSENSORMODEL_H


#include "TSMMisc.h"
#include "TSMWarning.h"
#include "TSMError.h"
#include "TSMCovarianceModel.h"

class TSM_EXPORT_API TSMSensorModel
{
public:

  
 
   //--------------------------------------------------------------
   // Constructors/Destructor
   //--------------------------------------------------------------

   TSMSensorModel() { }

   virtual ~TSMSensorModel() { }

   //--------------------------------------------------------------
   // Modifier
   //--------------------------------------------------------------
   
   //---
   // Core Photogrammetry
   //---

   virtual TSMWarning* groundToImage(
                   const double& x, 
                   const double& y, 
                   const double& z, 
                   double& line, 
                   double& sample, 
                   double& achieved_precision, 
                   const double& desired_precision = 0.001) 
      throw (TSMError) = 0;

      //> The groundToImage() method converts x, y and z (meters) in ground
      //  space (ECEF) to line and sample (pixels) in image space. 
      //<

   virtual TSMWarning* groundToImage(
                   const double& x, 
                   const double& y, 
                   const double& z, 
                   const double groundCovariance[9], 
                   double& line, 
                   double& sample, 
                   double imageCovariance[4],
                   double& achieved_precision, 
                   const double& desired_precision = 0.001)
      throw (TSMError) = 0;

      //> This method converts a given ground point into line and sample 
      //  (pixels) in image space and returns accuracy information 
      //  associated with the image and ground coordinates.
      //<
   
    virtual TSMWarning* imageToGround(
                   const double& line, 
                   const double& sample, 
                   const double& height,
                   double& x, 
                   double& y, 
                   double& z,
                   double& achieved_precision, 
                   const double& desired_precision = 0.001) 
      throw (TSMError) = 0;
       
     
      //> This method converts a given line and sample (pixels) in image 
      //  space to a ground point. 
      //<

   virtual TSMWarning* imageToGround(
                   const double& line, 
                   const double& sample, 
                   const double imageCovariance[4],
                   const double& height,
                   const double& heightVariance,
                   double& x, 
                   double& y, 
                   double& z,
                   double groundCovariance[9],
                   double& achieved_precision, 
                   const double& desired_precision = 0.001) 
      throw (TSMError) = 0;

      //> This method converts a given line and sample (pixels) in //image space
      //  to a ground point and returns accuracy information associated with
      //  the image and ground coordinates. 
      //<

   virtual TSMWarning* imageToProximateImagingLocus(
                   const double& line, 
                   const double& sample, 
                   const double& x, 
                   const double& y, 
                   const double& z, 
                   double locus[6], 
                   double& achieved_precision, 
                   const double& desired_precision = 0.001) 
      throw (TSMError) = 0;

      //> The imageToProximateImagingLocus() method computes a proximate
      //  imaging locus, a vector approximation of the imaging locus for the
      //  given line and sample nearest the given x, y and z or at the given
      //  height. The precision of this calculation refers to the locus's 
      //  origin and does not refer to the locus's orientation.
      //<

   virtual TSMWarning* imageToRemoteImagingLocus(
                   const double& line, 
                   const double& sample, 
                   double locus[6], 
                   double& achieved_precision, 
                   const double& desired_precision = 0.001) 
      throw (TSMError) = 0;

      //> The imageToRemoteImagingLocus() method computes locus, a vector
      //  approximation of the imaging locus for the given line and sample.
      //  The precision of this calculation refers only to the origin of the
      //  locus vector and does not refer to the locus's orientation. For an
      //  explanation of the remote imaging locus, see the section at the 
      //  beginning of this document.
      //<

   //---
   // Uncertainty Propagation
   //---

   virtual TSMWarning* computeGroundPartials(
                   const double& x, 
                   const double& y, 
                   const double& z, 
                   double partials[6])
      throw (TSMError)  = 0;

      //> The computeGroundPartials method calculates the partial
      //  derivatives (partials) of image position (both line and sample)
      //  with respect to ground coordinates at the given ground 
      //  position x, y, z.
      //  Upon successful completion, computeGroundPartials() produces the
      //  partial derivatives as follows:
      //
      //  partials [0] = line wrt x
      //  partials [1] = line wrt y
      //  partials [2] = line wrt z
      //  partials [3] = sample wrt x
      //  partials [4] = sample wrt y
      //  partials [5] = sample wrt z
      //<
  
   virtual TSMWarning* computeSensorPartials(
                   const int& index, 
                   const double& x, 
                   const double& y, 
                   const double& z, 
                   double& line_partial, 
                   double& sample_partial, 
                   double& achieved_precision, 
                   const double& desired_precision = 0.001)
      throw (TSMError) = 0;

   virtual TSMWarning* computeSensorPartials(
                   const int& index, 
                   const double& line, 
                   const double& sample, 
                   const double& x, 
                   const double& y, 
                   const double& z, 
                   double& line_partial, 
                   double& sample_partial, 
                   double& achieved_precision, 
                   const double& desired_precision = 0.001)
      throw (TSMError) = 0;

      //> The computeSensorPartials() method calculates the partial 
      //  derivatives of image position (both line and sample) with 
      //  respect to the given sensor parameter (index) at the given 
      //  ground position.
      //  Two versions of the method are provided. The first method,
      //  computeSensorPartials(), takes in only necessary information. 
      //  It performs groundToImage() on the ground coordinate and then 
      //  calls the second form of the method with the obtained line 
      //  and sample. If the calling function has already performed 
      //  groundToImage with the ground coordinate, it may call the second
      //  method directly since it may be significantly faster than the
      //  first. The results are unpredictable if the line and sample 
      //  provided do not correspond to the result of calling // //groundToImage()
      //  with the given ground position (x, y, and z).
      //<

   virtual TSMWarning* getCurrentParameterCovariance(
                   const int& index1, 
                   const int& index2,
		   double& covariance) 
      throw (TSMError) = 0;
             
      //> The getCurrentParameterCovariance() method 
      //  returns the covariance of the specified parameter pair 
      //  (index1, index2). The variance of the given parameter can be 
      //  obtained by using the same value for index1 and index2.
      //<

   virtual TSMWarning*  setCurrentParameterCovariance(
                   const int& index1, 
                   const int& index2, 
                   const double& covariance) 
      throw (TSMError) = 0;

      //> The setCurrentParameterCovariance() method is
      //  used to set the covariance value of the specified parameter pair.
      //<

   virtual TSMWarning* setOriginalParameterCovariance(
                   const int& index1, 
                   const int& index2, 
                   const double& covariance) 
      throw (TSMError) = 0;

   virtual TSMWarning* getOriginalParameterCovariance(
		   const int& index1, 
		   const int& index2,
		   double& covariance)
      throw  (TSMError) = 0;

      //> The first form of originalParameterCovariance() method sets
      //  the covariance of the specified parameter pair (index1, index2).
      //  The variance of the given parameter can be set using the same 
      //  value for index1 and index2. 
      //  The second form of originalParameterCovariance() method gets
      //  the covariance of the specified parameter pair (index1, index2). 
      //  The variance of the given parameter can be obtained using the 
      //  same value for index1 and index2.
      //<

   //---
   // Time and Trajectory
   //---

   virtual TSMWarning* getTrajectoryIdentifier(
                   std::string &trajectoryId) 
      throw (TSMError) = 0;

      //> This method returns a unique identifer to indicate which 
      //  trajectory was used to acquire the image. This ID is unique for
      //  each sensor type on an individual path.
      //<

   virtual TSMWarning* getReferenceDateAndTime(
                   std::string &date_and_time) 
      throw (TSMError) = 0;

      //> This method returns the time in seconds at which the specified
      //  pixel was imaged. The time provide is relative to the reference
      //  date and time given by the getReferenceDateAndTime() method and
      //  can be used to represent time offsets within the trajectory 
      //  associated with the given image.
      //<

   virtual TSMWarning* getImageTime(
                   const double& line, 
                   const double& sample, 
                   double& time) 
      throw (TSMError) = 0;

      //> The getImageTime() method returns the time in seconds at which
      //  the pixel specified by line and sample was imaged. The time
      //  provided is relative to the reference date and time given by
      //  getReferenceDateAndTime.
      //<

   virtual TSMWarning* getSensorPosition(
                   const double& line,
                   const double& sample, 
                   double& x, 
                   double& y, 
                   double& z) 
      throw (TSMError) = 0;

      //> The getSensorPosition() method returns the position of
      //  the physical sensor at the given position in the image.
      //<

   virtual TSMWarning* getSensorPosition(
                   const double& time,
                   double& x, 
                   double& y, 
                   double& z) 
      throw (TSMError) = 0;

      //> The getSensorPosition() method returns the position of
      //  the physical sensor at the given time of imaging.
      //<

   virtual TSMWarning* getSensorVelocity(
                   const double& line,
                   const double& sample, 
                   double& vx, 
                   double& vy, 
                   double &vz) 
      throw (TSMError) = 0;

      //> The getSensorVelocity() method returns the velocity
      //  of the physical sensor at the given position in the image.
      //<

   virtual TSMWarning* getSensorVelocity(
                   const double& time,
                   double& vx, 
                   double& vy, 
                   double &vz) 
      throw (TSMError) = 0;

      //> The getSensorVelocity() method returns the velocity
      //  of the physical sensor at the given time of imaging.
      //<

   //---
   // Sensor Model Parameters
   //---

   virtual TSMWarning* setCurrentParameterValue(
                   const int& index, 
                   const double& value)
      throw (TSMError) = 0;

      //> The setCurrentParameterValue() is used to set the 
      //  value of the adjustable parameter indicated by index.
      //<
       
   virtual TSMWarning* getCurrentParameterValue(
                   const int& index,
	           double& value)
      throw (TSMError) = 0;
      
      //> The getCurrentParameterValue()returns the value
      //  of the adjustable parameter given by index.
      //<

   virtual TSMWarning* getParameterName(
                   const int& index, 
                   std::string& name) 
      throw (TSMError) = 0;

      //> This method returns the name for the sensor model parameter
      //  indicated by the given index.
      //<

   virtual TSMWarning* getNumParameters(
                   int& numParams)
      throw (TSMError) = 0;

      //> This method returns the number of sensor model parameters.
      //<

   virtual TSMWarning* setOriginalParameterValue(
                   const int& index,
                   const double& value)
      throw (TSMError) = 0;

      //> The setOriginalParameterValue() method is 
      //  used to set the original parameter value of the indexed 
      //  parameter.
      //<
   
   virtual TSMWarning* getOriginalParameterValue(
                   const int& index,
		   double& value)
      throw (TSMError) = 0;
      
      //> The getOriginalParameterValue() method 
      //  returns the value of the adjustable parameter given by 
      //  index.
      //<

   virtual TSMWarning*  getParameterType(
                   const int& index,
		   TSMMisc::Param_CharType &pType) 
      throw (TSMError) = 0; 

      //> The getParameterType() method returns the type of the parameter 
      //  given by index. 
      //<

   virtual TSMWarning*  setParameterType(
                   const int& index,
		   const TSMMisc::Param_CharType &pType) 
      throw (TSMError) = 0; 

      //> The getParameterType() method sets the type of the parameter 
      //  for the given by index. 
      //<

   //---
   // Sensor Model Information
   //---

   virtual TSMWarning* getPedigree(
               	   std::string &pedigree) 
      throw (TSMError) = 0;

      //> The getPedigree() method returns a character std::string that
      //  identifies the sensor, the model type, its mode of acquisition
      //  and processing path. For example, an image that could produce
      //  either an optical sensor model or a cubic rational polynomial
      //  model would produce different pedigrees for each case.
      //<

   virtual TSMWarning* getImageIdentifier(
                   std::string &imageId) 
      throw (TSMError) = 0;

      //> This method returns the unique identifier to indicate the imaging 
      //  operation associated with this sensor model.
      //<

   virtual TSMWarning* setImageIdentifier(
                   const std::string &imageId) 
      throw (TSMError) = 0;

      //> This method sets the unique identifier for the image to which the
      //  sensor model pertains.
      //<

   virtual TSMWarning* getSensorIdentifier(
	           std::string &sensorId) 
      throw (TSMError) = 0;

      //> The getSensorIdentifier() method returns sensorId to indicate
      //  which sensor was used to acquire the image. This sensorId is 
      //  meant to uniquely identify the sensor used to make the image.
      //<

   virtual TSMWarning* getPlatformIdentifier(
                   std::string &platformId) 
      throw (TSMError) = 0;

      //> The getPlatformIdentifier() method returns platformId to indicate
      //  which platform was used to acquire the image. This platformId
      // is meant to uniquely identify the platform used to collect the // //image.
      //<

   virtual TSMWarning* setReferencePoint(
                   const double &x, 
                   const double &y, 
                   const double &z) 
      throw (TSMError) = 0;

      //> This method returns the ground point indicating the general 
      // location
      // of the image.
      //<

   virtual TSMWarning* getReferencePoint(
                   double &x, 
                   double &y, 
                   double &z) 
      throw (TSMError) = 0;

      //> This method sets the ground point indicating the general location
      //  of the image.
      //<
 
   virtual TSMWarning* getSensorModelName(
                   std::string &name) 
      throw (TSMError) = 0;


      //> This method returns a string identifying the name of the senor model.
      //<

   virtual TSMWarning* getImageSize(
                   int& num_lines,
                   int& num_samples) 
      throw (TSMError) = 0;

   //> This method returns the number of lines and samples in the imaging
   //  operation.
   //<

   //---
   // Sensor Model State
   //---

   virtual TSMWarning* getSensorModelState(
                   std::string& state) 
      throw (TSMError) = 0;

       
      //> This method returns the current state of the model in an 
      //  intermediate form. This intermediate form can then processed,
      //  for example, by saving to file so that this model 
      //  can be instantiated at a later date. The derived SensorModel 
      //  is responsible for saving all information needed to restore 
      //  itself to its current state from this intermediate form.
      //  A NULL pointer is returned if it is not possible to save the
      //  current state.
      //<

   //---
   //  Monoscopic Mensuration
   //---

   virtual TSMWarning* getValidAltitudeRange(
                   double& minAltitude,
                   double& maxAltitude) 
      throw (TSMError) = 0;

      //> The validAltitudeRange() method returns the minimum and maximum
      //  altitudes that describe the range of validity of the model. For
      // example, the model may not be valid at altitudes above the altitude
      //  of the sensor for physical models. 
      //<

   virtual TSMWarning* getIlluminationDirection(
                   const double&  x, 
                   const double&  y, 
                   const double&  z, 
                   double& direction_x,
                   double& direction_y,
                   double& direction_z) 
      throw (TSMError) = 0;

      //> The getIlluminationDirection() method calculates the direction of 
      //  illumination at the given ground position x, y, z.
      //<

   //---
   //  Error Correction
   //---

   virtual TSMWarning* getNumSystematicErrorCorrections(
                   int& numSec)
      throw (TSMError) = 0;
 
      //> The numSystematicErrorCorrections() method returns the number
      //  of systematic error corrections defined for the sensor model.
      //<

   virtual TSMWarning* getSystematicErrorCorrectionName(
                   const int& index, 
                   std::string &name) 
      throw (TSMError) = 0;

      //> This method returns the name for the sensor model parameter
      //  indicated by the given index.
      //<

   virtual TSMWarning* setCurrentSystematicErrorCorrectionSwitch(
                   const int& index, 
                   const bool &value,
                   const TSMMisc::Param_CharType& parameterType) 
      throw (TSMError) = 0;

      //> The setCurrentSystematicErrorCorrectionSwitch() is 
      //  used to set the switch of the systematic error correction 
      //  indicated by index.
      //<

   virtual TSMWarning* getCurrentSystematicErrorCorrectionSwitch(
                   const int& index,
                   bool &value) 
      throw (TSMError) = 0;

      //> The getCurrentSystematicErrorCorrectionSwitch()
      //  returns the value of the systematic error correction switch 
      //  given by index.
      //<

   virtual TSMWarning* getCovarianceModel(
                   tsm_CovarianceModel*& covModel)
      throw (TSMError) = 0;

      //> The getCovarianceModel() function returns covariance model
      //  information. This data supplies the data to compute cross
      //  covariance between images. Images may be correlated because
      //  they are taken by the same sensor or from sensors on the
      //  same platform. Images may also be correlated due to post
      //  processing of the sensor models.
      //<

   virtual TSMWarning* getUnmodeledError(
                   const double line,
                   const double sample,
                   double covariance[4])
      throw (TSMError) = 0;

      //> The getUnmodeledError() function gives a sensor specific 
      //  error for the given input image point. The error is reported 
      //  as the four terms of a 2x2 covariance mensuration error 
      //  matrix. This error term is meant to map error terms that are
      //  not modeled in the sensor model to image space for inclusion 
      //  in error propagation. The extra error is added to the 
      //  mensuration error that may already be in the matrix.
      //<

   virtual TSMWarning* getUnmodeledCrossCovariance(
                   const double pt1Line,
                   const double pt1Sample,
                   const double pt2Line,
                   const double pt2Sample,
                   double crossCovariance[4])
      throw (TSMError) = 0;

      //> The getUnmodeledCrossCovariance function gives the cross 
      //  covariance for unmodeled error between two image points on 
      //  the same image. The error is reported as the four terms of 
      //  a 2x2 matrix. The unmodeled cross covariance is added to 
      //  any values that may already be in the cross covariance matrix.
      //<

   virtual TSMWarning* getCollectionIdentifier(
                   std::string &collectionId) 
      throw (TSMError) = 0;

      //> This method returns a unique identifer that uniquely identifies 
      //  a collection activity by a sensor platform. This ID will vary
      //  depending on the sensor type and platform.
      //<

   virtual TSMWarning* isParameterShareable(
                   const int& index,
                   bool& shareable) 
      throw (TSMError) = 0;

      //> This method returns a flag to indicate whether or not a sensor 
      //  model parameter adjustments are shareable across images for the
      //  sensor model adjustable parameter referenced by index.
      //<

   virtual TSMWarning* getParameterSharingCriteria(
                   const int& index,
                   bool& requireModelNameMatch, 
                   bool& requireSensorIDMatch, 
                   bool& requirePlatformIDMatch, 
                   bool& requireCollectionIDMatch, 
                   bool& requireTrajectoryIDMatch, 
                   bool& requireDateTimeMatch, 
                   double& allowableTimeDelta) 
      throw (TSMError) = 0;

      //> This method returns characteristics to indicate how
      //  the sensor model adjustable parameter referenced by index
      //  may be shareable accross images.
      //<

};

#endif
