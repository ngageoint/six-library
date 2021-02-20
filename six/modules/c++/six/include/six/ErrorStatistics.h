/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_ERROR_STATISTICS_H__
#define __SIX_ERROR_STATISTICS_H__

#include <sys/Optional.h>

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"
#include "six/ParameterCollection.h"
#include <mem/ScopedCopyablePtr.h>

namespace six
{

/*!
 *  \struct CorrCoefs
 *  \brief CorrCoefs
 *
 *  Correlation coefficient parameters.  This object is shared
 *  between SICD and SIDD
 */
struct CorrCoefs
{
    CorrCoefs()
    {
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

    //! Equality operators
    bool operator==(const CorrCoefs& rhs) const;
    bool operator!=(const CorrCoefs& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct PosVelError
 *  \brief SICD/SIDD PosVelErr
 *
 *
 *  Position and velocity error statistics for the
 *  radar platform.  Name is altered to match API conventions
 */
struct PosVelError
{
    //!  CorrCoefs are nullptr, since optional
    PosVelError() :
        positionDecorr(Init::undefined<DecorrType>())
    {
    }

    //! Coordinate frame used for expressing P,V error statistics
    FrameType frame;

    double p1;
    double p2;
    double p3;
    double v1;
    double v2;
    double v3;

    //! Optional
    mem::ScopedCopyablePtr<CorrCoefs> corrCoefs;

    //! Can be none, make sure to set this undefined()
    DecorrType positionDecorr;
    const DecorrType& getPositionDecorr() const
    {
        return positionDecorr;
    }

    //! Equality operators
    bool operator==(const PosVelError& rhs) const;
    bool operator!=(const PosVelError& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct RadarSensor
 *  \brief (Optional) RadarSensor parameter
 *
 *
 *  Radar sensor error statistics.
 */
struct RadarSensor
{
    /*!
     *  Range bias error standard deviation. 
     *  Range bias at zero range
     */
    double rangeBias;

    /*!
     *  (Optional) Payload clock frequency scale factor
     *  standard deviation.
     */
    double clockFreqSF;
    /*!
     * (Optional) Transmit frequency scale factor
     *  standard deviation.
     */
    double transmitFreqSF;
    /*!
     *  (Optional) Range bias decorrelated rate
     *
     */
    DecorrType rangeBiasDecorr;
    const DecorrType& getRangeBiasDecorr() const
    {
        return rangeBiasDecorr;
    }

    RadarSensor() = default;

    //! Equality operator
    bool operator==(const RadarSensor& rhs) const
    {
        return (rangeBias == rhs.rangeBias && clockFreqSF == rhs.clockFreqSF &&
            transmitFreqSF == rhs.transmitFreqSF && getRangeBiasDecorr() == rhs.getRangeBiasDecorr());
    }

    bool operator!=(const RadarSensor& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct TropoError
 *  \brief (Optional) Troposphere delay error statistics
 *
 *  Contains troposphere delay error statistics
 */
struct TropoError
{
    /*!
     *  (Optional) Troposphere two-way delay error for normal
     *  incidence standard deviation.  Expressed as a
     *  two-range error
     */
    double tropoRangeVertical;

    /*!
     *  (Optional) Troposphere two-way delay error for SCP COA
     *  incidence angle standard deviation.  Expressed
     *  as a two-way range error
     */
    double tropoRangeSlant;

    /*!
     *  (Optional)
     *
     */
    DecorrType tropoRangeDecorr;
    const DecorrType& getTropoRangeDecorr() const
    {
        return tropoRangeDecorr;
    }

    TropoError() = default;

    bool operator==(const TropoError& rhs) const
    {
        return (tropoRangeVertical == rhs.tropoRangeVertical && tropoRangeSlant == rhs.tropoRangeSlant &&
            getTropoRangeDecorr() == rhs.getTropoRangeDecorr());
    }

    bool operator!=(const TropoError& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct IonoError
 *  \brief (Optional) Ionosphere delay error statistics
 *
 *  Contains Ionosphere delay error statistics
 */
struct IonoError
{
    /*!
     *  (Optional) Ionosphere two-way delay error for normal
     *  incidence standard deviation.  Expressed as a
     *  two-way range error
     */
    double ionoRangeVertical;

    /*!
     *  (Optional) Ionosphere two-way delay rate of change
     *  error for normal incidence standard deviation.
     *  Expressed as a two-way range error
     */
    double ionoRangeRateVertical;

    /*!
     *  Ionosphere range error and range rate error correlation
     *  coefficient.
     *
     */
    double ionoRgRgRateCC;
    
    /*!
     *  Ionosphere range error decorrelation ratio
     */
    DecorrType ionoRangeVertDecorr;
    const DecorrType& getIonoRangeVertDecorr() const
    {
        return ionoRangeVertDecorr;
    }

    IonoError() = default;

    //! Equality operator
    bool operator==(const IonoError& rhs) const
    {
        return (ionoRangeRateVertical == rhs.ionoRangeRateVertical && ionoRangeVertical == ionoRangeVertical &&
            ionoRgRgRateCC == rhs.ionoRgRgRateCC && getIonoRangeVertDecorr() == rhs.getIonoRangeVertDecorr());
    }

    bool operator!=(const IonoError& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct Components
 *  \brief (Optional) SICD/SIDD error components
 *
 *  (Optional) Error statistics by components.  All children are
 *  optional and thus, not initialized by the constructor
 */
struct Components
{
    //!  Constructor
    Components()
    {
    }

    mem::ScopedCopyablePtr<PosVelError> posVelError;
    mem::ScopedCopyablePtr<RadarSensor> radarSensor;
    mem::ScopedCopyablePtr<TropoError> tropoError;
    mem::ScopedCopyablePtr<IonoError> ionoError;

    //! Equality operator
    bool operator==(const Components& rhs) const
    {
        return (posVelError == rhs.posVelError && radarSensor == rhs.radarSensor &&
            tropoError == rhs.tropoError && ionoError == rhs.ionoError);
    }

    bool operator!=(const Components& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct CompositeSCP
 *  \brief SICD/SIDD CompositeSCP representation
 *
 *  Composite error statistics estimated at the scene center point
 *  Choose SCP type ROW_COL or RG_AZ
 *
 *  This object takes a shortcut.  Instead of providing two mutually
 *  exclusive sub-structures, one for RgAzErr and one for RowColErr,
 *  it provides one generalized alternative.  In the case where
 *  the data is defined as row/col, the values are written appropriately
 *  as RowColErr/Row, RowColErr/Col and RowColErr/RowCol.  In the case
 *  where they represent RgAz, we get RgAzErr/Rg RgAzErr/Az and RgAzErr/RgAz
 *  This only applies for SICD 0.4 and 0.5 - with SICD 1.0, this is always
 *  RG_AZ.
 *
 */
struct CompositeSCP
{
    //!  Types
    enum SCPType
    {
        ROW_COL, RG_AZ
    };

    //!  Constructor
    CompositeSCP(SCPType scpTypeIn = RG_AZ) :
        scpType(scpTypeIn)
    {
    }

    SCPType scpType;

    //!  SICD/SIDD Rg or Row, depending on scpType
    double xErr;

    //!  SICD/SIDD Az or Col, depending on scpType
    double yErr;

    //!  SICD/SIDD RowCol or RgAz depending on scpType
    double xyErr;

    //! Equality operators
    bool operator==(const CompositeSCP& rhs) const
    {
        return (xErr == rhs.xErr && yErr == rhs.yErr &&
            xyErr == rhs.xyErr && scpType == rhs.scpType);
    }

    bool operator!=(const CompositeSCP& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct ErrorStatistics
 *  \param SICD/SIDD ErrorStatistics block
 *
 *  Parameters needed for computing error statistics.
 *
 */
struct ErrorStatistics
{
    /*!
     *  (Optional) Composite error statistics estimated at the scene
     *  center point
     */
    mem::ScopedCopyablePtr<CompositeSCP> compositeSCP;

    /*!
     *  (Optional) error statistics components
     *
     */
    mem::ScopedCopyablePtr<Components> components;

    /*!
     *  Additional parameters
     *  Note, this is Parms in SICD, but we want a consistent API
     */
    ParameterCollection additionalParameters;

    ErrorStatistics()
    {
    }

    //! Equality operators
    bool operator==(const ErrorStatistics& rhs) const
    {
        return (compositeSCP == rhs.compositeSCP && components == rhs.components &&
            additionalParameters == rhs.additionalParameters);
    }

    bool operator!=(const ErrorStatistics& rhs) const
    {
        return !(*this == rhs);
    }
};
}

#endif

