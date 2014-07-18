/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#include "six/sidd/Utilities.h"

namespace
{
double getCenterTime(const six::sidd::DerivedData* derived)
{
    double centerTime;
    if (derived->measurement->projection->isMeasurable())
    {
        const six::sidd::MeasurableProjection* const projection =
            reinterpret_cast<const six::sidd::MeasurableProjection*>(
                derived->measurement->projection.get());

        centerTime = projection->timeCOAPoly(
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

    return centerTime;
}
}

namespace six
{
namespace sidd
{
scene::SideOfTrack
Utilities::getSideOfTrack(const DerivedData* derived)
{
    const double centerTime = getCenterTime(derived);

    // compute arpPos and arpVel
    const six::Vector3 arpPos = derived->measurement->arpPoly(centerTime);
    const six::Vector3 arpVel =
        derived->measurement->arpPoly.derivative()(centerTime);
    const six::Vector3 refPt =
        derived->measurement->projection->referencePoint.ecef;

    return scene::SceneGeometry(arpVel, arpPos, refPt).getSideOfTrack();
}

std::auto_ptr<scene::SceneGeometry>
Utilities::getSceneGeometry(const DerivedData* derived)
{
    const double centerTime = getCenterTime(derived);

    // compute arpPos and arpVel
    six::Vector3 arpPos = derived->measurement->arpPoly(centerTime);
    six::Vector3 arpVel =
            derived->measurement->arpPoly.derivative()(centerTime);
    six::Vector3 refPt = derived->measurement->projection->referencePoint.ecef;

    six::Vector3 rowVec;
    six::Vector3 colVec;

    if (derived->measurement->projection->projectionType
            == six::ProjectionType::POLYNOMIAL)
    {
        const six::sidd::PolynomialProjection* projection =
            reinterpret_cast<const six::sidd::PolynomialProjection*>(
                derived->measurement->projection.get());

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

        rowVec = downEcef - centerEcef;
        rowVec.normalize();
        colVec = rightEcef - centerEcef;
        colVec.normalize();
    }
    else if (derived->measurement->projection->projectionType
            == six::ProjectionType::PLANE)
    {
        const six::sidd::PlaneProjection* projection =
                reinterpret_cast<const six::sidd::PlaneProjection*>(
                    derived->measurement->projection.get());

        rowVec = projection->productPlane.rowUnitVector;
        colVec = projection->productPlane.colUnitVector;
    } else if (derived->measurement->projection->projectionType
            == six::ProjectionType::GEOGRAPHIC)
    {
    	std::auto_ptr<scene::SceneGeometry> geom(new scene::SceneGeometry(
    	            arpVel, arpPos, refPt));
    	return geom;
    }
    else
    {
        throw except::Exception(
                                Ctxt(
                                     "Cylindrical projections not yet supported"));
    }

    std::auto_ptr<scene::SceneGeometry> geom(new scene::SceneGeometry(
            arpVel, arpPos, refPt, rowVec, colVec));
    return geom;
}

std::auto_ptr<scene::GridECEFTransform>
Utilities::getGridECEFTransform(const DerivedData* derived)
{
    if (!derived->measurement->projection->isMeasurable())
    {
        throw except::Exception(Ctxt("Projection type is not measurable: " +
                derived->measurement->projection->projectionType.toString()));
    }

    const six::sidd::MeasurableProjection* p =
            reinterpret_cast<const six::sidd::MeasurableProjection*>(
                    derived->measurement->projection.get());

    std::auto_ptr<scene::GridECEFTransform> transform;

    switch ((int) p->projectionType)
    {
    case six::ProjectionType::PLANE:
    {
        const six::sidd::PlaneProjection* const planeP =
                reinterpret_cast<const six::sidd::PlaneProjection*>(p);

        transform.reset(new scene::PlanarGridECEFTransform(
                p->sampleSpacing,
                p->referencePoint.rowCol,
                planeP->productPlane.rowUnitVector,
                planeP->productPlane.colUnitVector,
                p->referencePoint.ecef));
        break;
    }

    case six::ProjectionType::GEOGRAPHIC:
/*
        // Not complete or tested yet
        return new scene::GeographicGridECEFTransform(p->sampleSpacing,
                                                 p->referencePoint.rowCol,
                                                 scene::Utilities::ecefToLatLon(p->referencePoint.ecef));
*/

    case six::ProjectionType::CYLINDRICAL:
/*
        // Not complete or tested yet
    {
        six::Vector3 stripmapDir =
                ((six::sidd::CylindricalProjection*)p)->stripmapDirection;
        six::Vector3 ecef = p->referencePoint.ecef;
        scene::ECEFToLLATransform ecefToLLA;
        scene::LatLonAlt lla = ecefToLLA.transform(ecef);

        scene::WGS84EllipsoidModel model;
        double lon = lla.getLonRadians();
        double lat = atan2(ecef[2], pow(1 - model.calculateFlattening(), 2) *
            sqrt(pow(ecef[0], 2) + pow(ecef[1], 2)));

        six::Vector3 e, n, u;
        e[0] = -sin(lon);
        e[1] = cos(lon);
        e[2] = 0;

        n[0] = -sin(lat) * cos(lon);
        n[1] = -sin(lat) * sin(lon);
        n[2] = cos(lat);

        u[0] = cos(lat) * cos(lon);
        u[1] = cos(lat) * sin(lon);
        u[2] = sin(lat);

        double a = atan2(e.dot(stripmapDir), n.dot(stripmapDir));
        six::Vector3 colVec = cos(a) * n + sin(a) * e;
        six::Vector3 rowVec = math::linear::cross(colVec, u);

        return new scene::CylindricalGridECEFTransform(p->sampleSpacing,
                                                  p->referencePoint.rowCol,
                                                  rowVec,
                                                  colVec,
                                                  u,
                                                  p->referencePoint.ecef,
                                                  ((six::sidd::CylindricalProjection*)p)->curvatureRadius);
    }
*/

    default:
        throw except::Exception(Ctxt("Invalid projection type: " +
                p->projectionType.toString()));

    }

    return transform;
}

std::auto_ptr<scene::GridGeometry>
Utilities::getGridGeometry(const DerivedData* derived)
{
    if (!derived->measurement->projection->isMeasurable())
    {
        throw except::Exception(Ctxt("Projection type is not measurable: " +
                derived->measurement->projection->projectionType.toString()));
    }

    const six::sidd::MeasurableProjection* p =
            reinterpret_cast<const six::sidd::MeasurableProjection*>(
                    derived->measurement->projection.get());

    std::auto_ptr<scene::GridGeometry> geom;

    // Only currently have an implementation for PGD
    switch ((int) p->projectionType)
    {
    case six::ProjectionType::PLANE:
    {
        const six::sidd::PlaneProjection* const planeP =
                reinterpret_cast<const six::sidd::PlaneProjection*>(p);

        geom.reset(new scene::PlanarGridGeometry(
                planeP->productPlane.rowUnitVector,
                planeP->productPlane.colUnitVector,
                p->referencePoint.ecef,
                derived->measurement->arpPoly,
                p->timeCOAPoly));
        break;
    }

    default:
        throw except::Exception(Ctxt("Invalid/unsupported projection type: " +
                p->projectionType.toString()));

    }

    return geom;
}

void Utilities::setProductValues(Poly2D timeCOAPoly,
        PolyXYZ arpPoly, ReferencePoint ref, const Vector3* row,
        const Vector3* col, types::RgAz<double>res, Product* product)
{
    double scpTime = timeCOAPoly(ref.rowCol.row, ref.rowCol.col);

    Vector3 arpPos = arpPoly(scpTime);
    PolyXYZ arpVelPoly = arpPoly.derivative();
    Vector3 arpVel = arpVelPoly(scpTime);

    setProductValues(arpVel, arpPos, ref.ecef, row, col, res, product);
}

void Utilities::setProductValues(Vector3 arpVel, Vector3 arpPos,
        Vector3 refPos, const Vector3* row, const Vector3* col,
        types::RgAz<double>res, Product* product)
{
    const scene::SceneGeometry sceneGeom(arpVel, arpPos, refPos, *row, *col);

    //do some setup of derived data from geometry
    if (product->north == Init::undefined<double>())
    {
        product->north = sceneGeom.getNorthAngle();
    }

    //if (product->resolution
    //    == Init::undefined<RowColDouble>())
    {
        product->resolution = sceneGeom.getGroundResolution(res);
    }
}

void Utilities::setCollectionValues(Poly2D timeCOAPoly,
        PolyXYZ arpPoly, ReferencePoint ref, const Vector3* row,
        const Vector3* col, Collection* collection)
{
    double scpTime = timeCOAPoly(ref.rowCol.row, ref.rowCol.col);

    Vector3 arpPos = arpPoly(scpTime);
    PolyXYZ arpVelPoly = arpPoly.derivative();
    Vector3 arpVel = arpVelPoly(scpTime);

    setCollectionValues(arpVel, arpPos, ref.ecef, row, col, collection);
}

void Utilities::setCollectionValues(Vector3 arpVel, Vector3 arpPos,
        Vector3 refPos, const Vector3* row, const Vector3* col,
        Collection* collection)
{
    const scene::SceneGeometry sceneGeom(arpVel, arpPos, refPos, *row, *col);

    if (collection->geometry.get() == NULL)
    {
        collection->geometry.reset(new Geometry());
    }
    if (collection->phenomenology.get() == NULL)
    {
        collection->phenomenology.reset(new Phenomenology());
    }

    if (collection->geometry->slope == Init::undefined<double>())
    {
        collection->geometry->slope = sceneGeom.getETPSlopeAngle();
    }
    if (collection->geometry->squint == Init::undefined<double>())
    {
        collection->geometry->squint = sceneGeom.getSquintAngle();
    }
    if (collection->geometry->graze == Init::undefined<double>())
    {
        collection->geometry->graze = sceneGeom.getETPGrazingAngle();
    }
    if (collection->geometry->tilt == Init::undefined<double>())
    {
        collection->geometry->tilt = sceneGeom.getETPTiltAngle();
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

std::pair<six::PolarizationType, six::PolarizationType> Utilities::convertDualPolarization(
        six::DualPolarizationType pol)
{
    std::pair<six::PolarizationType, six::PolarizationType> pols;
    pols.first = _convertDualPolarization(pol, true);
    pols.second = _convertDualPolarization(pol, false);
    return pols;
}

inline
double square(double val)
{
    return (val * val);
}

void assign(math::linear::MatrixMxN<7, 7>& sensorCovar,
            size_t row,
            size_t col,
            double val)
{
    sensorCovar(row, col) = sensorCovar(col, row) = val;
}

void getSensorCovariance(const six::PosVelError& error,
                         double rangeBias,
                         math::linear::MatrixMxN<7, 7>& sensorCovar)
{
    sensorCovar(0, 0) = square(error.p1);
    sensorCovar(1, 1) = square(error.p2);
    sensorCovar(2, 2) = square(error.p3);
    sensorCovar(3, 3) = square(error.v1);
    sensorCovar(4, 4) = square(error.v2);
    sensorCovar(5, 5) = square(error.v3);
    sensorCovar(6, 6) = square(rangeBias);

    if (error.corrCoefs.get())
    {
        const six::CorrCoefs& corrCoefs(*error.corrCoefs);

        // Position Error
        assign(sensorCovar, 0, 1, error.p1 * error.p2 * corrCoefs.p1p2);
        assign(sensorCovar, 0, 2, error.p1 * error.p3 * corrCoefs.p1p3);
        assign(sensorCovar, 1, 2, error.p2 * error.p3 * corrCoefs.p2p3);

        // Velocity Error
        assign(sensorCovar, 3, 4, error.v1 * error.v2 * corrCoefs.v1v2);
        assign(sensorCovar, 3, 5, error.v1 * error.v3 * corrCoefs.v1v3);
        assign(sensorCovar, 4, 5, error.v2 * error.v3 * corrCoefs.v2v3);

        // Position-Velocity Covariance
        assign(sensorCovar, 0, 3, error.p1 * error.v1 * corrCoefs.p1v1);
        assign(sensorCovar, 0, 4, error.p1 * error.v2 * corrCoefs.p1v2);
        assign(sensorCovar, 0, 5, error.p1 * error.v3 * corrCoefs.p1v3);
        assign(sensorCovar, 1, 3, error.p2 * error.v1 * corrCoefs.p2v1);
        assign(sensorCovar, 1, 4, error.p2 * error.v2 *corrCoefs.p2v2);
        assign(sensorCovar, 1, 5, error.p2 * error.v3 * corrCoefs.p2v3);
        assign(sensorCovar, 2, 3, error.p3 * error.v1 * corrCoefs.p3v1);
        assign(sensorCovar, 2, 4, error.p3 * error.v2 * corrCoefs.p3v2);
        assign(sensorCovar, 2, 5, error.p3 * error.v3 * corrCoefs.p3v3);
    }
}

void getErrors(const DerivedData& data,
               scene::Errors& errors)
{
    errors.clear();

    const six::ErrorStatistics* const errorStats(data.errorStatistics.get());
    if (errorStats)
    {
        const six::Components* const components(errorStats->components.get());

        if (components && components->posVelError.get())
        {
            errors.mFrameType = components->posVelError->frame;

            const double rangeBias = (components->radarSensor.get()) ?
                    components->radarSensor->rangeBias : 0.0;

            getSensorCovariance(*components->posVelError,
                                rangeBias,
                                errors.mSensorErrorCovar);
        }

        if (errorStats->compositeSCP.get() &&
            errorStats->compositeSCP->scpType == CompositeSCP::RG_AZ)
        {
            const types::RgAz<double> composite(
                    errorStats->compositeSCP->xErr,
                    errorStats->compositeSCP->yErr);
            const double corr = errorStats->compositeSCP->xyErr;
//            const types::RgAz<double> sampleSpacing(
//                    data.grid->row->sampleSpacing,
//                    data.grid->col->sampleSpacing);
            const six::sidd::MeasurableProjection* p =
                        reinterpret_cast<const six::sidd::MeasurableProjection*>(
                                data.measurement->projection.get());
            const types::RowCol<double> sampleSpacing = p->sampleSpacing;

            errors.mUnmodeledErrorCovar(0, 0) =
                    square(composite.rg) / square(sampleSpacing.row);
            errors.mUnmodeledErrorCovar(1, 1) =
                    square(composite.az) / square(sampleSpacing.col);
            errors.mUnmodeledErrorCovar(0, 1) =
                    errors.mUnmodeledErrorCovar(1, 0) =
                            corr * (composite.rg * composite.az) /
                            (sampleSpacing.row * sampleSpacing.col);
        }

        if (components && components->ionoError.get())
        {
            const six::IonoError& ionoError(*components->ionoError);
            errors.mIonoErrorCovar(0, 0) =
                    square(ionoError.ionoRangeVertical);
            errors.mIonoErrorCovar(1, 1) =
                    square(ionoError.ionoRangeRateVertical);
            errors.mIonoErrorCovar(0, 1) =
                    errors.mIonoErrorCovar(1, 0) =
                            ionoError.ionoRangeVertical *
                            ionoError.ionoRangeRateVertical *
                            ionoError.ionoRgRgRateCC;
        }

        if (components && components->tropoError.get())
        {
            errors.mTropoErrorCovar(0, 0) =
                    square(components->tropoError->tropoRangeVertical);
        }
    }
}

scene::ProjectionModel* Utilities::getProjectionModel(const DerivedData* data)
{
	const six::ProjectionType gridType = data->measurement->projection->projectionType;

	scene::SideOfTrack lookDir = getSideOfTrack(data);
	scene::Errors errors;
	getErrors(*data, errors);
	std::auto_ptr<scene::SceneGeometry> geom;

	switch (gridType)
	{
	case six::ProjectionType::PLANE:
		{
			six::sidd::PlaneProjection* plane =
				reinterpret_cast<six::sidd::PlaneProjection*>(
						data->measurement->projection.get());
		geom = getSceneGeometry(data);
		return new scene::PlaneProjectionModel(	geom->getSlantPlaneZ(),
												plane->productPlane.rowUnitVector,
												plane->productPlane.colUnitVector,
												plane->referencePoint.ecef,
												(math::poly::OneD<Vector3>&) data->measurement->arpPoly,
												(math::poly::TwoD<double>&) plane->timeCOAPoly,
												(int) lookDir,
												errors);
		}
	case six::ProjectionType::GEOGRAPHIC:
	{
		six::sidd::MeasurableProjection* geo =
						reinterpret_cast<six::sidd::MeasurableProjection*>(
								data->measurement->projection.get());
		geom = getSceneGeometry(data);
		return new scene::GeodeticProjectionModel(	geom->getSlantPlaneZ(),
													geo->referencePoint.ecef,
													(math::poly::OneD<Vector3>&) data->measurement->arpPoly,
													(math::poly::TwoD<double>&) geo->timeCOAPoly,
													(int) lookDir,
													errors);
	}
	case six::ProjectionType::POLYNOMIAL:
		throw except::Exception(Ctxt("Grid type not supported: " +
			                gridType.toString()));
	case six::ProjectionType::CYLINDRICAL:
		throw except::Exception(Ctxt("Grid type not supported: " +
			                gridType.toString()));
	case six::ProjectionType::NOT_SET:
		throw except::Exception(Ctxt("Grid type not supported: " +
		                gridType.toString()));
	default:
		throw except::Exception(Ctxt("Invalid grid type: " +
		                gridType.toString()));
	}

}

}
}


