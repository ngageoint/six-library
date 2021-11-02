//#############################################################################
//
//    FILENAME:          csm.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for constants and other definitions used in the CSM API.
//
//    LIMITATIONS:       None
//
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     01-Jul-2003   LMT      Initial version.
//     02-Mar-2012   SCM      Added csm namespace.
//     30-Oct-2012   SCM      Changed covariances from std::vectors to arrays.
//                            Made the covariances public.
//     30-Oct-2012   SCM      Renamed to csm.h
//     31-Oct-2012   SCM      Moved common sensor type and mode defines here.
//     29-Nov-2012   JPK      Added Parameter and SharingCriteria structs and
//                            ParamSet struct.
//     06-Dec-2012   JPK      Renamed ParamSet and ParamType to Set and Type
//                            and put them in new namespace "param".
//                            Implemented additional constructors for
//                            ImageCoordCovar and EcefCoordCovar and added
//                            new struct EcefLocus.
//     17-Dec-2012   BAH      Documentation updates.
//     12-Feb-2013   JPK      Renamed param::EXACT to param::FIXED
//     10-Sep-2013   SCM      Added std namespace to memory function calls.
//     22-Feb-2018   JPK      Updated version to 3.0.3
//
//    NOTES:
//
//#############################################################################

#ifndef __CSM_CSM_H
#define __CSM_CSM_H

#include <vector>
#include <string>
#include <cstring>

#ifdef _WIN32
# ifdef CSM_LIBRARY
#  define CSM_EXPORT_API __declspec(dllexport)
# else
#  define CSM_EXPORT_API __declspec(dllimport)
# endif
#elif LINUX_BUILD
# define CSM_EXPORT_API __attribute__ ((visibility("default")))
#else
#  define CSM_EXPORT_API
#endif

// The getCsmVersion method should use CURRENT_CSM_VERSION to
// return the CSM API version that the sensor model/plugin was written to.
#define CURRENT_CSM_VERSION csm::Version(3, 0, 3);

// Common definition for an unknown name, type, node,etc.
#define CSM_UNKNOWN "UNKNOWN"

// Common sensor types returned from the csm::Model::getSensorType method.
#define CSM_SENSOR_TYPE_UNKNOWN       CSM_UNKNOWN
#define CSM_SENSOR_TYPE_EO            "EO"
#define CSM_SENSOR_TYPE_IR            "IR"
#define CSM_SENSOR_TYPE_MWIR          "MWIR"
#define CSM_SENSOR_TYPE_LWIR          "LWIR"
#define CSM_SENSOR_TYPE_SAR           "SAR"
#define CSM_SENSOR_TYPE_EOIRSC        "EO_IR_SPECIAL_CASE"

// Common sensor modes returned from the csm::Model::getSensorMode method.
#define CSM_SENSOR_MODE_UNKNOWN       CSM_UNKNOWN
#define CSM_SENSOR_MODE_FRAME         "FRAME"
#define CSM_SENSOR_MODE_PULSE         "PULSE"
#define CSM_SENSOR_MODE_PB            "PUSHBROOM"
#define CSM_SENSOR_MODE_WB            "WHISKBROOM"
#define CSM_SENSOR_MODE_SPOT          "SPOT"
#define CSM_SENSOR_MODE_STRIP         "STRIP"
#define CSM_SENSOR_MODE_SCAN          "SCAN"
#define CSM_SENSOR_MODE_VIDEO         "VIDEO"
#define CSM_SENSOR_MODE_BODY_POINTING "BODY_POINTING"

