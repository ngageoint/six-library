/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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


#ifndef __CPHD_REFERENCE_GEOMETRY_H__
#define __CPHD_REFERENCE_GEOMETRY_H__

#include <ostream>

#include <mem/ScopedCopyablePtr.h>
#include <cphd/Enums.h>
#include <cphd/Types.h>

namespace cphd
{
struct SRP
{
    SRP();

    bool operator==(const SRP& other) const
    {
        return ecf == other.ecf && iac == other.iac;
    }

    bool operator!=(const SRP& other) const
    {
        return !((*this) == other);
    }

    Vector3 ecf;
    Vector3 iac;
};


struct ImagingType
{
    ImagingType();

    bool operator==(const ImagingType& other) const
    {
        return azimuthAngle == other.azimuthAngle &&
                grazeAngle == other.grazeAngle &&
                twistAngle == other.twistAngle && 
                slopeAngle == other.slopeAngle &&
                layoverAngle == other.layoverAngle;
    }
    bool operator!=(const ImagingType& other) const
    {
        return !((*this) == other);
    }

    virtual ~ImagingType() {}

    double azimuthAngle;
    double grazeAngle;
    double twistAngle;
    double slopeAngle;
    double layoverAngle;
};

struct Monostatic : virtual public ImagingType
{
    Monostatic();

    bool operator==(const Monostatic& other) const
    {
        if((ImagingType)(*this) != (ImagingType)other) {
            return false;
        }

        return sideOfTrack == other.sideOfTrack &&
                slantRange == other.slantRange &&
                groundRange == other.groundRange &&
                dopplerConeAngle == other.dopplerConeAngle &&
                incidenceAngle == other.incidenceAngle &&
                arpPos == other.arpPos && arpVel == other.arpVel;
    }
    bool operator!=(const Monostatic& other) const
    {
        return !((*this) == other);
    }

    six::SideOfTrackType sideOfTrack;
    double slantRange;
    double groundRange;
    double dopplerConeAngle;
    double incidenceAngle;
    Vector3 arpPos;
    Vector3 arpVel;
};

struct Bistatic : public ImagingType
{
    struct PlatformParams
    {
        PlatformParams();

        bool operator==(const PlatformParams& other) const
        {
            return sideOfTrack == other.sideOfTrack &&
                    azimuthAngle == other.azimuthAngle &&
                    grazeAngle == other.grazeAngle &&
                    dopplerConeAngle == other.dopplerConeAngle &&
                    groundRange == other.groundRange &&
                    slantRange == other.slantRange &&
                    time == other.time &&
                    pos == other.pos && vel == other.vel;
        }

        bool operator!=(const PlatformParams& other) const
        {
            return !((*this) == other);
        }

        six::SideOfTrackType sideOfTrack;
        double time;
        double azimuthAngle;
        double grazeAngle;
        double incidenceAngle;
        double dopplerConeAngle;
        double groundRange;
        double slantRange;
        Vector3 pos;
        Vector3 vel;
    };

    Bistatic();

    bool operator==(const Bistatic& other) const
    {
        if((ImagingType)(*this) != (ImagingType)other) {
            return false;
        }

        return azimuthAngleRate == other.azimuthAngleRate &&
                bistaticAngle == other.bistaticAngle &&
                bistaticAngleRate == other.bistaticAngleRate &&
                txPlatform == other.txPlatform &&
                rcvPlatform == other.rcvPlatform;
    }

    bool operator!=(const Bistatic& other) const
    {
        return !((*this) == other);
    }

    double azimuthAngleRate;
    double bistaticAngle;
    double bistaticAngleRate;
    PlatformParams txPlatform;  // Transition Platform
    PlatformParams rcvPlatform;  // Receive Platform

};

struct ReferenceGeometry
{
    ReferenceGeometry();

    bool operator==(const ReferenceGeometry& other) const
    {
        return referenceTime == other.referenceTime &&
                srpCODTime == other.srpCODTime &&
                srpDwellTime == other.srpDwellTime && 
                srp == other.srp &&
                monostatic == other.monostatic &&
                bistatic == other.bistatic;
    }

    bool operator!=(const ReferenceGeometry& other) const
    {
        return !((*this) == other);
    }

    double referenceTime;
    double srpCODTime;
    double srpDwellTime;
    SRP srp;
    mem::ScopedCopyablePtr<Monostatic> monostatic;
    mem::ScopedCopyablePtr<Bistatic> bistatic;
};

}

#endif
