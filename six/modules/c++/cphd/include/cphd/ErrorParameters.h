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

#include <scene/FrameType.h>

#include <cphd/Enums.h>
#include <cphd/Types.h>

namespace cphd
{

/*
 * Error Decorrelation function
 */
struct Decorr
{
    //! Default constructor
    Decorr();

    //! Equality operators
    bool operator==(const Decorr& other) const
    {
        return corrCoefZero == other.corrCoefZero &&
                decorrRate == other.decorrRate;
    }
    bool operator!=(const Decorr& other) const
    {
        return !((*this) == other);
    }

    //! Error correlation coefficient for zero time
    //! difference (CC0)
    double corrCoefZero;

    //! Error decorrelation rate.
    double decorrRate;
};

/*
 * Position and velocity error statistics for the sensory platform
 */
struct PosVelErr
{
    /*
     * (Optional) Correlation coefficient parameters
     */
    struct CorrCoefs
    {
        //! Default constructor
        CorrCoefs();

        //! Equality operators
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

        //! P1, P2 correlation coefficient.
        double p1p2;

        //! P1, P3 correlation coefficient.
        double p1p3;

        //! P1, V1 correlation coefficient.
        double p1v1;

        //! P1, V2 correlation coefficient.
        double p1v2;

        //! P1, V3 correlation coefficient.
        double p1v3;

        //! P2, P3 correlation coefficient.
        double p2p3;

        //! P2, V1 correlation coefficient.
        double p2v1;

        //! P2, V2 correlation coefficient.
        double p2v2;

        //! P2, V3 correlation coefficient.
        double p2v3;

        //! P3, V1 correlation coefficient.
        double p3v1;

        //! P3, V2 correlation coefficient.
        double p3v2;

        //! P3, V3 correlation coefficient.
        double p3v3;

        //! V1, V2 correlation coefficient.
        double v1v2;

        //! V1, V3 correlation coefficient.
        double v1v3;

        //! V2, V3 correlation coefficient.
        double v2v3;
    };

    //! Default constructor
    PosVelErr();

    //! Equality operator
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

    //! Coordinate frame used for specifying position
    //! and velocity error statistics.
    scene::FrameType frame;

    //! Position coordinate 1 standard deviation.
    double p1;

    //! Position coordinate 2 standard deviation.
    double p2;

    //! Position coordinate 3 standard deviation.
    double p3;

    //! Velocity coordinate 1 standard deviation.
    double v1;

    //! Velocity coordinate 2 standard deviation.
    double v2;

    //! Velocity coordinate 3 standard deviation.
    double v3;

    //! (Optional) Correlation Coefficient parameters
    mem::ScopedCopyablePtr<CorrCoefs> corrCoefs;

    //! (Optional) Platform position error decorrelation function.
    mem::ScopedCopyablePtr<Decorr> positionDecorr;
};

/*
 * (Optional) Parameters that describe the statistics of errors
 * in measured or estimated parameters that describe
 * the collection
 * See section 9
 */
struct ErrorParameters
{
    /*
     * (Conditional) Parameters for CollectType = "MONOSTATIC"
     */
    struct Monostatic
    {
        /*
         * Position and velocity error statistics for the
         * sensor platform
         */
        struct RadarSensor
        {
            //! Default constructor
            RadarSensor();

            //! Equality operators
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

            //! Range bias error standard deviation.
            double rangeBias;

            //! (Optional) Payload clock frequency scale factor standard deviation
            double clockFreqSF;

            //! (Optional) Collection Start time error standard deviation
            double collectionStartTime;

            //! (Optional) Range Bias error decorrelation function
            mem::ScopedCopyablePtr<Decorr> rangeBiasDecorr;
        };

        /*
         * (Optional) Error parameters relating to the Troposphere
         */
        struct TropoError
        {
            //! Default constructor
            TropoError();

            //! Equality operators
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

            //! (Optional) Troposphere two-way delay error for normal
            //! incidence standard deviation
            double tropoRangeVertical;

