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

#include <assert.h>

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
    CorrCoefs() = default;

    // From Bill: "Volume 3: States if correlation coefficients omitted then set to 0."
    double p1p2 = 0.0;
    double p1p3 = 0.0;
    double p1v1 = 0.0;
    double p1v2 = 0.0;
    double p1v3 = 0.0;
    double p2p3 = 0.0;
    double p2v1 = 0.0;
    double p2v2 = 0.0;
    double p2v3 = 0.0;
    double p3v1 = 0.0;
    double p3v2 = 0.0;
    double p3v3 = 0.0;
    double v1v2 = 0.0;
    double v1v3 = 0.0;
    double v2v3 = 0.0;

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
    PosVelError() = default;

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
    DecorrType positionDecorr = Init::undefined<DecorrType>();
    InitRef<DecorrType> PositionDecorr(const std::optional<DecorrType>& v = std::optional<DecorrType>()) const
    {
        return make_InitRef(positionDecorr, v);
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
    double rangeBias = Init::undefined<double>();
    InitRef<double> RangeBias(const std::optional<double>& v = std::optional<double>()) const
    {
        return make_InitRef(rangeBias, v);
    }

    /*!
     *  (Optional) Payload clock frequency scale factor
     *  standard deviation.
     */
    double clockFreqSF = 0.0; // From Bill: "Volume 3: States if ClockFreqSF is omitted then set to 0."
    InitRef<double> ClockFreqSF(const std::optional<double>& v = std::optional<double>()) const
    {
        return make_InitRef(clockFreqSF, v);
    }

    /*!
     * (Optional) Transmit frequency scale factor
     *  standard deviation.
     */
    double transmitFreqSF = 0.0; // From Bill: "No mention of the TransmitFreqSF in volume 3 ... would still recommend setting to 0."
    InitRef<double> TransmitFreqSF(const std::optional<double>& v = std::optional<double>()) const
    {
        return make_InitRef(clockFreqSF, v);
    }

    /*!
     *  (Optional) Range bias decorrelated rate
     *
     */
    DecorrType rangeBiasDecorr = Init::undefined<DecorrType>();
    InitRef<DecorrType> RangeBiasDecorr(const std::optional<DecorrType>& v = std::optional<DecorrType>()) const
    {
        return make_InitRef(rangeBiasDecorr, v);
    }

    RadarSensor() = default;

    //! Equality operator
    bool operator==(const RadarSensor& rhs) const
    {
        return (rangeBias == rhs.rangeBias && clockFreqSF == rhs.clockFreqSF &&
            transmitFreqSF == rhs.transmitFreqSF && rangeBiasDecorr == rhs.rangeBiasDecorr);
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
    double tropoRangeVertical = Init::undefined<double>();
    InitRef<double> TropoRangeVertical(const std::optional<double>& v = std::optional<double>()) const
    {
        return make_InitRef(tropoRangeVertical, v);
    }

    /*!
     *  (Optional) Troposphere two-way delay error for SCP COA
     *  incidence angle standard deviation.  Expressed
     *  as a two-way range error
     */
    double tropoRangeSlant = Init::undefined<double>();
    InitRef<double> TropoRangeSlant(const std::optional<double>& v = std::optional<double>()) const
    {
        return make_InitRef(tropoRangeSlant, v);
    }

    /*!
     *  (Optional)
     *
     */
    DecorrType tropoRangeDecorr = Init::undefined<DecorrType>();
    InitRef<DecorrType> TropoRangeDecorr(const std::optional<DecorrType>& v = std::optional<DecorrType>()) const
    {
        return make_InitRef(tropoRangeDecorr, v);
    }

    TropoError() = default;

    bool operator==(const TropoError& rhs) const
    {
        return (tropoRangeVertical == rhs.tropoRangeVertical && tropoRangeSlant == rhs.tropoRangeSlant &&
            tropoRangeDecorr == rhs.tropoRangeDecorr);
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
    double ionoRangeVertical = Init::undefined<double>();
    InitRef<double> IonoRangeVertical(const std::optional<double>& v = std::optional<double>()) const
    {
        return make_InitRef(ionoRangeVertical, v);
    }

    /*!
     *  (Optional) Ionosphere two-way delay rate of change
     *  error for normal incidence standard deviation.
     *  Expressed as a two-way range error
     */
    double ionoRangeRateVertical = Init::undefined<double>();
    InitRef<double> IonoRangeRateVertical(const std::optional<double>& v = std::optional<double>()) const
    {
        return make_InitRef(ionoRangeRateVertical, v);
    }

    /*!
     *  Ionosphere range error and range rate error correlation
     *  coefficient.
     *
     */
    double ionoRgRgRateCC = Init::undefined<double>();
    InitRef<double> IonoRgRgRateCC(const std::optional<double>& v = std::optional<double>()) const
    {
        return make_InitRef(ionoRgRgRateCC, v);
    }
    
    /*!
     *  Ionosphere range error decorrelation ratio
     */
    DecorrType ionoRangeVertDecorr = Init::undefined<DecorrType>();
    InitRef<DecorrType> IonoRangeVertDecorr(const std::optional<DecorrType>& v = std::optional<DecorrType>()) const
    {
        return make_InitRef(ionoRangeVertDecorr, v);
    }

    IonoError() = default;

    //! Equality operator
    bool operator==(const IonoError& rhs) const
    {
        return (ionoRangeRateVertical == rhs.ionoRangeRateVertical && ionoRangeVertical == ionoRangeVertical &&
            ionoRgRgRateCC == rhs.ionoRgRgRateCC && ionoRangeVertDecorr == rhs.ionoRangeVertDecorr);
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
    Components() = default;

    mem::ScopedCopyablePtr<PosVelError> posVelError;
    void reset(PosVelError* value)
    {
        posVelError.reset(value);
    }
    const PosVelError* getPosVelError() const
    {
        return posVelError.get();
    }
    PosVelError* getPosVelError()
    {
        return posVelError.get();
    }

    mem::ScopedCopyablePtr<RadarSensor> radarSensor;
    void reset(RadarSensor* value)
    {
        radarSensor.reset(value);
    }
    const RadarSensor* getRadarSensor() const
    {
        return radarSensor.get();
    }
    RadarSensor* getRadarSensor()
    {
        return radarSensor.get();
    }

    mem::ScopedCopyablePtr<TropoError> tropoError;
    void reset(TropoError* value)
    {
        tropoError.reset(value);
    }
    const TropoError* getTropoError() const
    {
        return tropoError.get();
    }
    TropoError* getTropoError()
    {
        return tropoError.get();
    }

    mem::ScopedCopyablePtr<IonoError> ionoError;
    void reset(IonoError* value)
    {
        ionoError.reset(value);
    }
    const IonoError* getIonoError() const
    {
        return ionoError.get();
    }
    IonoError* getIonoError()
    {
        return ionoError.get();
    }

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

    SCPType scpType = RG_AZ;

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
    void reset(CompositeSCP* pValue)
    {
        compositeSCP.reset(pValue);
    }
    const CompositeSCP* getCompositeSCP() const
    {
        return compositeSCP.get();
    }
    CompositeSCP* getCompositeSCP()
    {
        return compositeSCP.get();
    }

    /*!
     *  (Optional) error statistics components
     *
     */
    mem::ScopedCopyablePtr<Components> components;
    void reset(Components* pValue)
    {
        components.reset(pValue);
    }
    const Components* getComponents() const
    {
        return components.get();
    }
    Components* getComponents()
    {
        return components.get();
    }

    /*!
     *  Additional parameters
     *  Note, this is Parms in SICD, but we want a consistent API
     */
    ParameterCollection additionalParameters;

    ErrorStatistics() = default;

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

