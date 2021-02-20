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
#ifndef __SIX_DISPLAY_H__
#define __SIX_DISPLAY_H__

#include <sys/Optional.h>
#include <mem/ScopedCopyablePtr.h>
#include <mem/ScopedCloneablePtr.h>

#include <six/Types.h>
#include <six/Init.h>
#include <six/Parameter.h>
#include <six/ParameterCollection.h>
#include <six/sidd/Enums.h>
#include <six/sidd/Filter.h>
#include <six/sidd/LookupTable.h>

namespace six
{
namespace sidd
{
/*!
 *  \struct Remap
 *  \brief A remap type
 *  \todo Change this, should init its LUT if necessary
 *
 *  Base class for remaps.  Contains LUT, display type, and
 *  clone() function so that objects can be created easily.
 *  This class does not auto-create any LUT, it defaults to nullptr.
 *  Neither do the derived classes.
 */
struct Remap
{
    Remap(LUT* lut = nullptr) :
        remapLUT(lut)
    {
    }

    virtual ~Remap()
    {
    }

    DisplayType displayType;
    mem::ScopedCopyablePtr<LUT> remapLUT; // MonoLUT or ColorLUT or nullptr

    virtual Remap* clone() const = 0;

    bool operator==(const Remap& rhs)
    {
        return this->equalTo(rhs);
    }

    bool operator!=(const Remap& rhs)
    {
        return !(*this == rhs);
    }

    virtual bool equalTo(const Remap& rhs) const = 0;

};

/*!
 *  \struct MonochromeDisplayRemap
 *  \brief Mono remap derived class
 *
 *  Monochrome remap.  Correctly sets the remap info for
 *  MONO
 *
 */
struct MonochromeDisplayRemap : public Remap
{
    /*!
     *  Create a remap object, but does not set up a LUT.  LUT
     *  remains uninitialized
     */
    MonochromeDisplayRemap(std::string _remapType, LUT* lut = nullptr) :
        Remap(lut), remapType(_remapType)
    {
        this->displayType = DisplayType::MONO;
    }

    /*!
     *  Clone this object, and any sub-LUT
     *
     */
    virtual Remap* clone() const
    {
        return new MonochromeDisplayRemap(*this);
    }

    //!  The remap type
    std::string remapType;

    //!  Remap parameters
    ParameterCollection remapParameters;

    virtual bool equalTo(const Remap& rhs) const;
    virtual bool operator==(const MonochromeDisplayRemap& rhs) const;
};

/*!
 *  \struct ColorDisplayRemap
 *  \brief Color remap object, leaves remap as nullptr
 *
 *  A color display remap contains clipping information, and possibly
 *  also contains a remap LUT.
 */
struct ColorDisplayRemap : public Remap
{
    //!  Constructor
    ColorDisplayRemap(LUT* lut = nullptr) :
        Remap(lut)
    {
        this->displayType = DisplayType::COLOR;
    }

    //!  Clone the remap
    virtual Remap* clone() const
    {
        return new ColorDisplayRemap(*this);
    }

    virtual bool equalTo(const Remap& rhs) const;
    virtual bool operator==(const ColorDisplayRemap& rhs) const;
};

/*!
 *  \struct MonitorCompensationApplied
 *  \brief Monitor compensation parameters
 *
 *  On construction, these parameters are set to be optional
 *
 */
struct MonitorCompensationApplied
{
    double gamma;
    double xMin;

    //! Constructor
    MonitorCompensationApplied();

    //! Equality operator
    bool operator==(const MonitorCompensationApplied& rhs) const
    {
        return gamma == rhs.gamma && xMin == rhs.xMin;
    }

