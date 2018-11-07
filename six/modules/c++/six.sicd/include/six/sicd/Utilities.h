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
     * Build ProjectionPolynomialFitter from complexData and
     * given GriddedDisplayType.
     * This always uses a PlanarGridECEFTransform
     * \param complexData ComplexData from which to construct fitter
     * \return ProjectionPolynomialFitter from ComplexData
     */
    static std::auto_ptr<scene::ProjectionPolynomialFitter>
    getPolynomialFitter(const ComplexData& complexData);

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
     * \param[out] ComplexData associated with the SICD NITF
     * \param[out] widebandData, vector containing SICD wideband data
     * \param[out] outputRowColToSlantRow Projection polynomial
     * \param[out] outputRowColToSlantCol Projection polynomial
     * \param[out] noiseMesh Noise mesh
     *
     * \throws See six::sicd::Utilities::getComplexData and
     *           six::sicd::Utilities::getWidebandData
     *
     */
    static void readSicd(const std::string& sicdPathname,
                         const std::vector<std::string>& schemaPaths,
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
     * parses the SICD's DES and returns a a vector of Mesh objects.
     * \param reader A NITFReadControl loaded with the desired SICD
     * \param[out] outputRowColToSlantRow Projection polynomial
     * \param[out] outputRowColToSlantCol Projection polynomial
     * \throws except::Exception if the provided reader is not a SICD or
     *  projection polynomials can't be computed.
     */
    static void getProjectionPolys(
        NITFReadControl& reader,
        std::auto_ptr<ComplexData>& complexData,
        six::Poly2D& outputRowColToSlantRow,
        six::Poly2D& outputRowColToSlantCol);

    /*!
     * Convert a polynomial of X and Y to a polynomial of row and col.
     * \param polyXY The polynomial of X and Y.
     * \param outSampleSpacing The output sample spacing.
     * \param outCenter The output center row and col.
     * \param polyScaleFactor A scale factor value.
     * \param polyShift An offset value.
     * \result the polynomial of row and col.
     */
    static six::Poly2D transformXYPolyToRowColPoly(
        const six::Poly2D& polyXY,
        const types::RowCol<double>& outSampleSpacing,
        const types::RowCol<double>& outCenter,
        double polyScaleFactor,
        double polyShift);

    /*!
     * Convert polynomials that compute X (Y) in the input from X and
     * Y in the output into polynomials that compute row (col) in the
     * input from row and col in the output.
     * \param outputXYToInputX The polynomial that computes an input X
     *  from an output X and Y.
     * \param outputXYToInputY The polynomial that computes an input Y
     *  from an output X and Y.
     * \param inSampleSpacing The input sample spacing.
     * \param outSampleSpacing The output sample spacing.
     * \param inCenter The input center pixel coordinate.
     * \param outCenter The output center pixel coordinate.
     * \param[out] outputRowColToInputRow The polynomial that computes
     *  an input row from an output row and col.
     * \param[out] outputRowColToInputCol The polynomial that computes
     *  an input col from an output row and col.
     */
    static void transformXYProjectionPolys(const six::Poly2D& outputXYToInputX,
                                    const six::Poly2D& outputXYToInputY,
                                    const types::RowCol<double>& inSampleSpacing,
                                    const types::RowCol<double>& outSampleSpacing,
                                    const types::RowCol<double>& inCenter,
                                    const types::RowCol<double>& outCenter,
                                    six::Poly2D& outputRowColToInputRow,
                                    six::Poly2D& outputRowColToInputCol);

    /*!
     * Compute project polynomials from mesh information.
     * \param outputMesh Output plane mesh.
     * \param slantMesh Slant plane mesh.
     * \param orderX X order of the resultant polynomials.
     * \param orderY Y order of the resultant polynomials.
     * \param[out] outputXYToSlantX Output plane (X,Y) meters from ORP
     *  to slant plane X meters from SCP project polynomial.
     * \param[out] outputXYToSlantY Output plane (X,Y) meters from ORP
     *  to slant plane Y meters from SCP project polynomial.
     * \param[out] slantXYToOutputX Slant plane (X,Y) meters from SCP
     *  to output plane X meters from ORP project polynomial.
     * \param[out] slantXYToOutputY Slant plane (X,Y) meters from SCP
     *  to output plane Y meters from ORP project polynomial.
     * \result true for successful processing.
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
};
}
}
#endif

