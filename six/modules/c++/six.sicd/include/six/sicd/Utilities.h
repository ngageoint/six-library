/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef __SIX_SICD_UTILITIES_H__
#define __SIX_SICD_UTILITIES_H__

#include <memory>
#include <string>
#include <vector>
#include <utility>

#include <scene/SceneGeometry.h>
#include <scene/ProjectionModel.h>
#include <six/sicd/ComplexData.h>
#include <six/sicd/SICDMesh.h>
#include <six/NITFReadControl.h>
#include <six/sicd/AreaPlaneUtility.h>

namespace six
{
namespace sicd
{
class Utilities
{
public:
    /*!
     * Build SceneGeometry from ComplexData members
     * \param data ComplexData from which to construct Geometry
     * \return SceneGeometry from ComplexData
     */
    static scene::SceneGeometry* getSceneGeometry(const ComplexData* data);

    /*!
     * Build ProjectionModel from ComplexData members
     * \param data ComplexData from which to construct ProjectionModel
     * \param geom SceneGeometry for the model
     * \return ProjectionModel from complexData
     */
    static scene::ProjectionModel* getProjectionModel(const ComplexData* data,
            const scene::SceneGeometry* geom);

    /*!
     * Get information in or deriveable from the ComplexData.
     */
    static void getModelComponents(
        const ComplexData& complexData,
        std::auto_ptr<scene::SceneGeometry>& geometry,
        std::auto_ptr<scene::ProjectionModel>& projectionModel,
        six::sicd::AreaPlane& areaPlane);

    /*!
     * Build ProjectionPolynomialFitter from complexData and
     * given GriddedDisplayType.
     * This always uses a PlanarGridECEFTransform
     * \param complexData ComplexData from which to construct fitter
     * \param numPoints1D Number of points to use in each direction of grid.
     * \param sampleWithinValidDataPolygon Only get grid sample points from
     * with the valid data polygon.
     * \return ProjectionPolynomialFitter from ComplexData
     */
    static std::auto_ptr<scene::ProjectionPolynomialFitter>
    getPolynomialFitter(const ComplexData& complexData,
                        size_t numPoints1D =
                         scene::ProjectionPolynomialFitter::DEFAULTS_POINTS_1D,
                        bool sampleWithinValidDataPolygon = false);

    /*
     * If the SICD contains a valid data polygon, provides this.
     * If the SICD does not contain a valid data polygon, but it does contain
     * a radarCollection->area->plane block that defines the output plane,
     * projects the four output plane corners back into the slant plane and
     * uses this to approxiate the valid data polygon.
     */
    static void getValidDataPolygon(
            const ComplexData& sicdData,
            const scene::ProjectionModel& projection,
            std::vector<types::RowCol<double> >& validData);

    /*
     * Given a SICD path name and a list of schema, this function reads
     * and parses the SICD in order to provide the wideband data as well
     * as the ComplexData associated with the image.
     *
     * \param sicdPathname SICD NITF pathname
     * \param schemaPaths One or more files or directories containing SICD
     * schemas
     * \param ComplexData associated with the SICD NITF
     * \param widebandData, vector containing SICD wideband data
     *
     * \throws See six::sicd::Utilities::getComplexData and
     *           six::sicd::Utilities::getWidebandData
     *
     */
    static void readSicd(const std::string& sicdPathname,
                         const std::vector<std::string>& schemaPaths,
                         std::auto_ptr<ComplexData>& complexData,
                         std::vector<std::complex<float> >& widebandData);

    /*
     * Given a SICD path name and a list of schema, this function reads
     * and parses the SICD in order to provide the wideband data as well
     * as the ComplexData associated with the image.
     *
     * \param sicdPathname SICD NITF pathname
     * \param schemaPaths One or more files or directories containing SICD
     * schemas
     * \param orderX X order of the resultant projection polynomials.
     * \param orderY Y order of the resultant projection polynomials.
     * \param[out] ComplexData associated with the SICD NITF
     * \param[out] widebandData, vector containing SICD wideband data
     * \param[out] outputRowColToSlantRow Projection polynomial taking
     *  (row,column) coordinate in the output plane image as input
     *  and returning the projected row coordinate in the slant plane
     *  image as output.
     * \param[out] outputRowColToSlantCol Projection polynomial taking
     *  (row,column) coordinate in the output plane image as input
     *  and returning the projected column coordinate in the
     *  slant plane image as output.
     * \param[out] noiseMesh Noise mesh
     *
     * \throws See six::sicd::Utilities::getComplexData and
     *           six::sicd::Utilities::getWidebandData
     *
     */
    static void readSicd(const std::string& sicdPathname,
                         const std::vector<std::string>& schemaPaths,
                         size_t orderX,
                         size_t orderY,
                         std::auto_ptr<ComplexData>& complexData,
                         std::vector<std::complex<float> >& widebandData,
                         six::Poly2D& outputRowColToSlantRow,
                         six::Poly2D& outputRowColToSlantCol,
                         std::auto_ptr<NoiseMesh>& noiseMesh);

