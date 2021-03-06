/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#include "six/sidd/Display.h"

namespace six
{
namespace sidd
{
MonitorCompensationApplied::MonitorCompensationApplied() :
    gamma(six::Init::undefined<double>()),
    xMin(six::Init::undefined<double>())
{
}

DRAHistogramOverrides::DRAHistogramOverrides() :
    clipMin(six::Init::undefined<int>()),
    clipMax(six::Init::undefined<int>())
{
}

DynamicRangeAdjustment::DRAParameters::DRAParameters() :
    pMin(six::Init::undefined<double>()),
    pMax(six::Init::undefined<double>()),
    eMinModifier(six::Init::undefined<double>()),
    eMaxModifier(six::Init::undefined<double>())
{
}

DynamicRangeAdjustment::DRAOverrides::DRAOverrides() :
    subtractor(six::Init::undefined<double>()),
    multiplier(six::Init::undefined<double>())
{
}

bool MonochromeDisplayRemap::equalTo(const Remap& rhs) const
{
    const MonochromeDisplayRemap* remap =
            dynamic_cast<const MonochromeDisplayRemap*>(&rhs);
    if (remap != nullptr)
    {
        return *this == *remap;
    }
    return false;
}

bool MonochromeDisplayRemap::operator==(const MonochromeDisplayRemap& rhs) const
{
    return (remapType == rhs.remapType &&
            remapParameters == rhs.remapParameters && remapLUT == rhs.remapLUT);
}

bool ColorDisplayRemap::equalTo(const Remap& rhs) const
{
    ColorDisplayRemap const* remap =
            dynamic_cast<ColorDisplayRemap const*>(&rhs);
    if (remap != nullptr)
    {
        return *this == *remap;
    }
    return false;
}

bool ColorDisplayRemap::operator==(const ColorDisplayRemap& rhs) const
{
    return remapLUT == rhs.remapLUT;
}

bool RRDS::operator==(const RRDS& rhs) const
{
    return (downsamplingMethod == rhs.downsamplingMethod &&
            antiAlias == rhs.antiAlias && interpolation == rhs.interpolation);
}

bool ProductGenerationOptions::operator==(
        const ProductGenerationOptions& rhs) const
{
    return (bandEqualization == rhs.bandEqualization &&
            modularTransferFunctionRestoration ==
                    rhs.modularTransferFunctionRestoration &&
            dataRemapping == rhs.dataRemapping &&
            asymmetricPixelCorrection == rhs.asymmetricPixelCorrection);
}

bool BandEqualization::operator==(const BandEqualization& rhs) const
{
    return (algorithm == rhs.algorithm && bandLUTs == rhs.bandLUTs);
}

bool NonInteractiveProcessing::operator==(
        const NonInteractiveProcessing& rhs) const
{
    return (productGenerationOptions == rhs.productGenerationOptions &&
            rrds == rhs.rrds);
}

bool Scaling::operator==(const Scaling& rhs) const
{
    return antiAlias == rhs.antiAlias && interpolation == rhs.interpolation;
}

bool ColorManagementModule::operator==(const ColorManagementModule& rhs) const
{
    return (renderingIntent == rhs.renderingIntent &&
            sourceProfile == rhs.sourceProfile &&
            displayProfile == rhs.displayProfile &&
            iccProfile == rhs.iccProfile);
}

bool ColorManagementModule::operator!=(const ColorManagementModule& rhs) const
{
    return !(*this == rhs);
}

bool GeometricTransform::operator==(const GeometricTransform& rhs) const
{
    return scaling == rhs.scaling && orientation == rhs.orientation;
}

bool SharpnessEnhancement::operator==(const SharpnessEnhancement& rhs) const
{
    return (modularTransferFunctionCompensation ==
                    rhs.modularTransferFunctionCompensation &&
            modularTransferFunctionEnhancement ==
                    rhs.modularTransferFunctionEnhancement);
}

bool DynamicRangeAdjustment::DRAParameters::operator==(
        const DynamicRangeAdjustment::DRAParameters& rhs) const
{
    return (pMin == rhs.pMin && pMax == rhs.pMax &&
            eMinModifier == rhs.eMinModifier &&
            eMaxModifier == rhs.eMaxModifier);
}

bool DynamicRangeAdjustment::DRAOverrides::operator==(
        const DynamicRangeAdjustment::DRAOverrides& rhs) const
{
    return (subtractor == rhs.subtractor && multiplier == rhs.multiplier);
}

bool DynamicRangeAdjustment::operator==(const DynamicRangeAdjustment& rhs) const
{
    return (draParameters == rhs.draParameters &&
            draOverrides == rhs.draOverrides);
}

bool InteractiveProcessing::operator==(const InteractiveProcessing& rhs) const
{
    return (geometricTransform == rhs.geometricTransform &&
            sharpnessEnhancement == rhs.sharpnessEnhancement &&
            colorSpaceTransform == rhs.colorSpaceTransform &&
            dynamicRangeAdjustment == rhs.dynamicRangeAdjustment &&
            tonalTransferCurve == rhs.tonalTransferCurve);
}

bool Display::operator==(const Display& rhs) const
{
    return (pixelType == rhs.pixelType &&
            remapInformation == rhs.remapInformation &&
            magnificationMethod == rhs.magnificationMethod &&
            decimationMethod == rhs.decimationMethod &&
            histogramOverrides == rhs.histogramOverrides &&
            monitorCompensationApplied == rhs.monitorCompensationApplied &&
            numBands == rhs.numBands &&
            defaultBandDisplay == rhs.defaultBandDisplay &&
            nonInteractiveProcessing == rhs.nonInteractiveProcessing &&
            interactiveProcessing == rhs.interactiveProcessing &&
            displayExtensions == rhs.displayExtensions);
}
}
}
