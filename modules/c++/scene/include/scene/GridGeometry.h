#ifndef __SCENE_GRID_GEOMETRY_H__
#define __SCENE_GRID_GEOMETRY_H__

#include <types/RowCol.h>
#include "scene/Utilities.h"
#include "scene/LocalCoordinateTransform.h"
#include <import/math/poly.h>

namespace scene
{
class GridGeometry
{
protected:
    double  mSampleSpacingRows;
    double  mSampleSpacingCols;
    double  mSceneCenterRow;
    double  mSceneCenterCol;

    GridGeometry(double sampleSpacingRows,
                 double sampleSpacingCols,
                 double sceneCenterRow,
                 double sceneCenterCol) :
        mSampleSpacingRows(sampleSpacingRows),
        mSampleSpacingCols(sampleSpacingCols),
        mSceneCenterRow(sceneCenterRow),
        mSceneCenterCol(sceneCenterCol) {}
public:

    enum { MAX_ITER = 50 };

    virtual ~GridGeometry() {}

    virtual Vector3 rowColToECEF(double row, double col) const = 0;

    virtual types::RowCol<double> ecefToRowCol(const Vector3& p3) const
    {
        throw except::NotImplementedException(Ctxt("ecefToRowCol not implemented for this subclass"));
    }

    // TODO move these into just the Planar derived class if they are not needed by the other subclasses

    /**
     *  Project a point from the ground to the geometric grid.
     *
     * \param groundPt    A point lying in the scene, defined in ECEF coordinates.
     */
    virtual Vector3 gridToScene(const Vector3& gridPt,
                                double height) const
    {
        throw except::NotImplementedException(Ctxt(
                "gridToScene not implemented for this subclass"));
    }

    /**
     *  Project a point from the ground to the geometric grid.
     *
     * \param groundPt    A point lying in the scene, defined in ECEF coordinates.
     */
    virtual Vector3 sceneToGrid(const Vector3& groundPt) const
    {
        throw except::NotImplementedException(Ctxt(
                "sceneToGrid not implemented for this subclass"));
    }

};


class PlanarGridGeometry : public GridGeometry
{
protected:
    Vector3 mRow;
    Vector3 mCol;
    Vector3 mRefPt;
    math::poly::OneD<Vector3> mARPPoly;
    math::poly::OneD<Vector3> mARPVelPoly;
    math::poly::TwoD<double> mTimeCOAPoly;

    /**
     *  Project a point from the ground to the geometric grid, using
     *  range-doppler projection at the given time.  Both the input scene
     *  point and the output grid point are in ECEF coordinates.
     *
     * \param groundPt    A point lying in the scene, defined in ECEF coordinates.
     * \param time        The collection time to use when projecting the ground point into the geometric grid.
     */
    Vector3 sceneToGrid(const Vector3& groundPt, double time) const;

public:
    PlanarGridGeometry(double sampleSpacingRows,
                       double sampleSpacingCols,
                       double sceneCenterRow,
                       double sceneCenterCol,
                       const Vector3& row,
                       const Vector3& col,
                       const Vector3& refPt,
                       const math::poly::OneD<Vector3>& arpPoly,
                       const math::poly::TwoD<double>& timeCOAPoly) :
        GridGeometry(sampleSpacingRows, sampleSpacingCols,
                     sceneCenterRow, sceneCenterCol),
        mRow(row), mCol(col),
        mRefPt(refPt),
        mARPPoly(arpPoly),
        mTimeCOAPoly(timeCOAPoly)
    {
        mARPVelPoly = mARPPoly.derivative();
    }

    ~PlanarGridGeometry() {}

    virtual Vector3 rowColToECEF(double row, double col) const;

    virtual types::RowCol<double> ecefToRowCol(const Vector3& p3) const;

    /**
     *  Project a point from the geometric grid to the ground, using
     *  range-doppler projection.  Both the input grid point and the
     *  output scene point are in ECEF coordinates.  Height refers to the
     *  desired altitude of the scene point above the earth ellipsoid model.
     *
     * \param gridPt    A point lying on the geometric grid, defined in ECEF coordinates.
     * \param height    The height of the desired ground point in the scene
     */
    virtual Vector3 gridToScene(const Vector3& gridPt,
                                double height) const;

    /**
     *  This function calls the protected version of sceneToGrid
     *  iteratively, beginning with the center of aperture and adjusting
     *  the time value until the algorithm converges on a point in the
     *  geometric grid.
     *
     * \param groundPt    A point lying in the scene, defined in ECEF coordinates.
     */
    virtual Vector3 sceneToGrid(const Vector3& groundPt) const;
};

class CylindricalGridGeometry : public GridGeometry
{
protected:
    Vector3 mRow;
    Vector3 mCol;
    Vector3 mNormal;
    Vector3 mRefPt;
    double  mRs;
public:
    CylindricalGridGeometry(double sampleSpacingRows,
                            double sampleSpacingCols,
                            double sceneCenterRow,
                            double sceneCenterCol,
                            const Vector3& row,
                            const Vector3& col,
                            const Vector3& normal,
                            const Vector3& refPt,
                            double radiusOfCurvStripmap) :
        GridGeometry(sampleSpacingRows, sampleSpacingCols,
                     sceneCenterRow, sceneCenterCol),
        mRow(row), mCol(col), mNormal(normal),
        mRefPt(refPt),
        mRs(radiusOfCurvStripmap) {}

    ~CylindricalGridGeometry() {}

    virtual Vector3 rowColToECEF(double row, double col) const;
};


class GeographicGridGeometry : public GridGeometry
{
protected:
    LatLonAlt mRefPt;
public:
    GeographicGridGeometry(double sampleSpacingRows,
                           double sampleSpacingCols,
                           double sceneCenterRow,
                           double sceneCenterCol,
                           const LatLonAlt& refPt) :
        GridGeometry(sampleSpacingRows, sampleSpacingCols,
                     sceneCenterRow, sceneCenterCol),
        mRefPt(refPt) {}


    ~GeographicGridGeometry() {}

    virtual Vector3 rowColToECEF(double row, double col) const;

    // NOTE: The altitude information is ignored in this calculation - it is
    //       assumed to lie on the GGD surface.  If p3 is above or below the
    //       surface, the row/col that's returned will correspond to the same
    //       lat/lon, but its altitude will be assumed to be the same as the
    //       reference point.  Taking this row/col and converting back to ECEF
    //       via rowColToECEF() will then result in a different ECEF than p3.
    //       If p3 does lie on the GGD surface, round-tripping will result in
    //       the same ECEF.
    virtual types::RowCol<double> ecefToRowCol(const Vector3& p3) const;
};
}

#endif