    /*
     * Given a SICD pathname and list of schemas, provides a representation
     * of the SICD XML as a ComplexData object
     *
     * \param pathname SICD NITF or XML pathname
     * \param schemaPaths One or more files or directories containing SICD
     * schemas
     *
     * \return ComplexData associated with the SICD NITF or XML
     *
     * \throws except::Exception if file is not a SICD or Complex XML
     */
    static
    std::auto_ptr<ComplexData> getComplexData(
            const std::string& pathname,
            const std::vector<std::string>& schemaPaths);

    /*
     * Given a reference to a loaded NITFReadControl, this function
     * parses the SICD's XML and returns a new ComplexData object.
     *
     * \param reader A NITFReadControl loaded with the desired SICD
     *
     * \return ComplexData associated with the SICD NITF
     *
     * \throws except::Exception if the provided reader is not a SICD
     *
     */
    static std::auto_ptr<ComplexData> getComplexData(NITFReadControl& reader);

    /*
     * Given a loaded NITFReadControl and a ComplexData object, this
     * function loads the wideband data associated with the reader
     * and ComplexData object.
     *
     * \param reader A loaded NITFReadControl associated with the SICD
     * \param complexData complexData associated with the SICD
     * \param buffer A pointer to the buffer to load data into.  Must be
     *   at least complexData.getNumCols() * complexData.getNumRows() pixels
     *
     * \return a pointer to the loaded data.
     *
     * \throws except::Exception if the pixel type of the SICD is not a
     *           complex float32 or complex int64, or
     *         if the buffer pointer is null
     */
    static void getWidebandData(NITFReadControl& reader,
                                const ComplexData& complexData,
                                std::complex<float>* buffer);

    /*
     * Given a loaded NITFReadControl and a ComplexData object, this
     * function loads the wideband data of the region associated with
     * the reader and ComplexData object.
     *
     * \param reader A loaded NITFReadControl associated with the SICD
     * \param complexData complexData associated with the SICD
     * \param offset The starting row and column in the region
     * \param extent The number of rows and columns in the region
     * \param buffer A pointer to the buffer to load data into.  Must be
     *   at least complexData.getNumCols() * complexData.getNumRows() pixels
     *
     * \return a pointer to the loaded data.
     *
     * \throws except::Exception if the pixel type of the SICD is not a
     *           complex float32 or complex int64, or
     *         if the buffer pointer is null
     */
    static void getWidebandData(NITFReadControl& reader,
                                const ComplexData& complexData,
                                const types::RowCol<size_t>& offset,
                                const types::RowCol<size_t>& extent,
                                std::complex<float>* buffer);

    /*
     * Given a loaded NITFReadControl and a ComplexData object, this
     * function loads the wideband data associated with the reader
     * and ComplexData object. This loads the whole image.
     *
     * This function allows the user to provide a vector to be resized
     * to fit the whole image in.
     *
     * \param reader A loaded NITFReadControl associated with the SICD
     * \param complexData complexData associated with the SICD
     * \param buffer The functions output, will contain the image
     *
     * \throws except::Exception if the pixel type of the SICD is not a complex
     *           float32 or complex int16
     */
    static void getWidebandData(NITFReadControl& reader,
                                const ComplexData& complexData,
                                std::vector<std::complex<float> >& buffer);

    /*
     * Given a loaded NITFReadControl, a ComplexData object, and an
     * offset and extent, this function loads the wideband data of the
     * specificed region into the provided buffer.
     *
     * This function allows the user to provide a vector to be resized
     * to fit the region in.
     *
     * \param reader A loaded NITFReadControl associated with the SICD
     * \param complexData complexData associated with the SICD
     * \param offset The first row and column in the region to be read
     * \param extent The number of rows and columns in the region
     * \param buffer The functions output, will contain the image
     *
     * \throws except::Exception if the pixel type of the SICD is not a complex
     *           float32 or complex int16
     */
     static void getWidebandData(NITFReadControl& reader,
                                const ComplexData& complexData,
                                const types::RowCol<size_t>& offset,
                                const types::RowCol<size_t>& extent,
                                std::vector<std::complex<float> >& buffer);

