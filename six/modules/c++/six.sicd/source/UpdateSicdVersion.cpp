#include <cassert>
#include <six/sicd/Utilities.h>
#include <logging/Logger.h>

namespace
{
static const std::vector<std::string> VERSIONS =
{
    "0.3.1", "0.4.0", "0.4.1", "0.5.0", "1.0.0", "1.0.1", "1.1.0", "1.2.0"
};

class VersionUpdater
{
public:
    VersionUpdater(six::sicd::ComplexData& complexData,
                   const std::string& targetVersion) :
        mData(complexData),
        mTarget(targetVersion)
    {
    }

    bool finished() const
    {
        return mData.getVersion() == mTarget;
    }

    void bumpVersion()
    {
        if (finished())
        {
            throw except::Exception(Ctxt("Can't increment version any futher"));
        }

        auto it = std::find(VERSIONS.begin(), VERSIONS.end(), mData.getVersion());
        assert(it != VERSIONS.end());
        ++it;
        assert(it != VERSIONS.end());
        mData.setVersion(*it);
    }

private:
    six::sicd::ComplexData& mData;
    const std::string& mTarget;
};

void validateTargetVersion(six::sicd::ComplexData& complexData,
                           const std::string& version)
{
    const auto targetIt = std::find(VERSIONS.begin(), VERSIONS.end(), version);
    if (targetIt == VERSIONS.end())
    {
        std::ostringstream msg;
        msg << "Unrecognized SICD version: " << version;
        throw except::Exception(Ctxt(msg.str()));
    }

    const auto currentIt = std::find(VERSIONS.begin(), VERSIONS.end(),
                                     complexData.getVersion());
    if (std::distance(currentIt, targetIt) <= 0)
    {
        std::ostringstream msg;
        msg << "Target version <" << version << "> must be later than "
            << "current version <" << complexData.getVersion() << ">";
        throw except::Exception(Ctxt(msg.str()));
    }
}
}

