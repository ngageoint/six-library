/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_IMAGE_FORMATION_H__
#define __SIX_IMAGE_FORMATION_H__

#include "six/Types.h"
#include "six/Init.h"
#include "six/ParameterCollection.h"
#include "six/sicd/RadarCollection.h"

#include <mem/ScopedCopyablePtr.h>
#include <mem/ScopedCloneablePtr.h>

namespace six
{
namespace sicd
{

/*!
 *  \struct RcvChannelProcessed
 *  \brief SICD RcvChanProc
 *
 *  Parameters of the received processed channel
 */
struct RcvChannelProcessed
{
    //!  Constructor
    RcvChannelProcessed() :
        numChannelsProcessed(Init::undefined<unsigned int>()),
        prfScaleFactor(Init::undefined<double>())
    {
    }

    //! Number of recieve data channels processed to form the image
    unsigned int numChannelsProcessed;

    //!  Factor indicating the ratio of the effective PRF to the actual
    //!  due to combined receive channels
    double prfScaleFactor;

    //! Index of a data channel that was processed
    //! \todo Would this really be out of order?
    std::vector<int> channelIndex;

    //! Equality operator
    bool operator==(const RcvChannelProcessed& rhs) const
    {
        return (numChannelsProcessed == rhs.numChannelsProcessed &&
            prfScaleFactor == rhs.prfScaleFactor && channelIndex == rhs.channelIndex);
    }

    bool operator!=(const RcvChannelProcessed& rhs) const
    {
        return !(*this == rhs);
    }

};

/*!
 *  \struct Distortion
 *  \brief SICD Distortion parameters
 *
 *  Distortion correction parameters.
 *
 *  The model for H&V channels when H&V channels are aligned with
 *  true H&V directions:
 *  \verbatim

 |         |       |        | |         | |        |
 | Ohh Ovh |       | 1   Q1 | | Shh Svh | | 1   Q4 |
 |         | = A * |        | |         | |        |
 | Ohv Ovv |       | Q2  F1 | | Shv Svv | | Q3  F2 |
 |         |       |        | |         | |        |

 *  \endverbatim
 *  \todo Apply this transformation?
 *
 */
struct Distortion
{
    //! Constructor
    Distortion();

    /*!
     *  (Optional) Date of calibration of measurement
     *
     */
    DateTime calibrationDate;

    //!  Absolute amplitude scale factor
    double a;

    //! Recv distortion element (2,2)
    six::zdouble f1;

    //! Recv distortion element (1,2)
    six::zdouble q1;

    //! Recv distortion element (2,1)
    six::zdouble q2;

    //! Transmit distortion element (2,2)
    six::zdouble f2;

    //! Transmit distortion element (2,1)
    six::zdouble q3;

    //! Transmit distortion element (1,2)
    six::zdouble q4;

    /*!
     *  (Optional) Gain estimation error standard deviation (in dB) for
     *  parameter A
     */
    double gainErrorA;

    /*!
     *  (Optional) Gain estimation error standard deviation (in dB) for
     *  parameter F1
     */
    double gainErrorF1;

    /*!
     *  (Optional) Gain estimation error standard deviation (in dB) for
     *  parameter F2
     */
    double gainErrorF2;

    /*!
     *  (Optional) Phase estimation error standard deviation (in radians)
     *  for parameter F1
     */
    double phaseErrorF1;

    /*!
     *  (Optional) Phase estimation error standard deviation (in radians)
     *  for parameter F2
     */
    double phaseErrorF2;

