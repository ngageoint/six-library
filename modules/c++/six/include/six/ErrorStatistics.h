/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
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

#include "six/Types.h"
#include "six/Utilities.h"
#include "six/Init.h"
#include "six/Parameter.h"
#include <import/str.h>
#include <mem/ScopedCloneablePtr.h>

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
    CorrCoefs* clone() const
    {
        return new CorrCoefs(*this);
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
    //!  CorrCoefs are NULL, since optional
    PosVelError() :
        corrCoefs(NULL),
        positionDecorr(Init::undefined<DecorrType>())
    {
    }

    PosVelError* clone() const
    {
        return new PosVelError(*this);
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
    mem::ScopedCloneablePtr<CorrCoefs> corrCoefs;

    //! Can be none, make sure to set this undefined()
    DecorrType positionDecorr;
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

    //!  Constructor
    RadarSensor();

    //!  Clone this object
    RadarSensor* clone() const;
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

    //!  Constructor
    TropoError();

    //!  Clone this object
    TropoError* clone() const;
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

    //!  Constructor
    IonoError();

    //!  Clone this object
    IonoError* clone() const;
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

    //!  Clone this object
    Components* clone() const;

    mem::ScopedCloneablePtr<PosVelError> posVelError;
    mem::ScopedCloneablePtr<RadarSensor> radarSensor;
    mem::ScopedCloneablePtr<TropoError> tropoError;
    mem::ScopedCloneablePtr<IonoError> ionoError;
};

/*!
 *  \struct CompositeSCP
 *  \brief SICD/SIDD CompositeSCP representation
 *
 *  This object takes a shortcut.  Instead of providing two mutually
 *  exclusive sub-structures, one for RgAzErr and one for RowColErr,
 *  it provides one generalized alternative.  In the case where
 *  the data is defined as row/col, the values are written appropriately
 *  as RowColErr/Row, RowColErr/Col and RowColErr/RowCol.  In the case
 *  where they represent RgAz, we get RgAzErr/Rg RgAzErr/Az and RgAzErr/RgAz
 *
 */
struct CompositeSCP
{
    //!  Constructor
    CompositeSCP()
    {
    }

    CompositeSCP* clone() const;

    //!  SICD/SIDD Rg or Row, depending on scpType
    double xErr;

    //!  SICD/SIDD Az or Col, depending on scpType
    double yErr;

    //!  SICD/SIDD RowCol or RgAz depending on scpType
    double xyErr;
};

/*!
 *  \struct ErrorStatistics
 *  \param SICD/SIDD ErrorStatistics block
 *
 *  Parameters needed for computing error statistics.
 *  Composite error statistics estimated at the scene center point
 *  Choose SCP type ROW_COL or RG_AZ
 *
 */
struct ErrorStatistics
{
    //!  Types
    enum SCPType
    {
        ROW_COL, RG_AZ
    };

    //!  Note that how this is defined determines how CompositeSCP
    //!  Handled    
    SCPType scpType;

    /*!
     *  (Optional) Composite error statistcis estimated at the scene
     *  center point
     */
    mem::ScopedCloneablePtr<CompositeSCP> compositeSCP;

    /*!
     *  (Optional) error statistics components
     *
     */
    mem::ScopedCloneablePtr<Components> components;

    /*!
     *  Additional parameters
     *  Note, this is Parms in SICD, but we want a consistent API
     */
    std::vector<Parameter> additionalParameters;

    /*!
     *  Note that scpType is not prepared, and must be supplied
     *  for us to write
     */
    ErrorStatistics()
    {
    }

    //!  Clone, including non-NULL sub-objects
    ErrorStatistics* clone() const;

    /*!
     *  \todo this is an API anomaly.  Revisit and remove if possible.
     */
    void initialize(SCPType type);
};
}

#endif

