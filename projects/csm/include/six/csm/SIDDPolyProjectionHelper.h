#ifndef __SIX_CSM_SIDD_POLY_PROJECTION_HELPER_H__
#define __SIX_CSM_SIDD_POLY_PROJECTION_HELPER_H__

#include <six/sidd/DerivedData.h>
#include <six/csm/SensorModelHelper.h>

namespace six
{
namespace CSM
{
class SIDDPolyProjectionHelper : public SensorModelHelper
{
public:
    SIDDPolyProjectionHelper(mem::SharedPtr<six::sidd::DerivedData> data);

    virtual csm::ImageCoord groundToImage(
            const csm::EcefCoord& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual csm::ImageCoordCovar groundToImage(
            const csm::EcefCoordCovar& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* ) const;

    virtual csm::EcefCoord imageToGround(
            const csm::ImageCoord& imagePt,
            double height,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual csm::EcefCoordCovar imageToGround(
            const csm::ImageCoordCovar& imagePt,
            double height,
            double heightVariance,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual double getImageTime(const csm::ImageCoord& imagePt) const;

    virtual
    csm::EcefCoord getSensorPosition(const csm::ImageCoord& imagePt) const;

    virtual csm::EcefCoord getSensorPosition(double time) const;

    virtual csm::EcefVector
    getSensorVelocity(const csm::ImageCoord& imagePt) const;

    virtual csm::EcefVector getSensorVelocity(double time) const;

private:
    double getCenterTime() const
    {
        // TODO: If there's more than one collection, what should we use?
        const double duration =
                mData->exploitationFeatures->collections[0]->information->
                        collectionDuration;
        return (duration / 2);
    }

private:
    const mem::SharedPtr<six::sidd::DerivedData> mData;
    const six::sidd::PolynomialProjection* mProjection;
};
}
}

#endif
