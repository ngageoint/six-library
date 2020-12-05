#include <algorithm>
#include <six/sicd/AreaPlaneUtility.h>
#include <six/sicd/Utilities.h>
#include <math/ConvexHull.h>

namespace six
{
namespace sicd
{
const double AreaPlaneUtility::DEFAULT_SAMPLE_DENSITY = 1.5;
std::vector<RowColDouble > AreaPlaneUtility::computeCornersPix(
        const ComplexData& data)
{
    std::vector<RowColDouble > cornersPix;
    //! This checks whether we are working with a full or cropped product.
    //  If it's a full product with a valid data poly, use the polygon to
    //  mask out the invalid region during projection.
    //  If it's a cropped product, use the full extent. Here we assume the
    //  cropped region is in the valid region, and do not perform a check.
    if (data.imageData->validData.size() >= 3 &&
        (static_cast<sys::SSize_T>(data.imageData->numRows) ==
        data.imageData->fullImage.row) &&
        (static_cast<sys::SSize_T>(data.imageData->numCols) ==
        data.imageData->fullImage.col))
    {
        std::vector<types::RowCol<sys::SSize_T> > validPixelRegion(
                data.imageData->validData.size());
        for (size_t ii = 0; ii < validPixelRegion.size(); ++ii)
        {
            validPixelRegion[ii] = data.imageData->validData[ii];
        }

        std::vector<types::RowCol<sys::SSize_T> > hullPoints;
        math::ConvexHull<sys::SSize_T> convexHull(validPixelRegion, hullPoints);

        // ConvexHull makes a copy of the first vertex into the last,  so pop
        // off the last entry
        cornersPix.resize(hullPoints.size() - 1);
        for (size_t ii = 0; ii < hullPoints.size() - 1; ++ii)
        {
            cornersPix[ii] = hullPoints[ii];
        }
    }
    else
    {
        // Use the grid corners
        cornersPix.push_back(RowColDouble(0.0, 0.0));
        cornersPix.push_back(RowColDouble(
                0.0, data.getNumCols() - 1.0));
        cornersPix.push_back(RowColDouble(
                data.getNumRows() - 1.0, data.getNumCols() - 1.0));
        cornersPix.push_back(RowColDouble(
                data.getNumRows() - 1.0, 0.0));
    }

    return cornersPix;
}

types::RowCol<Vector3> AreaPlaneUtility::deriveUnitVectors(
        const ComplexData& data)
{
    std::unique_ptr<scene::SceneGeometry> geometry(
            Utilities::getSceneGeometry(&data));

    types::RowCol<Vector3> unitVectors;
    const Vector3 slantPlaneX = geometry->getSlantPlaneX();
    const Vector3 normal = geometry->getGroundPlaneNormal();
    unitVectors.row = slantPlaneX - normal * slantPlaneX.dot(normal);
    unitVectors.row.normalize();
    unitVectors.row *= -1;

    unitVectors.col = math::linear::cross(normal, unitVectors.row);
    return unitVectors;
}

RowColDouble AreaPlaneUtility::deriveSpacing(
        const ComplexData& data,
        const types::RowCol<Vector3>& unitVectors,
        double sampleDensity)
{
    std::unique_ptr<scene::SceneGeometry> geometry(
            Utilities::getSceneGeometry(&data));
    geometry->setImageVectors(unitVectors.row, unitVectors.col);
    geometry->setOutputPlaneVectors(unitVectors.row, unitVectors.col);

    const RowColDouble origResolution(
            data.grid->row->impulseResponseWidth,
            data.grid->col->impulseResponseWidth);
    const RowColDouble bandwidth(
            data.grid->row->impulseResponseBandwidth,
            data.grid->col->impulseResponseBandwidth);
    const RowColDouble k = (origResolution * bandwidth) /
            Constants::EXCESS_BANDWIDTH_FACTOR;
    const types::RgAz<double> resolution(origResolution.row / k.row,
            origResolution.col / k.col);
    const RowColDouble groundResolution =
            geometry->getGroundResolution(resolution);

    RowColDouble spacing;
    spacing.row = spacing.col = std::max(groundResolution.row,
            groundResolution.col) / sampleDensity;
    return spacing;
}

std::vector<Vector3> AreaPlaneUtility::computeInPlaneCorners(
        const ComplexData& data,
        const types::RowCol<Vector3>& unitVectors)
{
    std::unique_ptr<scene::SceneGeometry> geometry(
            Utilities::getSceneGeometry(&data));
    geometry->setImageVectors(unitVectors.row, unitVectors.col);
    geometry->setOutputPlaneVectors(unitVectors.row, unitVectors.col);
    std::unique_ptr<scene::ProjectionModel> projection(
            Utilities::getProjectionModel(&data, geometry.get()));

    std::vector<RowColDouble > cornersPix = computeCornersPix(data);
    std::vector<Vector3> corners(cornersPix.size());
    for (size_t ii = 0; ii < cornersPix.size(); ++ii)
    {
        // Offset the pixel corners relative to start chip
        cornersPix[ii].row += data.imageData->firstRow;
        cornersPix[ii].col += data.imageData->firstCol;

        const RowColDouble sampleSpacing(
                data.grid->row->sampleSpacing,
                data.grid->col->sampleSpacing);

        const RowColDouble imageGridPoint =
                (cornersPix[ii] - data.imageData->scpPixel) * sampleSpacing;

        corners[ii] = projection->imageToScene(imageGridPoint,
                data.geoData->scp.ecf,
                geometry->getGroundPlaneNormal());
    }
    return corners;
}

types::RgAz<std::vector<double> > AreaPlaneUtility::computeMetersFromCenter(
        const ComplexData& data,
        const types::RowCol<Vector3>& unitVectors)
{
    std::vector<Vector3> inPlaneCornersECEF =
            computeInPlaneCorners(data, unitVectors);

    types::RgAz<std::vector<double> > metersFromCenter(
            std::vector<double>(inPlaneCornersECEF.size()),
            std::vector<double>(inPlaneCornersECEF.size()));
    for (size_t ii = 0; ii < inPlaneCornersECEF.size(); ++ii)
    {
        metersFromCenter.rg[ii] = unitVectors.row.dot(
                inPlaneCornersECEF[ii] - data.geoData->scp.ecf);
        metersFromCenter.az[ii] = unitVectors.col.dot(
                inPlaneCornersECEF[ii] - data.geoData->scp.ecf);
    }

    std::sort(metersFromCenter.rg.begin(), metersFromCenter.rg.end());
    std::sort(metersFromCenter.az.begin(), metersFromCenter.az.end());

    return metersFromCenter;
}

types::RowCol<size_t> AreaPlaneUtility::derivePlaneDimensions(
        const types::RgAz<std::vector<double> >& sortedMetersFromCenter,
        const RowColDouble& spacing)
{
    const types::RowCol<size_t> lastIndex(
            sortedMetersFromCenter.rg.size() - 1,
            sortedMetersFromCenter.az.size() - 1);
    const double rowDistance = std::abs(
            sortedMetersFromCenter.rg[lastIndex.row] -
            sortedMetersFromCenter.rg[0]);
    const double colDistance = std::abs(
            sortedMetersFromCenter.az[lastIndex.col] -
            sortedMetersFromCenter.az[0]);

    types::RowCol<size_t> dimensions;
    dimensions.row = static_cast<size_t>(
            std::ceil(std::max<double>(1.0, rowDistance / spacing.row)));
    dimensions.col = static_cast<size_t>(
            std::ceil(std::max<double>(1.0, colDistance / spacing.col)));
    return dimensions;
}

RowColDouble AreaPlaneUtility::deriveReferencePoint(
        const types::RgAz<std::vector<double> >& sortedMetersFromCenter,
        const RowColDouble& spacing)
{
    RowColDouble referencePoint;
    referencePoint.row = std::abs(sortedMetersFromCenter.rg[0]) / spacing.row;
    referencePoint.col = std::abs(sortedMetersFromCenter.az[0]) / spacing.col;
    return referencePoint;
}

bool AreaPlaneUtility::hasAreaPlane(const ComplexData& data)
{
    return (data.radarCollection.get() &&
            data.radarCollection->area.get() &&
            data.radarCollection->area->plane.get());
}

void AreaPlaneUtility::setAreaPlane(ComplexData& data,
        bool includeSegmentList,
        double sampleDensity)
{
    if (!data.radarCollection.get())
    {
        data.radarCollection.reset(new RadarCollection());
    }
    if (!data.radarCollection->area.get())
    {
        data.radarCollection->area.reset(new Area());
    }
    if (!data.radarCollection->area->plane.get())
    {
        data.radarCollection->area->plane.reset(new AreaPlane());
        AreaPlane& areaPlane = *data.radarCollection->area->plane;
        deriveAreaPlane(data, areaPlane, includeSegmentList, sampleDensity);
        if (includeSegmentList)
        {
            data.imageFormation->segmentIdentifier = "AA";
        }

        std::vector<RowColDouble> imageCorners(4);
        imageCorners[0] = RowColDouble(0.0, 0.0);
        imageCorners[1] = RowColDouble(0.0, data.getNumCols() - 1.0);
        imageCorners[2] = RowColDouble(data.getNumRows() - 1.0,
                data.getNumCols() - 1.0);
        imageCorners[3] = RowColDouble(data.getNumRows() - 1.0, 0.0);
        LatLonAltCorners& acpCorners =
                data.radarCollection->area->acpCorners;

        std::unique_ptr<scene::SceneGeometry> geometry(
                Utilities::getSceneGeometry(&data));
        std::unique_ptr<scene::ProjectionModel> projectionModel(
                Utilities::getProjectionModel(&data, geometry.get()));
        const Vector3 groundPlaneNormal = Utilities::getGroundPlaneNormal(data);
        for (size_t ii = 0; ii < imageCorners.size(); ++ii)
        {
            const Vector3 groundPoint = projectionModel->imageToScene(
                    imageCorners[ii],
                    geometry->getReferencePosition(),
                    groundPlaneNormal);
            acpCorners.getCorner(ii) = scene::Utilities::ecefToLatLon(
                    groundPoint);
        }
    }
}

void AreaPlaneUtility::deriveAreaPlane(const ComplexData& data,
        AreaPlane& areaPlane,
        bool includeSegmentList,
        double sampleDensity)
{
    areaPlane.xDirection.reset(new AreaDirectionParameters());
    areaPlane.yDirection.reset(new AreaDirectionParameters());

    const types::RowCol<Vector3> unitVectors = deriveUnitVectors(data);
    areaPlane.xDirection->unitVector = unitVectors.row;
    areaPlane.yDirection->unitVector = unitVectors.col;

    const RowColDouble spacing = deriveSpacing(data, unitVectors, sampleDensity);
    areaPlane.xDirection->spacing = spacing.row;
    areaPlane.yDirection->spacing = spacing.col;

    areaPlane.xDirection->first = 0;
    areaPlane.yDirection->first = 0;

    const types::RgAz<std::vector<double> > metersFromCenter =
            computeMetersFromCenter(data, unitVectors);

    areaPlane.referencePoint.rowCol = deriveReferencePoint(
            metersFromCenter, spacing);
    areaPlane.referencePoint.ecef = data.geoData->scp.ecf;

    const types::RowCol<size_t> origDims = derivePlaneDimensions(
            metersFromCenter, spacing);
    areaPlane.xDirection->elements = origDims.row;
    areaPlane.yDirection->elements = origDims.col;

    switch (data.collectionInformation->radarMode)
    {
        case RadarModeType::SPOTLIGHT:
            areaPlane.orientation = OrientationType::DOWN;
            break;
        case RadarModeType::STRIPMAP:
        case RadarModeType::DYNAMIC_STRIPMAP:
            areaPlane.orientation = OrientationType::ARBITRARY;
            break;
        default:
            areaPlane.orientation = OrientationType::NOT_SET;
            break;
    }

    if (includeSegmentList)
    {
        areaPlane.segmentList.resize(1);
        areaPlane.segmentList[0].reset(new Segment());
        areaPlane.segmentList[0]->startLine = 0;
        areaPlane.segmentList[0]->startSample = 0;

        // End values are inclusive
        areaPlane.segmentList[0]->endLine =
                static_cast<int>(areaPlane.xDirection->elements - 1);
        areaPlane.segmentList[0]->endSample =
                static_cast<int>(areaPlane.yDirection->elements - 1);

        areaPlane.segmentList[0]->identifier = "AA";
    }
}

}
}

