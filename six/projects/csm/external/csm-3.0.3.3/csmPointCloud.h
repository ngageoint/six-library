//#############################################################################
//
//    FILENAME:          csmPointCloud.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Additions were made to the CSM API in CSM 3.0.2 to handle point clouds.
//    The previous API did not change. Previously this code was in a separate
//    library, but has now been migrated into a single library as of
//    CSM3.0.3
//
//    LIMITATIONS:       None
//
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//
//    22FEB2018     JPK       Modified to use existing csm macros (since
//                            point cloud is no longer in a separate library).
//    NOTES:
//
//#############################################################################

#ifndef __CSM_CSM_POINT_CLOUD_H
#define __CSM_CSM_POINT_CLOUD_H

#include <string>
#include "csm.h"

namespace csm
{
   //***
   // STRUCT: ModelCoord
   //> This structure represents a three-dimensional location (m0,m1,m2 in
   //  model coordinates).  Units are defined by the specific model
   //  implementation; in general, the meaning of model coordinates is
   //  determined by the dataset used to instantiate the model.
   //<
   //***
   struct ModelCoord
   {
   public:
      double m0;
      double m1;
      double m2;

      ModelCoord() : m0(0.0), m1(0.0), m2(0.0) {}

      ModelCoord(double aM0, double aM1, double aM2) : m0(aM0), m1(aM1), m2(aM2) {}
   };

   //***
   // STRUCT: ModelCoordCovar
   //> This structure represents a model-space coordinate with a corresponding
   //  3x3 covariance matrix.  As explained above, the meaning and units of
   //  model coordinates are defined by the model implementation.  Units in the
   //  covariance matrix will be the product of the quantities involved.  For
   //  example, the units of the upper-left entry in the covariance matrix will
   //  be (units of model coordinate 0, squared).  The units of the upper-right
   //  entry will be (units of model coordinate 0 * units of model coordinate
   //  2).
   //
   //  The covariance is stored as an array of nine elements that can be
   //  accessed directly or through the two-dimensional covar2d methods.
   //<
   //***
   struct ModelCoordCovar : public ModelCoord
   {
   public:
      double covariance[9];
         //> 3x3 model-space covariance matrix,  stored as an array of nine
         //  doubles as follows:
         //
         //-  [0] = m0 variance
         //-  [1] = m0/m1 covariance
         //-  [2] = m0/m2 covariance
         //-  [3] = m1/m0 covariance
         //-  [4] = m1 variance
         //-  [5] = m1/m2 covariance
         //-  [6] = m2/m0 covariance
         //-  [7] = m2/m1 covariance
         //-  [8] = m2 variance
         //
         //  It can be accessed directly or through the covar2d methods.
         //<

      ModelCoordCovar() : ModelCoord() { memset(covariance, 0, sizeof(covariance)); }
         //> Default Constructor
         //<

      ModelCoordCovar(double aM0, double aM1, double aM2)
         : ModelCoord(aM0, aM1, aM2) { memset(covariance, 0, sizeof(covariance)); }
         //> This constructor takes model coordinate values.
         //  The covariance is set to zeroes.
         //<

      ModelCoordCovar(double aM0, double aM1, double aM2, double aCovar[9])
         : ModelCoord(aM0, aM1, aM2) { memcpy(covariance, aCovar, sizeof(covariance)); }
         //> This constructor takes model coordinate values and covariance as
         //  an array of nine doubles.  Note that no check is made to ensure
         //  symmetry of the covariance matrix.
         //<

      ModelCoordCovar(double aM0,      double aM1,      double aM2,
                      double aCovar00, double aCovar01, double aCovar02,
                                       double aCovar11, double aCovar12,
                                                        double aCovar22)
         : ModelCoord(aM0, aM1, aM2)
      {
         covariance[0] = aCovar00;
         covariance[1] = covariance[3] = aCovar01;
         covariance[2] = covariance[6] = aCovar02;
         covariance[4] = aCovar11;
         covariance[5] = covariance[7] = aCovar12;
         covariance[8] = aCovar22;
      }
         //> This constructor takes model coordinate values and the upper-
         //  triangular portion of a covariance matrix. It is assumed that the
         //  covariance matrix is symmetric.
         //<

      double covar2d(unsigned int row, unsigned int col) const { return covariance[col + 3 * row]; }
         //> This method provides a convenient two-dimensional access to
         //  the covariance.  For example, the m1/m2 covariance stored in
         //  covariance[5] could also be accessed as follows:
         //
         //-    ImageCoordCovar coord;
         //-    double A = coord.covar2d(1,2);
         //<

      double& covar2d(unsigned int row, unsigned int col) { return covariance[col + 3 * row]; }
         //> This method provides a convenient two-dimensional means of setting
         //  the covariance.  For example, the m1/m2 covariance stored in
         //  covariance[5] could also be set as follows:
         //
         //-    ImageCoordCovar coord;
         //-    coord.covar2d(1,2) = 0.5;
         //<
   };

   //***
   // STRUCT: ModelVector
   //> This structure represents a three-dimensional vector in a sensor's
   //  model coordinate system.
   //<
   //***
   struct ModelVector
   {
   public:
       double m0;
       double m1;
       double m2;
   