            //! (Optional) Troposphere two-way delay error for the SRP for
            //! the reference vector of the reference channel
            double tropoRangeSlant;

            //! (Optional) Range Bias error decorrelation function.
            mem::ScopedCopyablePtr<Decorr> tropoRangeDecorr;
        };

        /*
         * (Optional) Error parameters related to the Ionosphere
         */
        struct IonoError
        {
            //! Default constructor
            IonoError();

            //! Equality operators
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

            //! Ionosphere two-way delay error for normal
            //! incidence standard deviation
            double ionoRangeVertical;

            //! (Optional) Ionosphere two-way delay rate of change error
            //! for normal incidence standard deviation. 
            double ionoRangeRateVertical;

            //! (Optional) Ionosphere range error and range rate error
            //! correlation coefficient.
            double ionoRgRgRateCC;

            //! (Optional) Range Bias error decorrelation function
            mem::ScopedCopyablePtr<Decorr> ionoRangeVertDecorr;
        };

        //! Equality operators
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

        //! Position and velocity error statistics for the
        //! Transmit platform.
        PosVelErr posVelErr;

        //! Radar sensor error statistics
        RadarSensor radarSensor;

        //! (Optional) Error parameters related to the Troposphere
        mem::ScopedCopyablePtr<TropoError> tropoError;

        //! (Optional) Error parameters related to the Ionosphere
        mem::ScopedCopyablePtr<IonoError> ionoError;

        //! (Optional) Additional error parameters to be added for
        //! Monostatic collections
        six::ParameterCollection parameter;
    };

    /*
     * (Conditional) Parameters for CollectType = "BISTATIC"
     */
    struct Bistatic
    {
        /*
         * Error statistics for the Transmit platform
         */
        struct RadarSensor
        {
            //! Default constructor
            RadarSensor();

            //! Equality operators
            bool operator==(const RadarSensor& other) const
            {
                return clockFreqSF == other.clockFreqSF &&
                        collectionStartTime == other.collectionStartTime;
            }
            bool operator!=(const RadarSensor& other) const
            {
                return !((*this) == other);
            }

            //! (Optional) Payload clock frequency scale factor standard
            //! deviation
            double clockFreqSF;

            //! Collection Start time error standard deviation.
            double collectionStartTime;
        };

        /*
         * Error statisitcs of platforms
         */
        struct Platform
        {
            //! Equality operators
            bool operator==(const Platform& other) const
            {
                return posVelErr == other.posVelErr &&
                        radarSensor == other.radarSensor;
            }
            bool operator!=(const Platform& other) const
            {
                return !((*this) == other);
            }

            //! Position and velocity error statistics
            PosVelErr posVelErr;

            //! Transmit sensor error statistics
            RadarSensor radarSensor;
        };

        //! Equality operators
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

        //! Error statistics for the Transmit platform
        Platform txPlatform;

        //! Error statistics for the Receive Only platform.
        Platform rcvPlatform;

        //! Additional error parameters to be added for
        //! Bistatic collections
        six::ParameterCollection parameter;
    };

    //! Equality operators
    bool operator==(const ErrorParameters& other) const
    {
        return monostatic == other.monostatic &&
                bistatic == other.bistatic;
    }
    bool operator!=(const ErrorParameters& other) const
    {
        return !((*this) == other);
    }

    //! (Conditional) Parameters for CollectType = "MONOSTATIC"
    mem::ScopedCopyablePtr<Monostatic> monostatic;

    //! (Conditional) Parameters for CollectType = "BISTATIC"
    mem::ScopedCopyablePtr<Bistatic> bistatic;
};

//! Ostream operators
std::ostream& operator<< (std::ostream& os, const Decorr& d);
std::ostream& operator<< (std::ostream& os, const PosVelErr::CorrCoefs& c);
std::ostream& operator<< (std::ostream& os, const PosVelErr& p);
std::ostream& operator<< (std::ostream& os, const ErrorParameters& e);
}

#endif
