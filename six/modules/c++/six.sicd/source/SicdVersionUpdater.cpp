#include <cassert>
#include <six/sicd/Utilities.h>
#include <logging/Logger.h>
#include <six/VersionUpdater.h>
#include <six/sicd/SicdVersionUpdater.h>

namespace
{
static const std::vector<std::string> SICD_VERSIONS =
{
    "0.4.0", "0.4.1", "0.5.0", "1.0.0", "1.0.1", "1.1.0", "1.2.0"
};
}

namespace six
{
namespace sicd
{
SicdVersionUpdater::SicdVersionUpdater(ComplexData& complexData,
                                       const std::string& targetVersion,
                                       logging::Logger& log) :
    VersionUpdater(complexData, targetVersion, SICD_VERSIONS, log),
    mData(complexData)
{
}

void SicdVersionUpdater::updateSingleIncrement()
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
                emitWarning("ComplexData.RMA.RMAT.DistRLPoly");
            }
        }
        return;
    }

    if (thisVersion == "0.5.0")
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
                matchType->typeID = "NOT SET";
                matchType->collectorName = "";
                matchType->illuminatorName = "";
                matchType->matchCollects.clear();
                emitWarning("ComplexData.MatchInformation.MatchType.TypeID");
            }
        }

        if (mData.scpcoa)
        {
            mData.scpcoa->azimAngle = 0;
            mData.scpcoa->layoverAngle = 0;
            emitWarning("ComplexData.SCPCOA.AzimAngle");
            emitWarning("ComplexData.SCPCOA.LayoverAngle");
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
}
}
}