     /*
     * Given a SICD pathname and list of schemas, provides a representation
     * of the SICD pixel data in a buffer. This reads the whole image.
     *
     * \param sicdPathname SICD NITF pathname
     * \param schemaPaths One or more files or directories containing SICD
     * schemas
     * \param complexData The ComplexData object associated with the SICD
     * \param buffer The pre-sized buffer to be read into
     *
     * \throws except::Exception if the pixel type of the SICD is not a complex
     *           float32 or complex int16, or
     *         if the buffer pointer is null
     */
    static
    void getWidebandData(
            const std::string& sicdPathname,
            const std::vector<std::string>& schemaPaths,
            const ComplexData& complexData,
            std::complex<float>* buffer);

     /*
     * Given a SICD pathname, list of schemas, complexData, and a region of interest,
     * provides a representation the given region's SICD pixel data in a buffer
     *
     * \param sicdPathname SICD NITF pathname
     * \param schemaPaths One or more files or directories containing SICD
     * schemas
     * \param complexData The ComplexData object associated with the SICD
     * \param offset The start row and column of the desired region
     * \param extent The number of rows and columns to be read
     * \param buffer The pre-sized buffer to be read into
     *
     * \throws except::Exception if the pixel type of the SICD is not a complex
     *           float32 or complex int16, or
     *         if the buffer pointer is null
     *
     */
    static
    void getWidebandData(
            const std::string& sicdPathname,
            const std::vector<std::string>& schemaPaths,
            const ComplexData& complexData,
            const types::RowCol<size_t>& offset,
            const types::RowCol<size_t>& extent,
            std::complex<float>* buffer);

    /*
    * Return the unit vector normal to the ground plane.
    * If an output plane is defined (i.e. RadarCollection.Area.Plane
    * exists), this is taken to be the ground plane. Otherwise,
    * the normalized ECEF coordinates of the Scene Center Point
    * (GeoData.scp.ec) are returned.
    * \param data The ComplexData object associated with the SICD
    * \return the unit vector normal to the ground plane
    */
    static Vector3 getGroundPlaneNormal(const ComplexData& data);

    /*
     * Extracts a Poly1D in the dimention given by index
     * \param poly The source polynomial
     * \param index Number specifying dimension. 0-2 for X-Z
     * \return Poly1D in specified dimension
     */
    static Poly1D nPoly(PolyXYZ poly, size_t index);

    /*
     * Determine if the vertices of a simple polygon are clockwise
     * Throw an exception if vertices has less than three points
     *
     * \param vertices The vertices of what is assumed to be a simple polygon
     * \param isUpPositive True if y value increased while going up the axis
     * \return True if vertices are clockwise
     */
    static bool isClockwise(const std::vector<RowColInt>& vertices,
                            bool isUpPositive=false);

     /* Parses the XML in 'xmlStream' and converts it into a ComplexData object.
     * Throws if the underlying type is not complex.
     *
     * \param xmlStream Input stream containing XML
     * \param schemaPaths Schema path(s)
     * \param log Logger
     *
     * \return Data representation of 'xmlStr'
     */
    static std::auto_ptr<ComplexData> parseData(
            ::io::InputStream& xmlStream,
            const std::vector<std::string>& schemaPaths,
            logging::Logger& log);

    /*
     * Parses the XML in 'pathname' and converts it into a ComplexData object.
     * Throws if the underlying type is not complex.
     *
     * \param pathname File containing plain text XML (not a NITF)
     * \param schemaPaths Schema path(s)
     * \param log Logger
     *
     * \return Data representation of the contents of 'pathname'
     */
    static std::auto_ptr<ComplexData> parseDataFromFile(
            const std::string& pathname,
            const std::vector<std::string>& schemaPaths,
            logging::Logger& log);

    /*
     * Parses the XML in 'xmlStr' and converts it into a ComplexData object.
     *
     * \param xmlStr XML document as a string
     * \param schemaPaths Schema path(s)
     * \param log Logger
     *
     * \return Data representation of 'xmlStr'
     */
    static std::auto_ptr<ComplexData> parseDataFromString(
        const std::string& xmlStr,
        const std::vector<std::string>& schemaPaths,
        logging::Logger& log);

