/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#ifndef __SIX_IMAGE_FORMATION_H__
#define __SIX_IMAGE_FORMATION_H__

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"

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
    //!  Constructor, everything undefined
    RcvChannelProcessed()
    {
        numChannelsProcessed = Init::undefined<unsigned int>();

        // Optional
        prfScaleFactor = Init::undefined<double>();
    }

    //!  Create a clone
    RcvChannelProcessed* clone() const
    {
        return new RcvChannelProcessed(*this);
    }

    //!  Destructor
    ~RcvChannelProcessed()
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

    /*!
     *  (Optional) Date of calibration of measurement
     *
     */
    DateTime calibrationDate;

    //!  Absolute amplitude scale factor
    double a;

    //! Recv distortion element (2,2)
    std::complex<double> f1;

    //! Recv distortion element (1,2)
    std::complex<double> q1;

    //! Recv distortion element (2,1)
    std::complex<double> q2;

    //! Transmit distortion element (2,2)
    std::complex<double> f2;

    //! Transmit distortion element (2,1)
    std::complex<double> q3;

    //! Transmit distortion element (1,2)
    std::complex<double> q4;

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

    //! Constructor
    Distortion()
    {
    }

    //! Destructor
    ~Distortion()
    {
    }

    //!  Make a deep copy of this
    Distortion* clone() const
    {
        return new Distortion(*this);
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
    Distortion* distortion;

    //!  Constructor, NULLs distortion
    PolarizationCalibration() :
        distortion(NULL)
    {
        hvAngleCompensationApplied = Init::undefined<BooleanType>();
        distortionCorrectionApplied = Init::undefined<BooleanType>();
    }
    //!  Destroy including distortion if non-NULL
    ~PolarizationCalibration()
    {
        if (distortion)
            delete distortion;
    }

    //!  Clone, including distortion if non-NULL
    PolarizationCalibration* clone() const
    {
        PolarizationCalibration* p = new PolarizationCalibration(*this);
        if (p->distortion)
        {
            p->distortion = distortion->clone();
        }
        return p;

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
    Processing()
    {
        applied = Init::undefined<BooleanType>();
    }

    //! Destructor.
    ~Processing()
    {
    }

    //! Text describing the type of processing applied
    std::string type;

    //! Flag indicating whether process has been applied
    BooleanType applied;

    //! (Optional) Additional parameters
    std::vector<Parameter> parameters;
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
    ImageFormation() :
        rcvChannelProcessed(NULL), polarizationCalibration(NULL)
    {
        // Assumption here for now
        imageFormationAlgorithm = ImageFormationType::PFA;

        tStartProc = Init::undefined<double>();
        tEndProc = Init::undefined<double>();
        txFrequencyProcMin = Init::undefined<double>();
        txFrequencyProcMax = Init::undefined<double>();
        slowTimeBeamCompensation = SlowTimeBeamCompensationType::NO;
        imageBeamCompensation = ImageBeamCompensationType::NO;
        azimuthAutofocus = AutofocusType::NO;
        rangeAutofocus = AutofocusType::NO;
        txRcvPolarizationProc = DualPolarizationType::NOT_SET;
        rcvChannelProcessed = new RcvChannelProcessed();
    }

    //!  Destructor
    ~ImageFormation();

    //!  Clone, including any non-NULL objects
    ImageFormation* clone() const;

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
    RcvChannelProcessed* rcvChannelProcessed;

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
    PolarizationCalibration* polarizationCalibration;
};

}
}
#endif
