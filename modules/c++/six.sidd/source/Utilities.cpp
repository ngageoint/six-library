#include "six/sidd/Utilities.h"

using namespace six;
using namespace six::sidd;

scene::SceneGeometry*
Utilities::getSceneGeometry(DerivedData* derived)
{

    if (! derived->measurement->projection->isMeasurable() )
	throw except::Exception(Ctxt("Projection is not measurable"));

    MeasurableProjection* measurableProjection =
	(MeasurableProjection*)derived->measurement->projection;

    Poly2D timeCOAPoly = measurableProjection->timeCOAPoly;
    ReferencePoint ref = measurableProjection->referencePoint;
    PolyXYZ arpPoly = derived->measurement->arpPoly;

    Vector3 refPos = ref.ecef;
    double scpTime = timeCOAPoly(ref.rowCol.row, ref.rowCol.col);

    Vector3 arpPos = arpPoly(scpTime);
    PolyXYZ arpVelPoly = arpPoly.derivative();
    Vector3 arpVel = arpVelPoly(scpTime);

    scene::SceneGeometry* geom = new scene::SceneGeometry(arpVel, arpPos,
            refPos);

    PlaneProjection* proj =
            (PlaneProjection*) derived->measurement->projection;

    // These vectors are junk as soon as derived object goes away
    geom->setImageVectors(&(proj->productPlane.rowUnitVector),
            &(proj->productPlane.colUnitVector));

    return geom;
}


void Utilities::setProductValues(Poly2D timeCOAPoly,
                                 PolyXYZ arpPoly, 
                                 ReferencePoint ref, 
                                 Vector3* row,
                                 Vector3* col, 
                                 RangeAzimuth<double> res,
                                 Product* product)
{
    double scpTime = timeCOAPoly(ref.rowCol.row, ref.rowCol.col);
    
    Vector3 arpPos = arpPoly(scpTime);
    PolyXYZ arpVelPoly = arpPoly.derivative();
    Vector3 arpVel = arpVelPoly(scpTime);
    
    setProductValues(arpVel, arpPos, ref.ecef, row, col, res, product);
}

void Utilities::setProductValues(Vector3 arpVel, 
                                 Vector3 arpPos,
                                 Vector3 refPos, 
                                 Vector3* row, 
                                 Vector3* col,
                                 RangeAzimuth<double> res, 
                                 Product* product)
{
    scene::SceneGeometry* sceneGeom = new scene::SceneGeometry(arpVel, arpPos,
            refPos);
    sceneGeom->setImageVectors(row, col);

    //do some setup of derived data from geometry
    if (product->north == Init::undefined<double>())
    {
        product->north = sceneGeom->getNorthAngle();
    }

    //if (product->resolution
    //    == Init::undefined<RowColDouble>())
    {
        sceneGeom->getGroundResolution(res.range, res.azimuth,
                product->resolution.row, product->resolution.col);
    }

    delete sceneGeom;
}

void Utilities::setCollectionValues(Poly2D timeCOAPoly,
                                    PolyXYZ arpPoly, 
                                    ReferencePoint ref, 
                                    Vector3* row,
                                    Vector3* col, 
                                    Collection* collection)
{
    double scpTime = timeCOAPoly(ref.rowCol.row, ref.rowCol.col);

    Vector3 arpPos = arpPoly(scpTime);
    PolyXYZ arpVelPoly = arpPoly.derivative();
    Vector3 arpVel = arpVelPoly(scpTime);

    setCollectionValues(arpVel, arpPos, ref.ecef, row, col, collection);
}

void Utilities::setCollectionValues(Vector3 arpVel,
        Vector3 arpPos, Vector3 refPos, Vector3* row,
        Vector3* col, Collection* collection)
{
    scene::SceneGeometry* sceneGeom = new scene::SceneGeometry(arpVel, arpPos,
            refPos);
    sceneGeom->setImageVectors(row, col);

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
        collection->geometry->slope = sceneGeom->getSlopeAngle();
    }
    if (collection->geometry->squint == Init::undefined<double>())
    {
        collection->geometry->squint = sceneGeom->getSquintAngle();
    }
    if (collection->geometry->graze == Init::undefined<double>())
    {
        collection->geometry->graze = sceneGeom->getGrazingAngle();
    }
    if (collection->geometry->tilt == Init::undefined<double>())
    {
        collection->geometry->tilt = sceneGeom->getTiltAngle();
    }
    if (collection->geometry->azimuth == Init::undefined<double>())
    {
        collection->geometry->azimuth = sceneGeom->getAzimuthAngle();
    }
    if (collection->phenomenology->multiPath == Init::undefined<double>())
    {
        collection->phenomenology->multiPath = sceneGeom->getMultiPathAngle();
    }
    if (collection->phenomenology->groundTrack == Init::undefined<double>())
    {
        collection->phenomenology->groundTrack = sceneGeom->getImageAngle(
                sceneGeom->getGroundTrack());
    }

    if (collection->phenomenology->shadow == Init::undefined<
            AngleMagnitude>())
    {
        collection->phenomenology->shadow = sceneGeom->getShadow();
    }

    if (collection->phenomenology->layover == Init::undefined<
            AngleMagnitude>())
    {
        collection->phenomenology->layover = sceneGeom->getLayover();
    }

    delete sceneGeom;
}
