#include "scene/GridGeometry.h"


scene::Vector3 scene::PlanarGridGeometry::rowColToECEF(double row,
                                                 double col) const
{
    scene::Vector3 rowDisp = 
        mRow * mSampleSpacingRows * (row - mSceneCenterRow);
    
    
    scene::Vector3 colDisp = 
        mCol * mSampleSpacingCols * (col - mSceneCenterCol);
    
    return mRefPt + rowDisp + colDisp;
}

scene::RowCol<double> scene::PlanarGridGeometry::ecefToRowCol(const scene::Vector3& p3) const
{
    scene::Vector3 disp(p3 - mRefPt);
    scene::RowCol<double> rgAz(disp.dot(mRow), disp.dot(mCol));
    return scene::RowCol<double>(rgAz.row / mSampleSpacingRows + mSceneCenterRow,
                                 rgAz.col / mSampleSpacingCols + mSceneCenterCol);

}

scene::Vector3 scene::CylindricalGridGeometry::rowColToECEF(double row,
                                                            double col) const
{
    double theta = mSampleSpacingCols * (col - mSceneCenterCol) / mRs;
    
    scene::Vector3 rowDisp = 
        mRow * mSampleSpacingRows * (row - mSceneCenterRow);
    
    scene::Vector3 colDisp = 
        mCol * mRs * sin(theta);
       
    scene::Vector3 hgtDisp =
        mNormal * mRs * ( cos(theta) - 1 );
    return mRefPt + rowDisp + colDisp + hgtDisp;

}

scene::Vector3 scene::GeographicGridGeometry::rowColToECEF(double row,
                                                           double col) const
{
    scene::LatLonAlt lla(
        mRefPt.getLat() - (row - mSceneCenterRow) *
        mSampleSpacingRows / 3600.,
        
        mRefPt.getLon() + (col - mSceneCenterCol) *
        mSampleSpacingCols / 3600.,
        
        mRefPt.getAlt()
        );

    return scene::Utilities::latLonToECEF(lla);
}
