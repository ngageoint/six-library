#ifndef __SCENE_GRID_GEOMETRY_H__
#define __SCENE_GRID_GEOMETRY_H__

#include "scene/Types.h"
#include "scene/Utilities.h"

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
        virtual ~GridGeometry() {}

        virtual Vector3 rowColToECEF(double row,
                                     double col) const = 0;

        virtual RowCol<double> ecefToRowCol(const Vector3& p3) const
        {
            throw except::NotImplementedException(Ctxt("ecefToRowCol not implemented for this subclass"));
        }


    };


    class PlanarGridGeometry : public GridGeometry
    {
    protected:
        Vector3 mRow;
        Vector3 mCol;
        Vector3 mRefPt;
    public:
        PlanarGridGeometry(double sampleSpacingRows,
                           double sampleSpacingCols,
                           double sceneCenterRow,
                           double sceneCenterCol,
                           const Vector3& row,
                           const Vector3& col,
                           const Vector3& refPt) :
            GridGeometry(sampleSpacingRows, sampleSpacingCols,
                         sceneCenterRow, sceneCenterCol),
            mRow(row), mCol(col),
            mRefPt(refPt) {}
         
        ~PlanarGridGeometry() {} 
        
        Vector3 rowColToECEF(double row,
                             double col) const;

        RowCol<double> ecefToRowCol(const Vector3& p3) const;
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
        
        Vector3 rowColToECEF(double row,
                             double col) const;

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
        
        Vector3 rowColToECEF(double row,
                             double col) const;


    };


}

#endif