       ModelVector() : m0(0.0), m1(0.0), m2(0.0) { }
       ModelVector(double aM0, double aM1, double aM2) : m0(aM0), m1(aM1), m2(aM2) { }
   };

   //***
   // STRUCT: ModelCoordProperties
   //> This structure contains a description of a three-dimensional model
   //  coordinate system.  It is used by a sensor model to return a human-
   //  readable description of its model coordinate space.
   //
   //  Unit strings should follow the same conventions as the
   //  GeometricModel::getParameterUnits function whenever possible.
   //
   //  EXAMPLE: if a given sensor model uses UTM coordinates for horizontal
   //  position and HAE for vertical position, the corresponding
   //  ModelCoordProperties definition could be:
   //     ModelCoordProperties mcp;
   //     mcp.description = "UTM Zone 11 North";
   //     mcp.name0       = "UTM Easting";
   //     mcp.name1       = "UTM Northing";
   //     mcp.name2       = "Height above WGS-84 ellipsoid";
   //     mcp.units0      = "m";
   //     mcp.units1      = "m";
   //     mcp.units2      = "m";
   //<
   //***
   struct ModelCoordProperties {
       std::string description; // a short description of the model's coordinate system
       std::string name0;       // the name of the first model coordinate (m0)
       std::string name1;       // the name of the second model coordinate (m1)
       std::string name2;       // the name of the third model coordinate (m2)
       std::string units0;      // the units of the first model coordinate (m0)
       std::string units1;      // the units of the second model coordinate (m1)
       std::string units2;      // the units of the third model coordinate (m2)
   };

   //***
   // STRUCT: ModelBounds
   //> This structure describes an oriented bounding box in the model
   //  coordinate system.  It is used by a sensor model to return the model-
   //  space bounds of the region over which the model is valid.
   //
   //  The bounding box is centered on a given model coordinate.  It has three
   //  perpendicular axes labelled axis0, axis1, and axis2 below, with a
   //  dimension along each axis.  The structure has a contains() function that
   //  returns whether a given model coordinate lies within the box or not.
   //<
   //***
   class CSM_EXPORT_API ModelBounds
   {
   public:
      ModelBounds();
         //> This constructor creates a ModelBounds with uninitialized member
         //  variables.
         //<

      ModelBounds(const ModelCoord& center, double m0Dim, double m1Dim, double m2Dim);
         //> This constructor creates a ModelBounds object representing the
         //  axis-aligned bounding box with its center at the given center
         //  point and dimensions along the m0, m1, and m2 axes given by
         //  m0Dim, m1Dim, and m2Dim, respectively.
         //
         //  NOTE: A csm::Error exception will be thrown by this constructor
         //  if m0Dim, m1Dim, or m2Dim are zero.
         //<

      ModelBounds(const ModelCoord& center,
                  const ModelVector& axis0,
                  const ModelVector& axis1,
                  const ModelVector& axis2,
                  double dim0,
                  double dim1,
                  double dim2);
         //> This constructor creates a ModelBounds object representing the
         //  oriented bounding box with its center at the given center point,
         //  axes along the given axis0, axis1, and axis2 model-space
         //  directions (which must be mutally perpendicular), and dimensions
         //  along those axes given by dim0, dim1, and dim2, respectively.
         //
         //  NOTE: A csm::Error exception will be thrown by this constructor
         //  if the given axes are linearly dependent, or if any of the
         //  dimensions are zero.
         //<

      bool contains(const ModelCoord& modelCoord) const;
         //> This function returns true if the given model coordinate lies
         //  inside the oriented bounding box represented by this object,
         //  false otherwise.
         //<

      ModelCoord getCenter() const { return theCenter; }
         //> This function returns the model coordinate of the center of the
         //  bounding region.
         //<

      ModelVector getAxis0() const { return theA0; }
      ModelVector getAxis1() const { return theA1; }
      ModelVector getAxis2() const { return theA2; }
         //> These functions return unit-length vectors representing the
         //  three axes of the bounding box.  These axes correspond to the
         //  parameters axis0, axis1, and axis2 of the second constructor,
         //  respectively.
         //<

      double getDimension0() const { return theDim0; }
      double getDimension1() const { return theDim1; }
      double getDimension2() const { return theDim2; }
         //> These functions return the dimensions of the bounding box along
         //  the axes axis0, axis1, and axis2, respectively.
         //<

   private:
      ModelCoord theCenter;
      ModelVector theA0, theA1, theA2;
      double theDim0, theDim1, theDim2;

      static double normalize(ModelVector& v);
         //> Rescales the given vector v so that it has unit length, and
         //  returns the original length of v.
         //
         //  NOTE: if v was the zero vector on input, it will remain a zero-
         //  length vector and 0.0 will be returned from the function.
         //<

      static ModelVector orthogonalProjection(const ModelVector& u, const ModelVector& v);
         //> Returns the component of the vector u that is orthogonal to the
         //  vector v.
         //
         //  NOTE: if v is the zero vector, u will be returned.
         //<
   };
}

#endif
