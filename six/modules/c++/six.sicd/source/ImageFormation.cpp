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
#include "six/sicd/ImageFormation.h"
#include "six/sicd/RadarCollection.h"

using namespace six;
using namespace six::sicd;


Distortion::Distortion() :
    calibrationDate(Init::undefined<six::DateTime>()),
    a(Init::undefined<double>()),
    f1(Init::undefined<six::zdouble >()),
    q1(Init::undefined<six::zdouble >()),
    q2(Init::undefined<six::zdouble >()),
    f2(Init::undefined<six::zdouble >()),
    q3(Init::undefined<six::zdouble >()),
    q4(Init::undefined<six::zdouble >()),
    gainErrorA(Init::undefined<double>()),
    gainErrorF1(Init::undefined<double>()),
    gainErrorF2(Init::undefined<double>()),
    phaseErrorF1(Init::undefined<double>()),
    phaseErrorF2(Init::undefined<double>())
{
}

bool Distortion::operator==(const Distortion& rhs) const
{
    return (calibrationDate == rhs.calibrationDate &&
        a == rhs.a &&
        f1 == rhs.f1 &&
        q1 == rhs.q1 &&
        q2 == rhs.q2 &&
        f2 == rhs.f2 &&
        q3 == rhs.q3 &&
        q4 == rhs.q4 &&
        gainErrorA == rhs.gainErrorA &&
        gainErrorF1 == rhs.gainErrorF1 &&
        gainErrorF2 == rhs.gainErrorF2 &&
        phaseErrorF1 == rhs.phaseErrorF1 &&
        phaseErrorF2 == rhs.phaseErrorF2);
}

ImageFormation::ImageFormation() :
    rcvChannelProcessed(new RcvChannelProcessed()),
    txRcvPolarizationProc(DualPolarizationType::NOT_SET),
    imageFormationAlgorithm(ImageFormationType::PFA),
    tStartProc(Init::undefined<double>()),
    tEndProc(Init::undefined<double>()),
    txFrequencyProcMin(Init::undefined<double>()),
    txFrequencyProcMax(Init::undefined<double>()),
    slowTimeBeamCompensation(SlowTimeBeamCompensationType::NO),
    imageBeamCompensation(ImageBeamCompensationType::NO),
    azimuthAutofocus(AutofocusType::NO),
    rangeAutofocus(AutofocusType::NO)
{
}

bool ImageFormation::operator==(const ImageFormation& rhs) const
{
    return (segmentIdentifier == rhs.segmentIdentifier &&
        rcvChannelProcessed == rhs.rcvChannelProcessed &&
        txRcvPolarizationProc == rhs.txRcvPolarizationProc &&
        imageFormationAlgorithm == rhs.imageFormationAlgorithm &&
        tStartProc == rhs.tStartProc &&
        tEndProc == rhs.tEndProc &&
        txFrequencyProcMin == rhs.txFrequencyProcMin &&
        txFrequencyProcMax == rhs.txFrequencyProcMax &&
        slowTimeBeamCompensation == rhs.slowTimeBeamCompensation &&
        imageBeamCompensation == rhs.imageBeamCompensation &&
        azimuthAutofocus == rhs.azimuthAutofocus &&
        rangeAutofocus == rhs.rangeAutofocus &&
        processing == rhs.processing &&
        polarizationCalibration == rhs.polarizationCalibration);
}

void ImageFormation::fillDefaultFields(const RadarCollection& radarCollection)
{
    if (!Init::isUndefined(radarCollection.txFrequencyMin) &&
        !Init::isUndefined(radarCollection.txFrequencyMax))
    {
        // Default: we often assume that all transmitted bandwidth was
        // processed, if given no other information
        if (Init::isUndefined(txFrequencyProcMin))
        {
            txFrequencyProcMin = radarCollection.txFrequencyMin;
        }
        if (Init::isUndefined(txFrequencyProcMax))
        {
            txFrequencyProcMax = radarCollection.txFrequencyMax;
        }
    }
}