    /*
     * Converts 'data' back into a formatted XML string
     *
     * \param data Representation of SICD data
     * \param schemaPaths Schema paths.  If empty, the SIX_SCHEMA_PATH
     * environment variable will be used.
     * \param logger Logger.  If NULL, no logger will be used.
     *
     * \return XML string representation of 'data'
     */
    static std::string toXMLString(
            const ComplexData& data,
            const std::vector<std::string>& schemaPaths =
                    std::vector<std::string>(),
            logging::Logger* logger = NULL);

    /*!
     * Create a fake SICD that's populated enough for
     * general testing code to run without throwing exceptions
     *
     * \return mock ComplexData object
     */
    static std::auto_ptr<ComplexData> createFakeComplexData();

    /*
     * Given a reference to a loaded NITFReadControl, this function
     * parses the SICD's DES and returns a NoiseMesh if present.
     * \param reader A NITFReadControl loaded with the desired SICD
     * \return Noise Mesh associated with the SICD NITF
     * \throws except::Exception if the provided reader is not a SICD
     *
     */
    static std::auto_ptr<NoiseMesh> getNoiseMesh(NITFReadControl& reader);

    /*
     * Given a reference to a loaded NITFReadControl, this function
     * parses the SICD's DES and returns fitted projection polynomials
     * of the desired order.
     * \param reader A NITFReadControl loaded with the desired SICD
     * \param orderX X order of fitted polynomials.
     * \param orderY Y order of fitted polynomials.
     * \param complexData ComplexData from which to construct fitter
     * \param[out] outputRowColToSlantRow Projection polynomial taking
     *  (row, column) coordinate in the output plane image as input
     *  and returning the projected row coordinate in the slant plane
     *  image as output.
     * \param[out] outputRowColToSlantCol Projection polynomial taking
     *  (row, column) coordinate in the output plane image as input
     *  and returning the projected column coordinate in the slant
     *  plane image as output.
     * \throws except::Exception if the provided reader is not a SICD or
     *  projection polynomials can't be computed.
     */
    static void getProjectionPolys(
        NITFReadControl& reader,
        size_t orderX,
        size_t orderY,
        std::auto_ptr<ComplexData>& complexData,
        six::Poly2D& outputRowColToSlantRow,
        six::Poly2D& outputRowColToSlantCol);

    /*!
     * Convert a polynomial defined over an (X,Y) coordinate system
     * to a (row,column) coordinate system. Here X and Y are defined
     * as the planar distance in meters from the scene center position
     * of an image in the row and column directions respectively. The
     * input polynomial is expected to take an (X,Y) position over the
     * output plane image as input.
     * \param polyXY Polynomial taking an (X,Y) coordinate in meters
     *  from ORP defined over the output plane image as input.
     * \param outSampleSpacing The output plane image sample spacing
     *  in meters per pixel in row and column directions.
     * \param outCenter The output plane image center pixel.
     * \param polyScaleFactor Multiplicative scalar to apply to the
     *  transformed output polynomial. For instance, if the input
     *  polynomial returns a time in seconds and the desired output is
     *  a time in minutes, the scale factor would be 1.0 / 60
     * \param polyShift An additive shift to apply to the constant
     *  term of the transformed output polynomial.
     * \return A transformed copy of the input polynomial taking
     *  (row,column) coordinate over the output plane image as
     *  inputs. Polynomial output is scaled and shifted according to
     *  user input for polyScaleFactor and polyShift.
     */
    static six::Poly2D transformXYPolyToRowColPoly(
        const six::Poly2D& polyXY,
        const types::RowCol<double>& outSampleSpacing,
        const types::RowCol<double>& outCenter,
        double polyScaleFactor,
        double polyShift);