namespace csm
{
   namespace param
   {
      enum Type
      //>
      // This enumeration lists the possible model parameter or characteristic
      // types as follows.
      //
      //-  NONE       - Parameter value has not yet been initialized.
      //-  FICTITIOUS - Parameter value has been calculated by resection
      //-               or other means.
      //-  REAL       - Parameter value has been measured or read from
      //-               support data.
      //-  FIXED      - Parameter value has been specified and is not
      //-               to be adjusted, but may contribute to error
      //-               propagation.
      //
      {
         NONE = 0 ,
         FICTITIOUS,
         REAL,
         FIXED
      };
      //<
      enum Set
      //>
      // This enumeration lists the set of model parameters that a user may be
      // interested in exploiting.  Membership in one of these sets is
      // determined by model parameter type.
      //
      //-  VALID          - Parameters of type NONE are excluded.
      //-                   All others are included,
      //-  ADJUSTABLE     - Only REAL or FICTICIOUS parameters are included.
      //-  NON_ADJUSTABLE - Only FIXED parameters are included.
      //
      {
         VALID = 0,
         ADJUSTABLE,
         NON_ADJUSTABLE
      };
      //<
   }

   //***
   // STRUCT: SharingCriteria
   //> This structure stores information regarding whether or not a model
   //  parameter might be "shared" between models of the same type, based on
   //  common characteristics.
   //<
   //***
   struct SharingCriteria
   {
   public:
      bool matchesName;
        //> Requires that the models have the same model name as given by
        //  the Model::getModelName method.  Will almost always be set to true.
        //<

      bool matchesSensorID;
        //> Requires that the models have the same sensor ID as given by
        //  the Model::getSensorIdentifier method.
        //<

      bool matchesPlatformID;
        //> Requires that the models have the same platform ID as given by
        //  the Model::getPlatformIdentifier method.
        //<

      bool matchesCollectionID;
        //> Requires that the models have the same collection ID as given by
        //  the Model::getCollectionIdentifier method.
        //<

      bool matchesTrajectoryID;
        //> Requires that the models have the same trajectory ID as given by
        //  the Model::getTrajectoryIdentifier method.
        //<

      bool matchesDateTime;
        //> Requires that the models' imaging times must be within a certain
        //  time delta.  It is typically sufficient to compare the times at
        //  the start of the image.
        //<

      double maxTimeDelta;
        //> Maximum time separation, in seconds, for a model parameter to be
        //  shared when matchesDateTime is true.
        //<

      SharingCriteria()
         :
            matchesName         (false),
            matchesSensorID     (false),
            matchesPlatformID   (false),
            matchesCollectionID (false),
            matchesTrajectoryID (false),
            matchesDateTime     (false),
            maxTimeDelta        (0.0)
      {}

      SharingCriteria(bool   byName,
                      bool   bySensorID,
                      bool   byPlatformID,
                      bool   byCollectionID,
                      bool   byTrajectoryID,
                      bool   byDateTime,
                      double maxDelta)
         :
           matchesName         (byName),
           matchesSensorID     (bySensorID),
           matchesPlatformID   (byPlatformID),
           matchesCollectionID (byCollectionID),
           matchesTrajectoryID (byTrajectoryID),
           matchesDateTime     (byDateTime),
           maxTimeDelta        (maxDelta)
      {}
   };

   //***
   // STRUCT: ImageCoord
   //> This structure represents a two-dimensional image coordinate
   //  (line, sample in pixels).
   //  It typically represents an absolute coordinate.
   //<
   //***
   struct ImageCoord
   {
   public:
      double line;
      double samp;

      ImageCoord() : line(0.0), samp(0.0) {}

      ImageCoord(double aLine, double aSamp) : line(aLine), samp(aSamp) {}
   };

   //***
   // STRUCT: ImageCoordCovar
   //> This structure represents an image coordinate with a corresponding
   //  2x2 covariance matrix.
   //
   //  The covariance is stored as an array of four elements that can be
   //  accessed directly or through the two-dimensional covar2d methods.
   //<
   //***
   struct ImageCoordCovar : public ImageCoord
   {
   public:
      double covariance[4];
         //> 2x2 line and sample covariance matrix, in pixels squared,
         //  stored as an array of four doubles as follows:
         //
         //-  [0] = line variance
         //-  [1] = line/sample covariance
         //-  [2] = sample/line covariance
         //-  [3] = sample variance
         //
         //  It can be accessed directly or through the covar2d methods.
         //<