    //! Equality operators
    bool operator==(const Distortion& rhs) const;
    bool operator!=(const Distortion& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct PolarizationCalibration
 *  \brief (Optional) SICD PolarizationCalibration parameter
 *
 *  (Optional) Parameters describing the polarization calibration applied
 *  (if any)
 */
struct PolarizationCalibration
{
    //!  Constructor, NULLs distortion
    PolarizationCalibration() :
        hvAngleCompensationApplied(Init::undefined<BooleanType>()),
        distortionCorrectionApplied(Init::undefined<BooleanType>())
    {
    }

    /*!
     *  Parameter indicating if compensation for H&V channel misalignment
     *  has been applied
     */
    BooleanType hvAngleCompensationApplied;

    /*!
     *  Parameter indicating if polarization calibration processing
     *  has been applied
     */
    BooleanType distortionCorrectionApplied;

    /*!
     *  Distortion parameters
     */
    mem::ScopedCopyablePtr<Distortion> distortion;

    //! Equality operators
    bool operator==(const PolarizationCalibration& rhs) const
    {
        return (hvAngleCompensationApplied == rhs.hvAngleCompensationApplied &&
            distortionCorrectionApplied == rhs.distortionCorrectionApplied && distortion == rhs.distortion);
    }

    bool operator!=(const PolarizationCalibration& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 * \struct Processing
 * \brief  SICD Image formation processing block
 *
 * Describes types of specific processing that may have been applied.
 *
 */
struct Processing
{
    //! Constructor.
    Processing() :
        applied(Init::undefined<BooleanType>())
    {
    }

    //! Text describing the type of processing applied
    std::string type;

    //! Flag indicating whether process has been applied
    BooleanType applied;

    //! (Optional) Additional parameters
    ParameterCollection parameters;

    //! Equality operators
    bool operator==(const Processing& rhs) const
    {
        return (type == rhs.type && applied == rhs.applied &&
            parameters == rhs.parameters);
    }

    bool operator!=(const Processing& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct ImageFormation
 *  \brief SICD ImageFormation block
 *
 *  Describes the image formation process
 *
 */
struct ImageFormation
{
    //!  Constructor, nothing is initialized, except required rcv channel
    ImageFormation();

    /*!
     *  Identifier that describes the image that was processed
     *  Must be included when a radarCollection->area->plane::segmentList
     *  is included
     */
    std::string segmentIdentifier;

    /*!
     *  Parameters of the rcv processed channel
     *
     */
    mem::ScopedCopyablePtr<RcvChannelProcessed> rcvChannelProcessed;

    /*!
     *  Indicates the combined tx and rcv polarization processed
     *  to form the image
     */
    DualPolarizationType txRcvPolarizationProc;

    /*!
     *  Image formation algorithm used.  Note that the SICD D&E
     *  allows PFA, RMA, CSA, RDA, and other, even though PFA is
     *  currently mandatory
     */
    ImageFormationType imageFormationAlgorithm;

    /*!
     *  Earliest slow time value for data processed to form the image
     *  from collection start
     */
    double tStartProc;

    /*!
     *  Latest slow time value for data processed to form the image
     *  from collection start
     */
    double tEndProc;

    /*!
     *  This key maps to TxFrequencyProc.MinProc.  This is the minimum
     *  transmit frequency processed to form the image
     *
     */
    double txFrequencyProcMin;

    /*!
     *  This key maps to TxFrequencyProc.MaxProc and is the maximum
     *  transmit frequency processed to form the image
     */
    double txFrequencyProcMax;

    /*!
     *  Slow time beam shape compensation parameter
     */
    SlowTimeBeamCompensationType slowTimeBeamCompensation;

    /*!
     *  Image domain beam shape compensation parameter
     *
     */
    ImageBeamCompensationType imageBeamCompensation;

    /*!
     *  Integer parameter indicating if azimuth autofocus corrections
     *  were applied
     */
    AutofocusType azimuthAutofocus;

    /*!
     *  Integer parameters indicating if range autofocus corrections
     *  were applied
     */
    AutofocusType rangeAutofocus;

    /*!
     * (Optional) Parameters to describe types of specific processing
     * that may have been applied such as additional compensations.
     */
    std::vector<Processing> processing;

    /*!
     *  Parameters describing the polarization calibration applied
     *  (if any)
     */
    mem::ScopedCopyablePtr<PolarizationCalibration> polarizationCalibration;

    //! Equality operators
    bool operator==(const ImageFormation& rhs) const;
    bool operator!=(const ImageFormation& rhs) const
    {
        return !(*this == rhs);
    }

    void fillDefaultFields(const RadarCollection& radarCollection);
};

}
}
#endif

