#include "six/sidd/Utilities.h"

using namespace six;

scene::SceneGeometry*
six::sidd::Utilities::getSceneGeometry(DerivedData* derived)
{
    double centerTime;
    if (derived->measurement->projection->isMeasurable())
    {
        six::sidd::MeasurableProjection
                *projection =
                        (six::sidd::MeasurableProjection*) derived->measurement->projection;
        centerTime
                = projection->timeCOAPoly(
                                          projection->referencePoint.rowCol.row,
                                          projection->referencePoint.rowCol.col);
    }
    else
    {
        // we estimate...
        centerTime
                = derived->exploitationFeatures->collections[0]->information->collectionDuration
                        / 2;
    }

    // compute arpPos and arpVel
    six::Vector3 arpPos = derived->measurement->arpPoly(centerTime);
    six::Vector3 arpVel =
            derived->measurement->arpPoly.derivative()(centerTime);
    six::Vector3 refPt = derived->measurement->projection->referencePoint.ecef;

    six::Vector3 *rowVec = NULL;
    six::Vector3 *colVec = NULL;

    if (derived->measurement->projection->projectionType
            == six::ProjectionType::POLYNOMIAL)
    {
        six::sidd::PolynomialProjection
                *projection =
                        (six::sidd::PolynomialProjection*) derived->measurement->projection;

        double cR = projection->referencePoint.rowCol.row;
        double cC = projection->referencePoint.rowCol.col;

        scene::LatLonAlt centerLLA;
        centerLLA.setLat(projection->rowColToLat(cR, cC));
        centerLLA.setLon(projection->rowColToLon(cR, cC));
        six::Vector3 centerEcef = scene::Utilities::latLonToECEF(centerLLA);

        scene::LatLonAlt downLLA;
        downLLA.setLat(projection->rowColToLat(cR + 1, cC));
        downLLA.setLon(projection->rowColToLon(cR + 1, cC));
        six::Vector3 downEcef = scene::Utilities::latLonToECEF(downLLA);

        scene::LatLonAlt rightLLA;
        rightLLA.setLat(projection->rowColToLat(cR, cC + 1));
        rightLLA.setLon(projection->rowColToLon(cR, cC + 1));
        six::Vector3 rightEcef = scene::Utilities::latLonToECEF(rightLLA);

        rowVec = new six::Vector3(downEcef - centerEcef);
        rowVec->normalize();
        colVec = new six::Vector3(rightEcef - centerEcef);
        colVec->normalize();
    }
    else if (derived->measurement->projection->projectionType
            == six::ProjectionType::PLANE)
    {
        six::sidd::PlaneProjection *projection =
                (six::sidd::PlaneProjection*) derived->measurement->projection;

        rowVec = new six::Vector3(projection->productPlane.rowUnitVector);
        colVec = new six::Vector3(projection->productPlane.colUnitVector);
    }
    else
    {
        throw except::Exception(
                                Ctxt(
                                     "Geographic and Cylindrical projectsions not yet supported"));
    }

    //    std::cout << "Center Time: " << centerTime << std::endl;
    //    std::cout << "ARP Center Pos: " << arpPos << std::endl;
    //    std::cout << "ARP Center Vel: " << arpVel << std::endl;
    //    std::cout << "Scene Center Pos: " << refPt << std::endl;
    //    std::cout << "Row Vec: " << rowVec << std::endl;
    //    std::cout << "Col Vec: " << colVec << std::endl;

    return new scene::SceneGeometry(arpVel, arpPos, refPt, rowVec, colVec, true);
}

void six::sidd::Utilities::setProductValues(Poly2D timeCOAPoly,
        PolyXYZ arpPoly, ReferencePoint ref, const Vector3* row,
        const Vector3* col, RangeAzimuth<double>res, Product* product)
{
    double scpTime = timeCOAPoly(ref.rowCol.row, ref.rowCol.col);

    Vector3 arpPos = arpPoly(scpTime);
    PolyXYZ arpVelPoly = arpPoly.derivative();
    Vector3 arpVel = arpVelPoly(scpTime);

    setProductValues(arpVel, arpPos, ref.ecef, row, col, res, product);
}

