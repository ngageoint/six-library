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

#include <mem/ScopedCopyablePtr.h>
#include <mem/ScopedCloneablePtr.h>
#include <six/Types.h>
#include <six/Init.h>
#include <six/Parameter.h>
#include <six/ParameterCollection.h>
#include <six/sidd/Enums.h>
#include <six/sidd/Kernel.h>

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
 *  This class does not auto-create any LUT, it defaults to NULL.
 *  Neither do the derived classes.
 */
struct Remap
{
    Remap(LUT* lut = NULL) :
        remapLUT(lut)
    {
    }

    virtual ~Remap()
    {
    }

    DisplayType displayType;
    mem::ScopedCloneablePtr<LUT> remapLUT; // MonoLUT or ColorLUT or NULL

    virtual Remap* clone() const = 0;
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
    MonochromeDisplayRemap(std::string _remapType, LUT* lut = NULL) :
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
};

/*!
 *  \struct ColorDisplayRemap
 *  \brief Color remap object, leaves remap as NULL
 *
 *  A color display remap contains clipping information, and possibly
 *  also contains a remap LUT.
 */
struct ColorDisplayRemap : public Remap
{
    //!  Constructor
    ColorDisplayRemap(LUT* lut = NULL) :
        Remap(lut)
    {
        this->displayType = DisplayType::COLOR;
    }

    //!  Clone the remap
    virtual Remap* clone() const
    {
        return new ColorDisplayRemap(*this);
    }
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
    //! Constructor
    MonitorCompensationApplied();

    double gamma;
    double xMin;
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
};

struct BandInformation
{
    BandInformation();

    std::vector<std::string> bands;
    size_t bitsPerPixel;
    size_t displayFlag; // Which band to display by default
};

struct BandEqualization
{
    BandEqualizationAlgorithm algorithm;
    mem::ScopedCloneablePtr<LUT> bandLUT;
};

struct RRDS
{
    //! Algorithm used to perform RRDS downsampling
    DownsamplingMethod downsamplingMethod;

    //! Anti-aliasing kernel.  Only include if downsamplingMethod = DECIMATE.
    mem::ScopedCopyablePtr<Kernel> antiAlias;

    //! Interpolation kernel.  Only include if downsamplingMethod = DECIMATE.
    mem::ScopedCopyablePtr<Kernel> interpolation;
};

struct ProductGenerationOptions
{
    mem::ScopedCopyablePtr<BandEqualization> bandEqualization;
    Kernel modularTransferFunctionRestoration;

    mem::ScopedCloneablePtr<Remap> dataRemapping;

    mem::ScopedCopyablePtr<Kernel> asymmetricPixelCorrection;
};

struct NonInteractiveProcessing
{
    ProductGenerationOptions productGenerationOptions;
    RRDS rrds;
};

struct Scaling
{
    Kernel antiAlias;
    Kernel interpolation;
};

struct Orientation
{
    Orientation();

    DerivedOrientationType orientationType;

    /*!
     * In degrees.  Only include if orientationType == ANGLE.  Positive angles
     * are CW and negative angles are CCW.
     */
    double rotationAngle;
};

struct SharpnessEnhancement
{
    mem::ScopedCopyablePtr<Kernel> modularTransferFunctionCompensation;
    mem::ScopedCopyablePtr<Kernel> modularTransferFunctionRestoration;
};

struct ColorManagementModule
{
    RenderingIntent renderingIntent;

    // TODO: Just assume these are strings for now... this will likely change
    std::string sourceProfile;
    std::string displayProfile;
    std::string iccProfile;
};

struct ColorSpaceTransform
{
    ColorManagementModule colorManagementModule;
};

struct GeometricTransform
{
    Scaling scaling;
    Orientation orientation;
};

struct DynamicRangeAdjustment
{
    struct Modifiers
    {
        Modifiers();

        double eMin; //! eMin modifier
        double eMax; //! eMax modifier
        double subtractor; //! Subtractor value used to reduce haze in the image
        double multiplier; //! Multiplier value used to brighten the image data
    };

    DynamicRangeAdjustment();

    DRAType algorithmType; //! Algorithm used for dynamic range adjustment
    double pMin; //! DRA clip low point
    double pMax; //! DRA clip high point

    Modifiers modifiers;
};

struct OneDimensionalLookup
{
    Kernel ttc;
};

struct InteractiveProcessing
{
    GeometricTransform geometricTransform;
    SharpnessEnhancement sharpnessEnhancement;
    mem::ScopedCopyablePtr<ColorSpaceTransform> colorSpaceTransform;
    mem::ScopedCopyablePtr<DynamicRangeAdjustment> dynamicRangeAdjustment;
    mem::ScopedCopyablePtr<OneDimensionalLookup> oneDimensionalLookup;
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
    Display();

    /*!
     *  Defines the pixel type, based on enumeration and definition in
     *  D&E
     */
    PixelType pixelType;

    // Beginning of SIDD 1.0-only section

    /*!
     *  (Optional) Information regarding the encoding of the pixel data.
     *  Used for 8-bit pixel types.
     */
    mem::ScopedCloneablePtr<Remap> remapInformation;

    /*!
     *  (Optional) Recommended ELT magnification method for this data.
     */
    MagnificationMethod magnificationMethod;

    /*!
     *  (Optional) Recommended ELT decimation method for this data.
     */
    DecimationMethod decimationMethod;

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
    // Beginning of SIDD 1.1-only section

    mem::ScopedCopyablePtr<BandInformation> bandInformation;
    mem::ScopedCopyablePtr<NonInteractiveProcessing> nonInteractiveProcessing;
    mem::ScopedCopyablePtr<InteractiveProcessing> interactiveProcessing;

    // End of SIDD 1.1-only section

    /*!
     * Extensible parameters used to support profile-specific needs related to
     * product display.
     */
    ParameterCollection displayExtensions;
};
}
}

#endif
