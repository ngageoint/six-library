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
#ifndef __SIX_GRID_H__
#define __SIX_GRID_H__

#include <logging/Logger.h>
#include <mem/ScopedCopyablePtr.h>
#include <mem/ScopedCloneablePtr.h>
#include "six/CollectionInformation.h"
#include "six/sicd/Functor.h"
#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"
#include "six/ParameterCollection.h"
#include "six/sicd/GeoData.h"
#include "six/sicd/ImageData.h"
#include "six/sicd/RadarCollection.h"
#include "six/sicd/RMA.h"

namespace six
{
namespace sicd
{
struct PFA;
struct RgAzComp;
struct SCPCOA;

struct WeightType
{
    WeightType();

    /*!
     *  Type of aperture weighting applied in the spatial
     *  frequency domain to yield the impulse response in the r/c
     *  direction.  Examples include UNIFORM, TAYLOR, HAMMING, UNKNOWN
     */
    std::string windowName;

    /*!
     *  Optional free format field that can be used to pass forward the
     *  weighting parameter information.
     *  This is present in 1.0 (but not 0.4.1) and can be 0 to unbounded
     */
    ParameterCollection parameters;

    bool operator==(const WeightType& rhs) const
    {
        return windowName == rhs.windowName && parameters == rhs.parameters;
    }
    bool operator!=(const WeightType& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct DirectionParameters
 *  \brief Struct for SICD Row/Col Parameters
 *
 *  Parameters describing increasing row or column
 *  direction image coords
 */
struct DirectionParameters
{
    DirectionParameters();
    DirectionParameters* clone() const;

    //! Unit vector in increasing row or col direction
    Vector3 unitVector;

    //!  Sample spacing in row or col direction
    double sampleSpacing;

    //!  Half-power impulse response width in increasing row/col dir
    //!  Measured at scene center point
    double impulseResponseWidth;

    //! FFT sign
    FFTSign sign;

    //! Spatial bandwidth in Krow/Kcol used to form the impulse response
    //! in row/col direction, measured at scene center
    double impulseResponseBandwidth;

    //! Center spatial frequency in the Krow/Kcol
    double kCenter;

    //!  Minimum r/c offset from kCenter of spatial freq support for image
    double deltaK1;

    //!  Maximum r/c offset from kCenter of spatial freq support for image
    double deltaK2;

    /*!
     *  Offset from kCenter of the center of support in the r/c
     *  spatial frequency.  The polynomial is a function of the image
     *  r/c
     */
    Poly2D deltaKCOAPoly;

    //!  Optional parameters describing the aperture weighting
    mem::ScopedCopyablePtr<WeightType> weightType;

    /*!
     *  Sampled aperture amplitude weighting function applied
     *  in Krow/col to form the SCP impulse response in the row
     *  direction
     *  \note You cannot have less than two weights if you have any
     *  2 <= NW <= 512 according to spec
     *
     *  \todo could make this an object (WeightFunction)
     *
     */
    std::vector<double> weights;

    bool operator==(const DirectionParameters& rhs) const;
    bool operator!=(const DirectionParameters& rhs) const
    {
        return !(*this == rhs);
    }

    bool validate(const ImageData& imageData,
            logging::Logger& log) const;
    bool validate(const RgAzComp& rgAzComp,
            logging::Logger& log,
            double offset = 0) const;

    void fillDerivedFields(const ImageData& imageData);
    void fillDerivedFields(const RgAzComp& rgAzComp, double offset = 0);
    std::unique_ptr<Functor> calculateWeightFunction() const;

private:

    bool validateWeights(const Functor& weightFunction,
            logging::Logger& log) const;

    double derivedKCenter(const RgAzComp& rgAzComp,
            double offset = 0) const;

    Poly2D derivedKcoaPoly(const RgAzComp& rgAzComp,
            double offset = 0) const;

    std::vector<RowColInt>
            calculateImageVertices(const ImageData& imageData) const;

    /* Return vector contents, in order:
    * 0) deltaK1 (min)
    * 1) deltaK2 (max)
    */
    std::pair<double, double> calculateDeltaKs(
            const ImageData& imageData) const;

    static const double WGT_TOL;
    static const size_t DEFAULT_WEIGHT_SIZE;
    static const char BOUNDS_ERROR_MESSAGE[];

};

/*!
 *  \struct Grid
 *  \brief SICD Grid parameters
 *
 *  The block of parameters that describes the image sample grid
 *
 */
struct Grid
{

    //! TODO what to do with plane
    Grid();

    Grid* clone() const;
    ComplexImagePlaneType imagePlane;
    ComplexImageGridType type;
    Poly2D timeCOAPoly;
    mem::ScopedCloneablePtr<DirectionParameters> row;
    mem::ScopedCloneablePtr<DirectionParameters> col;

    bool operator==(const Grid& rhs) const;
    bool operator!=(const Grid& rhs) const
    {
        return !(*this == rhs);
    }

    bool validate(const CollectionInformation& collectionInformation,
            const ImageData& imageData,
            logging::Logger& log) const;

    bool validate(const RMA& rma, const Vector3& scp,
            const PolyXYZ& arpPoly, double fc,
            logging::Logger& log) const;

    bool validate(const PFA& pfa, const RadarCollection& radarCollection,
        double fc, logging::Logger& log) const;

    bool validate(const RgAzComp& rgAzComp,
            const GeoData& geoData,
            const SCPCOA& scpcoa,
            double fc,
            logging::Logger& log) const;

    void fillDerivedFields(const CollectionInformation& collectionInformation,
                           const ImageData& imageData,
                           const SCPCOA& scpcoa);
    void fillDerivedFields(const RMA& rma, const Vector3& scp, const PolyXYZ& arpPoly);
    void fillDerivedFields(const RgAzComp& rgAzComp,
            const GeoData& geoData,
            const SCPCOA& scpcoa,
            double fc);
    void fillDefaultFields(const RMA& rma, double fc);
    void fillDefaultFields(const PFA& pfa, double fc);
private:
    bool validateTimeCOAPoly(
            const CollectionInformation& collectionInformation,
            logging::Logger& log) const;
    bool validateFFTSigns(logging::Logger& log) const;
    bool validate(const RMAT& rmat, const Vector3& scp,
            double fc, logging::Logger& log) const;
    bool validate(const RMCR& rmcr, const Vector3& scp,
            double fc, logging::Logger& log) const;
    bool validate(const INCA& inca, const Vector3& scp,
            const PolyXYZ& arpPoly, double fc,
            logging::Logger& log) const;
    void fillDerivedFields(const RMAT& rmat, const Vector3& scp);
    void fillDerivedFields(const RMCR& rmcr, const Vector3& scp);
    void fillDerivedFields(const INCA& inca, const Vector3& scp,
            const PolyXYZ& arpPoly);
    void fillDefaultFields(const RMAT& rmat, double fc);
    void fillDefaultFields(const RMCR& rmcr, double fc);
    double derivedColKCenter(const RMAT& rmat, double fc) const;
    double derivedRowKCenter(const RMAT& rmat, double fc) const;
    double derivedRowKCenter(const RMCR& rmcr, double fc) const;
    double derivedRowKCenter(const INCA& inca) const;
    ComplexImageGridType defaultGridType(const RMA& rma) const;
    ComplexImagePlaneType defaultPlaneType(const RMA& rma) const;

    Vector3 derivedRowUnitVector(const RMAT& rmat, const Vector3& scp) const;
    Vector3 derivedColUnitVector(const RMAT& rmat, const Vector3& scp) const;

    Vector3 derivedRowUnitVector(const RMCR& rmcr, const Vector3& scp) const;
    Vector3 derivedColUnitVector(const RMCR& rmcr, const Vector3& scp) const;

    Vector3 derivedRowUnitVector(const INCA& inca, const Vector3& scp,
            const PolyXYZ& arpPoly) const;
    Vector3 derivedColUnitVector(const INCA& inca, const Vector3& scp,
            const PolyXYZ& arpPoly) const;

    Vector3 derivedRowUnitVector(const SCPCOA& scpcoa,
            const Vector3& scp) const;

    Vector3 derivedColUnitVector(const SCPCOA& scpcoa,
        const Vector3& scp) const;

    static const double UVECT_TOL;
    static const double WF_TOL;
    static const char WF_INCONSISTENT_STR[];
    static const char BOUNDS_ERROR_MESSAGE[];
};

}
}
#endif