void six::sidd::Utilities::setProductValues(Vector3 arpVel, Vector3 arpPos,
        Vector3 refPos, const Vector3* row, const Vector3* col,
        RangeAzimuth<double>res, Product* product)
{
    scene::SceneGeometry sceneGeom(arpVel, arpPos, refPos);
    sceneGeom.setImageVectors(row, col);

    //do some setup of derived data from geometry
    if (product->north == Init::undefined<double>())
    {
        product->north = sceneGeom.getNorthAngle();
    }

    //if (product->resolution
    //    == Init::undefined<RowColDouble>())
    {
        sceneGeom.getGroundResolution(res.range, res.azimuth,
                                      product->resolution.row,
                                      product->resolution.col);
    }
}

void six::sidd::Utilities::setCollectionValues(Poly2D timeCOAPoly,
        PolyXYZ arpPoly, ReferencePoint ref, const Vector3* row,
        const Vector3* col, Collection* collection)
{
    double scpTime = timeCOAPoly(ref.rowCol.row, ref.rowCol.col);

    Vector3 arpPos = arpPoly(scpTime);
    PolyXYZ arpVelPoly = arpPoly.derivative();
    Vector3 arpVel = arpVelPoly(scpTime);

    setCollectionValues(arpVel, arpPos, ref.ecef, row, col, collection);
}

void six::sidd::Utilities::setCollectionValues(Vector3 arpVel, Vector3 arpPos,
        Vector3 refPos, const Vector3* row, const Vector3* col,
        Collection* collection)
{
    scene::SceneGeometry sceneGeom(arpVel, arpPos, refPos);
    sceneGeom.setImageVectors(row, col);

    if (collection->geometry == NULL)
    {
        collection->geometry = new Geometry();
    }
    if (collection->phenomenology == NULL)
    {
        collection->phenomenology = new Phenomenology();
    }

    if (collection->geometry->slope == Init::undefined<double>())
    {
        collection->geometry->slope = sceneGeom.getSlopeAngle();
    }
    if (collection->geometry->squint == Init::undefined<double>())
    {
        collection->geometry->squint = sceneGeom.getSquintAngle();
    }
    if (collection->geometry->graze == Init::undefined<double>())
    {
        collection->geometry->graze = sceneGeom.getGrazingAngle();
    }
    if (collection->geometry->tilt == Init::undefined<double>())
    {
        collection->geometry->tilt = sceneGeom.getTiltAngle();
    }
    if (collection->geometry->azimuth == Init::undefined<double>())
    {
        collection->geometry->azimuth = sceneGeom.getAzimuthAngle();
    }
    if (collection->phenomenology->multiPath == Init::undefined<double>())
    {
        collection->phenomenology->multiPath = sceneGeom.getMultiPathAngle();
    }
    if (collection->phenomenology->groundTrack == Init::undefined<double>())
    {
        collection->phenomenology->groundTrack
                = sceneGeom.getImageAngle(sceneGeom.getGroundTrack());
    }

    if (collection->phenomenology->shadow == Init::undefined<AngleMagnitude>())
    {
        collection->phenomenology->shadow = sceneGeom.getShadow();
    }

    if (collection->phenomenology->layover == Init::undefined<AngleMagnitude>())
    {
        collection->phenomenology->layover = sceneGeom.getLayover();
    }
}

six::PolarizationType _convertDualPolarization(six::DualPolarizationType pol,
        bool useFirst)
{
    switch (pol)
    {
    case six::DualPolarizationType::OTHER:
        return six::PolarizationType::OTHER;
    case six::DualPolarizationType::V_V:
        return six::PolarizationType::V;
    case six::DualPolarizationType::V_H:
        return useFirst ? six::PolarizationType::V : six::PolarizationType::H;
    case six::DualPolarizationType::H_V:
        return useFirst ? six::PolarizationType::H : six::PolarizationType::V;
    case six::DualPolarizationType::H_H:
        return six::PolarizationType::H;
    case six::DualPolarizationType::RHC_RHC:
        return six::PolarizationType::RHC;
    case six::DualPolarizationType::RHC_LHC:
        return useFirst ? six::PolarizationType::RHC
                        : six::PolarizationType::LHC;
    case six::DualPolarizationType::LHC_RHC:
        return useFirst ? six::PolarizationType::LHC
                        : six::PolarizationType::RHC;
    case six::DualPolarizationType::LHC_LHC:
        return six::PolarizationType::LHC;
    default:
        return six::PolarizationType::NOT_SET;
    }
}

std::pair<six::PolarizationType, six::PolarizationType> six::sidd::Utilities::convertDualPolarization(
        six::DualPolarizationType pol)
{
    std::pair<six::PolarizationType, six::PolarizationType> pols;
    pols.first = _convertDualPolarization(pol, true);
    pols.second = _convertDualPolarization(pol, false);
    return pols;
}
