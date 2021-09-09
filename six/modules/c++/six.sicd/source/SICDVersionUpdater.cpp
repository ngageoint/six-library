/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
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
#include <cassert>
#include <six/sicd/Utilities.h>
#include <logging/Logger.h>
#include <scene/SceneGeometry.h>
#include <six/VersionUpdater.h>
#include <six/sicd/SICDVersionUpdater.h>

namespace six
{
namespace sicd
{
SICDVersionUpdater::SICDVersionUpdater(ComplexData& complexData,
                                       const std::string& targetVersion,
                                       logging::Logger& log) :
    VersionUpdater(complexData,
                   targetVersion,
                   getValidVersions(),
                   log),
    mData(complexData)
{
}

const std::vector<std::string>&
SICDVersionUpdater::getValidVersions()
{
    static const std::vector<std::string> sicdVersions =
    {
        "0.4.0", "0.4.1", "0.5.0", "1.0.0", "1.0.1", "1.1.0", "1.2.0"
    };
    return sicdVersions;
}

void SICDVersionUpdater::recordProcessingStep()
{
    if (!mData.imageFormation)
    {
        throw except::Exception(Ctxt(
                "ComplexData.ImageFormation is required in all SICD versions."));
    }

    // Add a new processing block to tell consumers about
    // the automated version update.
    Processing versionProcessing;
    versionProcessing.applied = six::BooleanType::IS_TRUE;
    versionProcessing.type = "Automated version update";
    mData.imageFormation->processing.push_back(versionProcessing);
}

void SICDVersionUpdater::addProcessingParameter(const std::string& fieldName)
{
    Parameter parameter(fieldName);
    parameter.setName("Guessed Field");
    const size_t index = mData.imageFormation->processing.size() - 1;
    mData.imageFormation->processing[index].parameters.push_back(parameter);
}

void SICDVersionUpdater::updateSingleIncrement()
{
    const std::string thisVersion = mData.getVersion();
    if (thisVersion == "0.4.1" ||
        thisVersion == "1.0.0" ||
        thisVersion == "1.0.1" ||
        thisVersion == "1.1.0")
    {
        // Nothing to do
        return;
    }

    if (mData.getVersion() == "0.4.0")
    {
        if (mData.rma && mData.rma->rmat)
        {
            if (mTarget == "0.4.1" || mTarget == "0.5.0")
            {
                // This polynomial was added in 0.4.1, but deprecated
                // again in 1.0.0. So if we're going to end up there anyway,
                // no need to do anything.
                mData.rma->rmat->distRefLinePoly = six::Poly1D(1);
                emitWarning("ComplexData.RMA.RMAT.DistRefLinePoly");
            }
        }
    }
    else if (thisVersion == "0.5.0")
    {
        if (mData.radarCollection.get())
        {
            auto& radarCollection = *mData.radarCollection;
            for (auto& rcvChannel : radarCollection.rcvChannels)
            {
                rcvChannel->txRcvPolarization = six::DualPolarizationType::OTHER;
                emitWarning("ComplexData.RadarCollection.RcvChannel.TxRcvPolarization");
            }

            if (radarCollection.txPolarization == six::PolarizationSequenceType::NOT_SET)
            {
                radarCollection.txPolarization = six::PolarizationSequenceType::OTHER;
                emitWarning("ComplexData.RadarCollection.TxPolarization");
            }

            radarCollection.polarizationHVAnglePoly = six::Init::undefined<six::Poly1D>();
        }

        if (mData.radiometric)
        {
            mData.radiometric->gammaZeroSFIncidenceMap = six::AppliedType::NOT_SET;
            mData.radiometric->sigmaZeroSFIncidenceMap = six::AppliedType::NOT_SET;
        }

        if (mData.antenna)
        {
            if (mData.antenna->tx)
            {
                mData.antenna->tx->halfPowerBeamwidths.reset();
                if (!mData.antenna->tx->array)
                {
                    mData.antenna->tx->array.reset(new six::sicd::GainAndPhasePolys());
                    mData.antenna->tx->array->gainPoly = six::Poly2D(1, 1);
                    mData.antenna->tx->array->phasePoly = six::Poly2D(1, 1);
                    emitWarning("ComplexData.Antenna.Tx.Array.GainPoly");
                    emitWarning("ComplexData.Antenna.Tx.Array.PhasePoly");
                }
                if (!mData.antenna->rcv->array)
                {
                    mData.antenna->rcv->array.reset(new six::sicd::GainAndPhasePolys());
                    mData.antenna->rcv->array->gainPoly = six::Poly2D(1, 1);
                    mData.antenna->rcv->array->phasePoly = six::Poly2D(1, 1);
                    emitWarning("ComplexData.Antenna.Rcv.Array.GainPoly");
                    emitWarning("ComplexData.Antenna.Rcv.Array.PhasePoly");
                }
                if (!mData.antenna->twoWay->array)
                {
                    mData.antenna->twoWay->array.reset(new six::sicd::GainAndPhasePolys());
                    mData.antenna->twoWay->array->gainPoly = six::Poly2D(1, 1);
                    mData.antenna->twoWay->array->phasePoly = six::Poly2D(1, 1);
                    emitWarning("ComplexData.Antenna.TwoWay.Array.GainPoly");
                    emitWarning("ComplexData.Antenna.TwoWay.Array.PhasePoly");
                }
            }
        }

        if (mData.matchInformation)
        {
            for (auto& matchType : mData.matchInformation->types)
            {
                matchType.typeID = "NOT SET";
                matchType.collectorName = "";
                matchType.illuminatorName = "";
                matchType.matchCollects.clear();
                emitWarning("ComplexData.MatchInformation.MatchType.TypeID");
            }
        }

        if (mData.scpcoa)
        {
            const auto *sceneGeometry = Utilities::getSceneGeometry(&mData);
            mData.scpcoa->azimAngle = sceneGeometry->getAzimuthAngle();
            mData.scpcoa->layoverAngle = sceneGeometry->getETPLayoverAngle();
        }

        if (mData.rma && mData.rma->rmat)
        {
            mData.rma->rmat->refTime = six::Init::undefined<double>();
            mData.rma->rmat->distRefLinePoly = six::Init::undefined<six::Poly1D>();
            mData.rma->rmat->cosDCACOAPoly = six::Init::undefined<six::Poly2D>();
            mData.rma->rmat->kx1 = six::Init::undefined<double>();
            mData.rma->rmat->kx2 = six::Init::undefined<double>();
            mData.rma->rmat->ky1 = six::Init::undefined<double>();
            mData.rma->rmat->ky2 = six::Init::undefined<double>();
            mData.rma->rmat->dopConeAngleRef = 0;
            emitWarning("ComplexData.RMA.RMAT.DopConeAngleRef");
        }
    }
    else
    {
        throw except::Exception(Ctxt("Unhandled version: " + thisVersion));
    }
}
}
}