      ImageCoordCovar() : ImageCoord() { std::memset(covariance, 0, sizeof(covariance)); }
         //> Default Constructor
         //<

      ImageCoordCovar(double aLine, double aSamp)
         : ImageCoord(aLine, aSamp) { std::memset(covariance, 0, sizeof(covariance)); }
         //> This constructor takes a line and sample in pixels.
         //  The covariance is set to zeroes.
         //<

      ImageCoordCovar(double aLine, double aSamp, double aCovar[4])
         : ImageCoord(aLine, aSamp) { std::memcpy(covariance, aCovar, sizeof(covariance)); }
         //> This constructor takes a line and sample in pixels and covariance
         //  as an array of four doubles in pixels squared.
         //  Note that no check is made to ensure symmetry of the covariance
         //  matrix.
         //<

      ImageCoordCovar(double aLine,    double aSamp,
                      double aCovar00, double aCovar01,
                                       double aCovar11)
         : ImageCoord(aLine, aSamp)
      {
         covariance[0] = aCovar00;
         covariance[1] = covariance[2] = aCovar01;
         covariance[3] = aCovar11;
      }
         //> This constructor takes a line and sample in pixels and the
         //  upper-triangular portion of a covariance matrix in pixels squared.
         //  It is assumed that the covariance matrix is symmetric.
         //<

      double  covar2d(unsigned int l, unsigned int s) const { return covariance[2*l + s]; }
         //> This method provides a convenient two-dimensional access to
         //  the covariance.  For example, the sample variance stored in
         //  covariance[3] could also be accessed as follows:
         //
         //-    ImageCoordCovar coord;
         //-    double A = coord.covar2d(1,1);
         //<

      double& covar2d(unsigned int l, unsigned int s)       { return covariance[2*l + s]; }
         //> This method provides a convenient two-dimensional means of setting
         //  the covariance.  For example, the sample variance stored in
         //  covariance[3] could also be set as follows:
         //
         //-    ImageCoordCovar coord;
         //-    coord.covar2d(1,1) = 0.5;
         //<
   };

   //***
   // STRUCT: ImageVector
   //> This structure represents a two-dimensional vector in image space.
   //  Units are pixels.  It can be used to represent the size of an image.
   //<
   //***
   struct ImageVector
   {
   public:
      double line;
      double samp;

      ImageVector() : line(0.0), samp(0.0) {}

      ImageVector(double aLine, double aSamp) : line(aLine), samp(aSamp) {}
   };

   //***
   // STRUCT: EcefCoord
   //> This structure represents a three-dimensional location (x,y,z in meters)
   //  in the WGS-84 Earth Centered Earth Fixed (ECEF) coordinate system.
   //  It typically represents an absolute coordinate; the EcefVector structure
   //  is used for velocity and direction vectors.
   //<
   //***
   struct EcefCoord
   {
   public:
      double x;
      double y;
      double z;

      EcefCoord() : x(0.0), y(0.0), z(0.0) {}

      EcefCoord(double aX, double aY, double aZ) : x(aX), y(aY), z(aZ) {}
   };

   //***
   // STRUCT: EcefCoordCovar
   //> This structure represents an ECEF coordinate with a corresponding
   //  3x3 covariance matrix.
   //
   //  The covariance is stored as an array of nine elements that can be
   //  accessed directly or through the two-dimensional covar2d methods.
   //<
   //***
   struct EcefCoordCovar : public EcefCoord
   {
   public:
      double covariance[9];
         //> 3x3 ECEF coordinate covariance matrix, in meters squared,
         //  stored as an array of nine doubles as follows:
         //
         //-  [0] = x  variance
         //-  [1] = xy covariance
         //-  [2] = xz covariance
         //-  [3] = yx covariance
         //-  [4] = y  variance
         //-  [5] = yz covariance
         //-  [6] = zx covariance
         //-  [7] = zy covariance
         //-  [8] = z  variance
         //
         //  It can be accessed directly or through the covar2d methods.
         //<

