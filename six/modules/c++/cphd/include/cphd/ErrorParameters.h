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

#pragma once
#ifndef SIX_cphd_ErrorParameters_h_INCLUDED_
#define SIX_cphd_ErrorParameters_h_INCLUDED_

#include <stddef.h>

#include <ostream>
#include <vector>
#include <std/optional>

#include <scene/FrameType.h>
#include <six/ErrorStatistics.h>
#include <six/XsElement.h>

#include <cphd/Enums.h>
#include <cphd/Types.h>

namespace cphd
{

/*
 *  \struct ErrorParameters
 *
 *  \brief Describes error parameters
 *
 *  (Optional) Parameters that describe the statistics of errors
 *  in measured or estimated parameters that describe
 *  the collection
 */
struct ErrorParameters
{
    /*
     *  \struct Monostatic
     *
     *  \brief (Conditional) Parameters for CollectType = "MONOSTATIC"
     */
    struct Monostatic
    {
         /*
         *  \struct RadarSensor
         *
         *  \brief Position and velocity error statistics for the
         *   sensor platform
         */
        struct RadarSensor
        {
            //! Constructor
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
            mem::ScopedCopyablePtr<six::DecorrType> rangeBiasDecorr;
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
        six::PosVelError posVelErr;

        //! Radar sensor error statistics
        RadarSensor radarSensor;

        //! (Optional) Error parameters related to the Troposphere
        mem::ScopedCopyablePtr<six::TropoError> tropoError;

        //! (Optional) Error parameters related to the Ionosphere
        mem::ScopedCopyablePtr<six::IonoError> ionoError;

        //! (Optional) Additional error parameters to be added for
        //! Monostatic collections
        six::ParameterCollection parameter;
    };

    /*
     *  \struct Bistatic
     *
     *  \brief (Conditional) Parameters for CollectType = "BISTATIC"
     */
    struct Bistatic final
    {
        /*
         *  \struct RadarSensor
         *
         *  \brief Error statistics for the Transmit platform
         */
        struct RadarSensor final
        {
            //! Constructor
            RadarSensor();

            //! Equality operators
            bool operator==(const RadarSensor& other) const
            {
                return (delayBias == other.delayBias)
                    && (clockFreqSF == other.clockFreqSF)
                    && (collectionStartTime == other.collectionStartTime);
            }
            bool operator!=(const RadarSensor& other) const
            {
                return !((*this) == other);
            }

             //! Transmit time delay bias error standard deviation
            six::XsElement_minOccurs0<double> delayBias{ "DelayBias" }; // new in CPHD 1.1.0

            //! (Optional) Payload clock frequency scale factor standard deviation
            double clockFreqSF;

            //! Collection Start time error standard deviation.
            double collectionStartTime;
        };

        /*
         *  \struct Platform
         *
         *  \brief Error statisitcs of platforms
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
            six::PosVelError posVelErr;

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
std::ostream& operator<< (std::ostream& os, const six::PosVelError& p);
std::ostream& operator<< (std::ostream& os, const ErrorParameters& e);
}

#endif // SIX_cphd_ErrorParameters_h_INCLUDED_