namespace six
{
namespace sicd
{
void Utilities::updateVersion(
    six::sicd::ComplexData& complexData,
    const std::string& targetVersion,
    logging::Logger& log)
{
    validateTargetVersion(complexData, targetVersion);
    VersionUpdater updater(complexData, targetVersion);

    // TODO: Regenerate Python bindings before submitting PR
    // All the changes from 1.0.0 to 1.2.0 simply reflect
    // changes in the layout or constraints in the schema
    // All we need for valid metadata is to set the version.
    if (complexData.getVersion() == "0.4.0")
    {
        if (complexData.rma && complexData.rma->rmat)
        {
            if (targetVersion == "0.4.1" || targetVersion == "0.5.0")
            {
                // This polynomial was added in 0.4.1, but deprecated
                // again in 1.0.0. So if we're going to end up there anyway,
                // no need to do anything.
                complexData.rma->rmat->distRefLinePoly = six::Poly1D(1);
                log.warn("ComplexData.RMA.RMAT.DistRLPoly needs valid values");
            }
        }
        updater.bumpVersion();
        if (updater.finished())
        {
            return;
        }
    }

    if (complexData.getVersion() == "0.4.1")
    {
        updater.bumpVersion();
        if (updater.finished())
        {
            return;
        }
    }

    if (complexData.getVersion() == "0.5.0")
    {
        if (complexData.radarCollection.get())
        {
            auto& radarCollection = *complexData.radarCollection;
            for (auto& rcvChannel : radarCollection.rcvChannels)
            {
                rcvChannel->txRcvPolarization = six::DualPolarizationType::OTHER;
                log.warn("ComplexData.RadarCollection.RcvChannel.TxRcvPolarization "
                         "needs a value");
            }

            if (radarCollection.txPolarization == six::PolarizationSequenceType::NOT_SET)
            {
                radarCollection.txPolarization = six::PolarizationSequenceType::OTHER;
                log.warn("ComplexData.RadarCollection.TxPolarization "
                         "needs a value");
            }

            radarCollection.polarizationHVAnglePoly = Init::undefined<Poly1D>();
        }

        if (complexData.radiometric)
        {
            complexData.radiometric->gammaZeroSFIncidenceMap = six::AppliedType::NOT_SET;
            complexData.radiometric->sigmaZeroSFIncidenceMap = six::AppliedType::NOT_SET;
        }

        if (complexData.antenna)
        {
            if (complexData.antenna->tx)
            {
                complexData.antenna->tx->halfPowerBeamwidths.reset();
                if (!complexData.antenna->tx->array)
                {
                    complexData.antenna->tx->array.reset(new GainAndPhasePolys());
                    complexData.antenna->tx->array->gainPoly = six::Poly2D(1, 1);
                    complexData.antenna->tx->array->phasePoly = six::Poly2D(1, 1);
                    log.warn("ComplexData.Antenna.Tx.Array.GainPoly needs values");
                    log.warn("ComplexData.Antenna.Tx.Array.PhasePoly needs values");
                }
                if (!complexData.antenna->rcv->array)
                {
                    complexData.antenna->rcv->array.reset(new GainAndPhasePolys());
                    complexData.antenna->rcv->array->gainPoly = six::Poly2D(1, 1);
                    complexData.antenna->rcv->array->phasePoly = six::Poly2D(1, 1);
                    log.warn("ComplexData.Antenna.Rcv.Array.GainPoly needs values");
                    log.warn("ComplexData.Antenna.Rcv.Array.PhasePoly needs values");
                }
                if (!complexData.antenna->twoWay->array)
                {
                    complexData.antenna->twoWay->array.reset(new GainAndPhasePolys());
                    complexData.antenna->twoWay->array->gainPoly = six::Poly2D(1, 1);
                    complexData.antenna->twoWay->array->phasePoly = six::Poly2D(1, 1);
                    log.warn("ComplexData.Antenna.TwoWay.Array.GainPoly needs values");
                    log.warn("ComplexData.Antenna.TwoWay.Array.PhasePoly needs values");
                }
            }
        }

        if (complexData.matchInformation)
        {
            for (auto& matchType : complexData.matchInformation->types)
            {
                matchType->typeID = "NOT SET";
                matchType->collectorName = "";
                matchType->illuminatorName = "";
                matchType->matchCollects.clear();
                log.warn("ComplexData.MatchInformation.MatchType.TypeID "
                         "populated with placeholder string.");
            }
        }

        if (complexData.scpcoa)
        {
            complexData.scpcoa->azimAngle = 0;
            complexData.scpcoa->layoverAngle = 0;
            log.warn("ComplexData.SCPCOA.AzimAngle needs a valid value");
            log.warn("ComplexData.SCPCOA.LayoverAngle needs a valid value");
        }

        if (complexData.rma && complexData.rma->rmat)
        {
            complexData.rma->rmat->refTime = Init::undefined<double>();
            complexData.rma->rmat->distRefLinePoly = Init::undefined<Poly1D>();
            complexData.rma->rmat->cosDCACOAPoly = Init::undefined<Poly2D>();
            complexData.rma->rmat->kx1 = Init::undefined<double>();
            complexData.rma->rmat->kx2 = Init::undefined<double>();
            complexData.rma->rmat->ky1 = Init::undefined<double>();
            complexData.rma->rmat->ky2 = Init::undefined<double>();
            complexData.rma->rmat->dopConeAngleRef = 0;
            log.warn("ComplexData.RMA.RMAT.DopConeAngleRef needs a valid value");
        }

        updater.bumpVersion();
        if (updater.finished())
        {
            return;
        }
    }

    if (complexData.getVersion() == "1.0.0" ||
        complexData.getVersion() == "1.0.1" ||
        complexData.getVersion() == "1.1.0")
    {
        // No metadata differences, so just jump straight to the end
        complexData.setVersion(targetVersion);
    }

    // This should only be true for 0.3.1, for which there is
    // no documentation
    if (complexData.getVersion() != targetVersion)
    {
        std::ostringstream msg;
        msg << "Conversion from version " << complexData.getVersion()
            << " not yet supported";
        throw except::NotImplementedException(Ctxt(msg.str()));
    }

    return;
}
}
}