      EcefCoordCovar() : EcefCoord() { std::memset(covariance, 0, sizeof(covariance)); }
         //> Default Constructor
         //<

      EcefCoordCovar(double aX, double aY, double aZ)
         : EcefCoord(aX, aY, aZ) { std::memset(covariance, 0, sizeof(covariance)); }
         //> This constructor takes ECEF x, y, and z values in meters.
         //  The covariance is set to zeroes.
         //<

      EcefCoordCovar(double aX, double aY, double aZ, double aCovar[9])
         : EcefCoord(aX, aY, aZ) { std::memcpy(covariance, aCovar, sizeof(covariance)); }
         //> This constructor takes ECEF x, y, and z values in meters and
         //  covariance as an array of nine doubles in meters squared.
         //  Note that no check is made to ensure symmetry of the covariance
         //  matrix.
         //<

      EcefCoordCovar(double aX,       double aY,       double aZ,
                     double aCovar00, double aCovar01, double aCovar02,
                                      double aCovar11, double aCovar12,
                                                       double aCovar22)
         : EcefCoord(aX, aY, aZ)
      {
         covariance[0] = aCovar00;
         covariance[1] = covariance[3] = aCovar01;
         covariance[2] = covariance[6] = aCovar02;
         covariance[4] = aCovar11;
         covariance[5] = covariance[7] = aCovar12;
         covariance[8] = aCovar22;
      }
         //> This constructor takes a ECEF x, y, and z values in meters and the
         //  upper-triangular portion of a covariance matrix in meters squared.
         //  It is assumed that the covariance matrix is symmetric.
         //<

      double  covar2d(unsigned int l, unsigned int s) const { return covariance[3*l + s]; }
         //> This method provides a convenient two-dimensional access to
         //  the covariance.  For example, the yz covariance stored in
         //  covariance[5] could also be accessed as follows:
         //
         //-    ImageCoordCovar coord;
         //-    double A = coord.covar2d(1,2);
         //<

      double& covar2d(unsigned int l, unsigned int s) { return covariance[3*l + s]; }
         //> This method provides a convenient two-dimensional means of setting
         //  the covariance.  For example, the yz covariance stored in
         //  covariance[5] could also be set as follows:
         //
         //-    ImageCoordCovar coord;
         //-    coord.covar2d(1,2) = 0.5;
         //<
   };

   //***
   // STRUCT: EcefVector
   //> This structure represents a three-dimensional vector in the WGS-84 Earth
   //  Centered Earth Fixed coordinate system.
   //
   //  Units of meters (distance), meters per second (velocity) and no units
   //  are appropriate for EcefVector.
   //<
   //***
   struct EcefVector
   {
   public:
      double x;
      double y;
      double z;

      EcefVector() : x(0.0), y(0.0), z(0.0) {}
      EcefVector(double aX, double aY, double aZ) : x(aX), y(aY), z(aZ) {}
   };

   //***
   // STRUCT: EcefLocus
   //> This structure contains an ECEF coordinate (in meters) and
   //  an ECEF direction vector.
   //<
   //***
   struct EcefLocus
   {
   public:
      EcefCoord  point;
      EcefVector direction;

      EcefLocus() : point() , direction() {}

      EcefLocus(const EcefCoord& argPoint,
                const EcefVector& argDirection)
         :
            point     (argPoint),
            direction (argDirection)
      {}

      EcefLocus(double ptX , double ptY , double ptZ,
                double dirX, double dirY, double dirZ)
         :
            point    (ptX, ptY, ptZ),
            direction(dirX,dirY,dirZ)
      {}
   };

} // namespace csm

#endif