    /*!
     * This function converts projection polynomials from an (X,Y)
     * coordinate system to a (row,column) coordinate system. Here X
     * and Y are defined as the planar distance in meters from the
     * scene center position of an image in the row and column
     * directions respectively. Input polynomials take as input an
     * (X,Y) coordinate over the output plane image and return the
     * projected X/Y coordinate in the slant plane image.
     * Transformed polynomials take as input a (row,column)
     * coordinate over the outptu plane image and return the projected
     * row/column coordinate in the slant plane image.
     * \param outputXYToSlantX Projection polynomial that computes a
     *  slant plane image X coordinate in meters from SCP from an
     *  (X,Y) output plane image coordinate in meters from ORP
     * \param outputXYToSlantY Projection polynomial that computes a
     *  slant plane image Y coordinate in meters from SCP from an
     *  (X,Y) output plane image coordinate in meters from ORP
     * \param slantSampleSpacing Sample spacing over the slant
     *  plane image in meters per pixel.
     * \param outputSampleSpacing Sample spacing over the output plane
     *  image in meters per pixel.
     * \param slantCenter Center pixel coordinate over the slant
     *  plane image.
     * \param outputCenter Center pixel coordinate over the output
     *  plane image.
     * \param[out] outputRowColToSlantRow Projection polynomial that
     *  computes a slant plane image row coordinate from a
     *  (row,column) coordinate in the output plane image
     * \param[out] outputRowColToSlantCol Projection polynomial that
     *  computes a slant plane image column coordinate from a
     *  (row,column) coordinate in the output plane image
     */
    static void transformXYProjectionPolys(
        const six::Poly2D& outputXYToSlantX,
        const six::Poly2D& outputXYToSlantY,
        const types::RowCol<double>& slantSampleSpacing,
        const types::RowCol<double>& outputSampleSpacing,
        const types::RowCol<double>& slantCenter,
        const types::RowCol<double>& outputCenter,
        six::Poly2D& outputRowColToSlantRow,
        six::Poly2D& outputRowColToSlantCol);

    /*!
     * Compute project polynomials from mesh information.
     * \param outputMesh Output plane mesh. Contains a grid of (X,Y)
     *  coordinates in meters from ORP over the output plane image.
     * \param slantMesh Slant plane mesh. Contains a grid of (X,Y)
     *  coordinates in meters from SCP over the slant plane image.
     * \param orderX X order of the resultant polynomials.
     * \param orderY Y order of the resultant polynomials.
     * \param[out] outputXYToSlantX Projection polynomial taking
     *  output plane (X,Y) coordinate in meters from ORP to slant
     *  plane X-coordinate in metesr from SCP.
     * \param[out] slantXYToOutputX Projection polynomial taking
     *  output plane (X,Y) coordinate in meters from ORP to slant
     *  plane Y-coordinate in metesr from SCP. to Output plane
     *  X-coordinate in meters from ORP.
     * \param[out] slantXYToOutputY Projection polynomial taking
     *  output plane (X,Y) coordinate in meters from ORP to slant
     *  plane Y-coordinate in metesr from SCP. to Output plane
     *  Y-coordinate in meters from ORP.
     */
     static void fitXYProjectionPolys(
            const six::sicd::PlanarCoordinateMesh& outputMesh,
            const six::sicd::PlanarCoordinateMesh& slantMesh,
            size_t orderX,
            size_t orderY,
            six::Poly2D& outputXYToSlantX,                  
            six::Poly2D& outputXYToSlantY,                  
            six::Poly2D& slantXYToOutputX,                  
            six::Poly2D& slantXYToOutputY);

    /*!
     * Project slant plane pixel locations to the output plane pixel locations.
     * \param complexData Complex metadata.
     * \param spPixels Slant plane pixel coordinates.
     * \param opPixels Output plane pixel coordinates.
     */
    static void projectPixelsToOutputPlane(
        const six::sicd::ComplexData& complexData,
        const std::vector<types::RowCol<double> >& spPixels,
        std::vector<types::RowCol<double> >& opPixels);

    /*!
     * Project slant plane valid data polygon pixel locations to output
     * plane pixel locations.
     * \param complexData Complex metadata.
     * \param opPixels Output plane pixel coordinates.
     */
    static void projectValidDataPolygonToOutputPlane(
        const six::sicd::ComplexData& complexData,
        std::vector<types::RowCol<double> >& opPixels);

    /*!
     * Project output plane pixel locations to slant plane pixel locations.
     * \param complexData Complex metadata.
     * \param opPixels Output plane pixel coordinates.
     * \param spPixels Slant plane pixel coordinates.
     */
    static void projectPixelsToSlantPlane(
        const six::sicd::ComplexData& complexData,
        const std::vector<types::RowCol<double> >& opPixels,
        std::vector<types::RowCol<double> >& spPixels);
};
}
}
#endif