    bool operator!=(const MonitorCompensationApplied& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct DRAHistogramOverrides
 *  \brief
 *
 */
struct DRAHistogramOverrides
{
    DRAHistogramOverrides();

    /*!
     *  Suggested override for the low clip point in the
     *  ELT DRA application.  Referred to as Pmin in SIPS docs.
     */
    int clipMin;

    /*!
     *  Suggested override for the high clip point in the ELT DRA
     *  application.  Referred to as Pmax in SIPS docs.
     *
     */
    int clipMax;

    //! Equality operator
    bool operator==(const DRAHistogramOverrides& rhs) const
    {
        return clipMin == rhs.clipMin && clipMax == rhs.clipMax;
    }

    bool operator!=(const DRAHistogramOverrides& rhs) const
    {
        return !(*this == rhs);
    }
};

struct BandEqualization
{
    BandEqualizationAlgorithm algorithm;
    std::vector<mem::ScopedCopyablePtr<LookupTable> > bandLUTs;

    bool operator==(const BandEqualization& rhs) const;
    bool operator!=(const BandEqualization& rhs) const
    {
        return !(*this == rhs);
    }
};

struct RRDS
{
    //! Algorithm used to perform RRDS downsampling
    DownsamplingMethod downsamplingMethod;

    /*!
     * Anti-aliasing filter.  Included for all values except
     * downsamplingMethod = DECIMATE or MAX_PIXEL.
     */
    mem::ScopedCopyablePtr<Filter> antiAlias;

    /*!
     * Interpolation filter.  Included for all values except
     * downsamplingMethod = DECIMATE or MAX_PIXEL.
     */
    mem::ScopedCopyablePtr<Filter> interpolation;

    bool operator==(const RRDS& rhs) const;
    bool operator!=(const RRDS& rhs) const
    {
        return !(*this == rhs);
    }
};

struct ProductGenerationOptions
{
    mem::ScopedCopyablePtr<BandEqualization> bandEqualization;
    mem::ScopedCopyablePtr<Filter> modularTransferFunctionRestoration;

    // Required
    mem::ScopedCopyablePtr<LookupTable> dataRemapping;

    mem::ScopedCopyablePtr<Filter> asymmetricPixelCorrection;

    bool operator==(const ProductGenerationOptions& rhs) const;
    bool operator!=(const ProductGenerationOptions& rhs) const
    {
        return !(*this == rhs);
    }
};

struct NonInteractiveProcessing
{
    ProductGenerationOptions productGenerationOptions;
    RRDS rrds;

    bool operator==(const NonInteractiveProcessing& rhs) const;
    bool operator!=(const NonInteractiveProcessing& rhs) const
    {
        return !(*this == rhs);
    }
};

struct Scaling
{
    Filter antiAlias;
    Filter interpolation;

    bool operator==(const Scaling& rhs) const;
    bool operator!=(const Scaling& rhs) const
    {
        return !(*this == rhs);
    }
};

struct Orientation
{
    //! Describes the shadow direction relative to the pixels in the file.
    ShadowDirection shadowDirection;
    bool operator==(const Orientation& rhs) const
    {
        return shadowDirection == rhs.shadowDirection;
    }
    bool operator!=(const Orientation& rhs) const
    {
        return !(*this == rhs);
    }
};

struct SharpnessEnhancement
{
    // Must include exactly one of modularTransferFunctionCompensation or
    // modularTransferFunctionEnhancement
    mem::ScopedCopyablePtr<Filter> modularTransferFunctionCompensation;
    mem::ScopedCopyablePtr<Filter> modularTransferFunctionEnhancement;

    bool operator==(const SharpnessEnhancement& rhs) const;
    bool operator!=(const SharpnessEnhancement& rhs) const
    {
        return !(*this == rhs);
    }
};

struct ColorManagementModule
{
    RenderingIntent renderingIntent;

    std::string sourceProfile;
    std::string displayProfile;
    std::string iccProfile;

    bool operator==(const ColorManagementModule& rhs) const;
    bool operator!=(const ColorManagementModule& rhs) const;
};

struct ColorSpaceTransform
{
    ColorManagementModule colorManagementModule;
    bool operator==(const ColorSpaceTransform& rhs) const
    {
        return (colorManagementModule == rhs.colorManagementModule);
    }
    bool operator!=(const ColorSpaceTransform& rhs) const
    {
        return !(*this == rhs);
    }
};

struct GeometricTransform
{
    Scaling scaling;
    Orientation orientation;

    bool operator==(const GeometricTransform& rhs) const;
    bool operator!=(const GeometricTransform& rhs) const
    {
        return !(*this == rhs);
    }
};

struct DynamicRangeAdjustment
{
    struct DRAParameters
    {
        DRAParameters();

        double pMin; //! DRA clip low point
        double pMax; //! DRA clip high point

        double eMinModifier; //! eMin modifier
        double eMaxModifier; //! eMax modifier

        bool operator==(const DRAParameters& rhs) const;
        bool operator!=(const DRAParameters& rhs) const
        {
            return !(*this == rhs);
        }
    };

    struct DRAOverrides
    {
        DRAOverrides();

        double subtractor; //! Subtractor value used to reduce haze in the image
        double multiplier; //! Multiplier value used to brighten the image data

        bool operator==(const DRAOverrides& rhs) const;
        bool operator!=(const DRAOverrides& rhs) const
        {
            return !(*this == rhs);
        }
    };

    DRAType algorithmType; //! Algorithm used for dynamic range adjustment
    size_t bandStatsSource; //! Indicates which band to use for DRA stats

    // In SIDD 1.0, must include exactly one of draParameters or draOverrides
    // In SIDD 2.0, include draParameters if algorithmType == AUTO,
    //              may include draOverrides unless algorithmType == None
    mem::ScopedCopyablePtr<DRAParameters> draParameters;
    mem::ScopedCopyablePtr<DRAOverrides> draOverrides;
    bool operator==(const DynamicRangeAdjustment& rhs) const;
    bool operator!=(const DynamicRangeAdjustment& rhs)
    {
        return !(*this == rhs);
    }
};

struct InteractiveProcessing
{
    GeometricTransform geometricTransform;
    SharpnessEnhancement sharpnessEnhancement;
    mem::ScopedCopyablePtr<ColorSpaceTransform> colorSpaceTransform;
    DynamicRangeAdjustment dynamicRangeAdjustment;
    mem::ScopedCopyablePtr<LookupTable> tonalTransferCurve;

    bool operator==(const InteractiveProcessing& rhs) const;
    bool operator!=(const InteractiveProcessing& rhs) const
    {
        return !(*this == rhs);
    }
};

/*
 *  The Display grouping contains information required for
 *  proper display of the imagery. The parameters in this block
 *  are expected to be utilized in conjunction with a viewer compliant
 *  with the NGA Softcopy Image Processing Standard (SIPS) v2.1. In
 *  addition, it also describes any remaps or monitor compensations
 *  applied to the data, as well as, differentiating whether a color
 *  remap or monochrome remap was applied to the data.
 */
struct Display
{
    Display() = default;

    /*!
     *  Defines the pixel type, based on enumeration and definition in
     *  D&E
     */
    PixelType pixelType = PixelType::NOT_SET;

    // Beginning of SIDD 1.0-only section

    /*!
     *  (Optional) Information regarding the encoding of the pixel data.
     *  Used for 8-bit pixel types.
     */
    mem::ScopedCloneablePtr<Remap> remapInformation;

    /*!
     *  (Optional) Recommended ELT magnification method for this data.
     */
    MagnificationMethod magnificationMethod = MagnificationMethod::NOT_SET;

    /*!
     *  (Optional) Recommended ELT decimation method for this data.
     */
    DecimationMethod decimationMethod = DecimationMethod::NOT_SET;

    /*!
     * (Optional) Recommended ELT DRA overrides.
     */
    mem::ScopedCopyablePtr<DRAHistogramOverrides> histogramOverrides;

    /*!
     * (Optional) Describes monitor compensation that may have been applied to
     * the product during processing.
     */
    mem::ScopedCopyablePtr<MonitorCompensationApplied>
            monitorCompensationApplied;

    // End of SIDD 1.0-only section
    // Beginning of SIDD 2.0-only section

    //Required
    size_t numBands;
    //Optional
    size_t defaultBandDisplay;

    //Required
    std::vector<mem::ScopedCopyablePtr<NonInteractiveProcessing> >
            nonInteractiveProcessing;
    std::vector<mem::ScopedCopyablePtr<InteractiveProcessing> >
            interactiveProcessing;

    // End of SIDD 2.0-only section

    /*!
     * Extensible parameters used to support profile-specific needs related to
     * product display.
     */
    ParameterCollection displayExtensions;

    bool operator==(const Display& rhs) const;
    bool operator!=(const Display& rhs) const
    {
        return !(*this == rhs);
    }
};
}
}

#endif
