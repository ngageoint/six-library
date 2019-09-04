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


#ifndef __CPHD_ERROR_PARAMETERS_H__
#define __CPHD_ERROR_PARAMETERS_H__

#include <ostream>
#include <vector>
#include <stddef.h>

#include <cphd/Enums.h>
#include <cphd/Types.h>

#include <scene/FrameType.h>


namespace cphd
{

/*
 * Error Decorrelation function
 */
struct Decorr
{
    Decorr();

    bool operator==(const Decorr& other) const
    {
        return corrCoefZero == other.corrCoefZero &&
                decorrRate == other.decorrRate;
    }

    bool operator!=(const Decorr& other) const
    {
        return !((*this) == other);
    }

    double corrCoefZero;
    double decorrRate;
};

/*
 * Position and velocity error statistics for the sensory platform
 */
struct PosVelErr
{
    /*
     * Correlation coefficient parameters
     */
    struct CorrCoefs
    {
        CorrCoefs();

        bool operator==(const CorrCoefs& other) const
        {
            return p1p2 == other.p1p2 && p1p3 == other.p1p3 &&
                    p1v1 == other.p1v1 && p1v2 == other.p1v2 &&
                    p1v3 == other.p1v3 && p2p3 == other.p2p3 &&
                    p2v1 == other.p2v1 && p2v2 == other.p2v2 &&
                    p2v3 == other.p2v3 && p3v1 == other.p3v1 &&
                    p3v2 == other.p3v2 && p3v3 == other.p3v3 &&
                    v1v2 == other.v1v2 && v1v3 == other.v1v3 &&
                    v2v3 == other.v2v3;
        }

        bool operator!=(const CorrCoefs& other) const
        {
            return !((*this) == other);
        }

        double p1p2;
        double p1p3;
        double p1v1;
        double p1v2;
        double p1v3;
        double p2p3;
        double p2v1;
        double p2v2;
        double p2v3;
        double p3v1;
        double p3v2;
        double p3v3;
        double v1v2;
        double v1v3;
        double v2v3;
    };

    PosVelErr();

    bool operator==(const PosVelErr& other) const
    {
        return frame == other.frame && p1 == other.p1 && p2 == other.p2 &&
                p3 == other.p3 && v1 == other.v1 &&
                v2 == other.v2 && v3 == other.v3 &&
                corrCoefs == other.corrCoefs &&
                positionDecorr == other.positionDecorr;
    }

    bool operator!=(const PosVelErr& other) const
    {
        return !((*this) == other);
    }

    scene::FrameType frame;
    double p1;
    double p2;
    double p3;
    double v1;
    double v2;
    double v3;
    mem::ScopedCopyablePtr<CorrCoefs> corrCoefs;
    mem::ScopedCopyablePtr<Decorr> positionDecorr;
};

/*
 * Parameters that describe the statistics of errors
 * in measured or estimated parameters that describe
 * the collection
 * See section 9
 */
struct ErrorParameters
{
    struct Monostatic
    {
        struct RadarSensor
        {
            RadarSensor();

            bool operator==(const RadarSensor& other) const
            {
                return rangeBias == other.rangeBias &&
                        clockFreqSF == other.clockFreqSF &&
                        collectionStartTime == other.collectionStartTime &&
                        rangeBiasDecorr == other.rangeBiasDecorr;
            }

            bool operator!=(const RadarSensor& other) const
            {
                return !((*this) == other);
            }

            double rangeBias;
            double clockFreqSF;
            double collectionStartTime;
            mem::ScopedCopyablePtr<Decorr> rangeBiasDecorr;
        };

        struct TropoError
        {
            TropoError();

            bool operator==(const TropoError& other) const
            {
                return tropoRangeVertical == other.tropoRangeVertical &&
                        tropoRangeSlant == other.tropoRangeSlant &&
                        tropoRangeDecorr == other.tropoRangeDecorr;
            }

            bool operator!=(const TropoError& other) const
            {
                return !((*this) == other);
            }

            double tropoRangeVertical;
            double tropoRangeSlant;
            mem::ScopedCopyablePtr<Decorr> tropoRangeDecorr;
        };

        struct IonoError
        {
            IonoError();

            bool operator==(const IonoError& other) const
            {
                return ionoRangeVertical == other.ionoRangeVertical &&
                        ionoRangeRateVertical == other.ionoRangeRateVertical &&
                        ionoRgRgRateCC == other.ionoRgRgRateCC &&
                        ionoRangeVertDecorr == other.ionoRangeVertDecorr;
            }

            bool operator!=(const IonoError& other) const
            {
                return !((*this) == other);
            }

            double ionoRangeVertical;
            double ionoRangeRateVertical;
            double ionoRgRgRateCC;
            mem::ScopedCopyablePtr<Decorr> ionoRangeVertDecorr;
        };

        bool operator==(const Monostatic& other) const
        {
            return posVelErr == other.posVelErr &&
                    radarSensor == other.radarSensor &&
                    tropoError == other.tropoError &&
                    ionoError == other.ionoError &&
                    parameter == other.parameter;
        }

        bool operator!=(const Monostatic& other) const
        {
            return !((*this) == other);
        }

        PosVelErr posVelErr;
        RadarSensor radarSensor;
        mem::ScopedCopyablePtr<TropoError> tropoError;
        mem::ScopedCopyablePtr<IonoError> ionoError;
        six::ParameterCollection parameter;
    };


    struct Bistatic
    {
        struct RadarSensor
        {
            RadarSensor();

            bool operator==(const RadarSensor& other) const
            {
                return clockFreqSF == other.clockFreqSF &&
                        collectionStartTime == other.collectionStartTime;
            }

            bool operator!=(const RadarSensor& other) const
            {
                return !((*this) == other);
            }

            double clockFreqSF;
            double collectionStartTime;
        };

        struct Platform
        {
            bool operator==(const Platform& other) const
            {
                return posVelErr == other.posVelErr &&
                        radarSensor == other.radarSensor;
            }

            bool operator!=(const Platform& other) const
            {
                return !((*this) == other);
            }

            PosVelErr posVelErr;
            RadarSensor radarSensor;
        };

        bool operator==(const Bistatic& other) const
        {
            return txPlatform == other.txPlatform &&
                    rcvPlatform == other.rcvPlatform &&
                    parameter == other.parameter;
        }

        bool operator!=(const Bistatic& other) const
        {
            return !((*this) == other);
        }

        Platform txPlatform;
        Platform rcvPlatform;
        six::ParameterCollection parameter;
    };

    bool operator==(const ErrorParameters& other) const
    {
        return monostatic == other.monostatic &&
                bistatic == other.bistatic;
    }

    bool operator!=(const ErrorParameters& other) const
    {
        return !((*this) == other);
    }

    mem::ScopedCopyablePtr<Monostatic> monostatic;
    mem::ScopedCopyablePtr<Bistatic> bistatic;
};

std::ostream& operator<< (std::ostream& os, const Decorr& d);
std::ostream& operator<< (std::ostream& os, const PosVelErr::CorrCoefs& c);
std::ostream& operator<< (std::ostream& os, const PosVelErr& p);
std::ostream& operator<< (std::ostream& os, const ErrorParameters& e);
}

#endif
