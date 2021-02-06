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

#include <cphd/Enums.h>
#include <cphd/Types.h>
#include <mem/ScopedCopyablePtr.h>

namespace cphd
{

/*!
 *  \struct SRP
 *
 *  \brief The SRP position for the reference vector of the
 *  reference channel
 */
struct SRP
{
    //! Constructor
    SRP();

    //! Equality operator
    bool operator==(const SRP& other) const
    {
        return ecf == other.ecf && iac == other.iac;
    }
    bool operator!=(const SRP& other) const
    {
        return !((*this) == other);
    }

    //! SRP position in ECF coordinates. Value
    //! included in the SRPPos PVP
    Vector3 ecf;

    //! SRP position in Image Area Coordinates (IAX,
    //! IAY, IAZ).
    Vector3 iac;
};

/*!
 *  \struct ImagingType
 *
 *  \brief Base class for both Monostatic and
 *   Bistatic imaging types
 */
struct ImagingType
{
    //! Constructor
    ImagingType();

    //! Equality operators
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

    //! Destructor
    virtual ~ImagingType() {}

    //! Angle from north to the line from the SRP to the
    //! ARP ETP Nadir (i.e. North to +GPX). Measured
    //! clockwise from +North toward +East.
    double azimuthAngle;

    //! Grazing angle for the ARP to SRP LOS and the
    //! Earth Tangent Plane (ETP) at the SRP.
    double grazeAngle;

    //! Twist angle between cross range in the ETP and
    //! cross range in the slant plane at the SRP
    double twistAngle;

    //! Angle between the ETP normal (uUP) and the
    //! slant plane normal (uSPN) at the SRP
    double slopeAngle;

    //! Angle from north to the layover direction in the
    //! ETP. Measured clockwise from +North toward
    //! +East
    double layoverAngle;
};

/*!
 *  \struct Monostatic
 *
 *  \brief (Conditional) Collection Type Monostatic metadata.
 *
 *   Single radar platform that is both the transmitter
 *   and the receiver
 */
struct Monostatic : public ImagingType
{
    //! Constructor
    Monostatic();

    //! Equality operators
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

    //! Side of Track parameter for the collection.
    //! L Left-looking or R Right-looking
    six::SideOfTrackType sideOfTrack;

    //! Slant range from the ARP to the SRP
    double slantRange;

    //! Ground range from the ARP nadir to the SRP.
    //! Distance measured along spherical earth model
    //! passing through the SRP.
    double groundRange;

    //! Doppler Cone Angle between ARP velocity and
    //! SRP Line of Sight (LOS).
    double dopplerConeAngle;

    //! Incidence angle for the ARP to SRP LOS and the
    //! Earth Tangent Plane (ETP) at the SRP
    double incidenceAngle;

    //! ARP position in ECF coordinates (ARP)
    Vector3 arpPos;

    //! ARP velocity in ECF coordinates (VARP).
    Vector3 arpVel;
};

/*!
 *  \struct Bistatic
 *
 *  \brief (Conditional) Collection Type Bistatic metadata.
 *
 *  Pair of radar platforms, with a seperate transmitter
 *  and a passive receiver.
 */
struct Bistatic : public ImagingType
{
    /*!
     *  \struct PlatformParams
     *
     *  \brief Describe transmit and recieve platform parameters
     */
    struct PlatformParams
    {
        //! Constructor
        PlatformParams();

        //! Equality operator
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

        //! Side of Track parameter for the collection.
        //! L Left-looking or R Right-looking
        six::SideOfTrackType sideOfTrack;

        //! The transmit time for the vector (txc = TxTime).
        double time;

        //! Angle from north to the line from the SCP to the
        //! Pos Nadir in the ETP
        double azimuthAngle;

        //! Grazing angle between the RefPt LOS and Earth
        //! Tangent Plane (ETP
        double grazeAngle;

        //! Incidence angle between the RefPt LOS and ETP
        //! normal
        double incidenceAngle;

        //! Doppler Cone Angle between VXmt and line of
        //! sight to the ARP
        double dopplerConeAngle;

        //! Ground range from the transmit APC to the SRP
        //! (measured on the spherical surface containing the
        //! SRP).
        double groundRange;

        //! Slant range from the transmit APC to the SRP.
        double slantRange;

        //! Transmit APC position (Xmt) in ECF
        //! coordinates at txc
        Vector3 pos;

        //! Transmit APC velocity (VXmt) in ECF
        //! coordinates at txc.
        Vector3 vel;
    };

    //! Constructor
    Bistatic();

    //! Equality operator
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

    //! Angle from north to the line from the SCP to the
    //! Pos Nadir in the ETP
    double azimuthAngleRate;

    //! Bistatic angle (Beta) between unit vector from
    //! SRP to transmit APC (uXmt) and the unit vector
    //! from the SRP to the receive APC (uRcv).
    double bistaticAngle;

    //! Instantaneous rate of change of the bistatic angle
    //! (d(Beta)/dt).
    double bistaticAngleRate;

    //! Parameters that describe the Transmit platform.
    PlatformParams txPlatform;

    //! Parameters that describe the Receive platform
    PlatformParams rcvPlatform;

};

/*!
 *  \struct ReferenceGeometry
 *
 *  \brief Parameters that describe the collection geometry
 */
struct ReferenceGeometry
{
    //! Constructor
    ReferenceGeometry();

    //! Equality operator
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

    //! Reference time for the selected reference vector
    double referenceTime;

    //! The COD Time for point on the reference surface
    //! at (SRP_IAX, SRP_IAY).
    double srpCODTime;

    //! The Dwell Time for point on the reference
    //! surface at (SRP_IAX, SRP_IAY)
    double srpDwellTime;

    //! The SRP position for the reference vector of the
    //! reference channel.
    SRP srp;

    //! Parameters for CollectType = "MONOSTATIC"
    mem::ScopedCopyablePtr<Monostatic> monostatic;

    //! Parameters for CollectType = "BISTATIC"
    mem::ScopedCopyablePtr<Bistatic> bistatic;
};

//! Ostream operators
std::ostream& operator<< (std::ostream& os, const SRP& s);
std::ostream& operator<< (std::ostream& os, const ImagingType& i);
std::ostream& operator<< (std::ostream& os, const Monostatic& m);
std::ostream& operator<< (std::ostream& os, const Bistatic::PlatformParams& p);
std::ostream& operator<< (std::ostream& os, const Bistatic& b);
std::ostream& operator<< (std::ostream& os, const ReferenceGeometry& r);
}

#endif
