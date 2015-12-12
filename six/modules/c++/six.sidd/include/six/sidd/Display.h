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
    virtual ~MonochromeDisplayRemap()
    {
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
    virtual ~ColorDisplayRemap()
    {
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
    double gamma;
    double xMin;
    //! Constructor
    MonitorCompensationApplied()
    {
        gamma = Init::undefined<double>();
        xMin = Init::undefined<double>();
    }
    //! Copy this
    MonitorCompensationApplied* clone() const
    {
        return new MonitorCompensationApplied(*this);
    }
};

/*!
 *  \struct DRAHistogramOverrides
 *  \brief 
 *
 */
struct DRAHistogramOverrides
{
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

    DRAHistogramOverrides()
    {
        clipMin = Init::undefined<int>();
        clipMax = Init::undefined<int>();
    }

    DRAHistogramOverrides* clone() const
    {
        return new DRAHistogramOverrides(*this);
    }
};

struct Kernel
{
    struct Predefined
    {
        Predefined() :
            kernelFamily(six::Init::undefined<size_t>()),
            kernelMember(six::Init::undefined<size_t>())
        {
        }

        // Exactly one of dbName or kernelFamiy+kernelMember must be set

        //! Database name of filter to use
        KernelDatabaseName dbName;

        //! Index specifying the kernel family
        size_t kernelFamily;

        //! Index specifying the member for the kernel family
        size_t kernelMember;
    };

    struct Custom
    {
        /*! General indicates the same kernel is used for the entire operation
            Filterbank indicates that the kernel is spatially variant
         */
        KernelCustomType type;

        //! Size of the kernel
        RowColInt kernelSize;

        /*! For KernelType=General, populate with the coefficients for the
            kernelSize kernel.  For KernelType=FilterBank, each row should
            contain the coefficients for that phasing.
         */
        std::vector<double> kernelCoef;
    };

    //! Name of the filter
    std::string kernelName;

    // Exactly one of Predefined or Custom
    mem::ScopedCopyablePtr<Predefined> predefined;
    mem::ScopedCopyablePtr<Custom> custom;

    /*! Specifies if the kernel is to be applied using convolution or
        correlation
     */
    KernelOperation operation;
};

struct BandInformation
{
    std::vector<std::string> bands;
    size_t bitsPerPixel;
    size_t displayFlag; // Which band to display by default
};

struct BandEqualization
{
    std::string algorithm; // TODO: This is supposed to be an enum??
    mem::ScopedCloneablePtr<LUT> bandLUT;
};

struct RRDS
{
    //! Algorithm used to perform RRDS downsampling
    DownsamplingMethod downsamplingMethod;

    //! Anti-aliasing kernel.  Only include if downsamplingMethod = DECIMATE.
    mem::ScopedCopyablePtr<Kernel> antiAliasing;

    //! Interpolation kernel.  Only include if downsamplingMethod = DECIMATE.
    mem::ScopedCopyablePtr<Kernel> interpolation;
};

struct ProductGenerationOptions
{
    mem::ScopedCopyablePtr<BandEqualization> bandEqualization;
    Kernel modularTransferFunction;

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
    Orientation() :
        rotationAngle(six::Init::undefined<double>())
    {
    }

    DerivedOrientationType orientationType;

    /*!
     * In degrees.  Only include if orientationType == ANGLE.  Positive angles
     * are CW and negative angles are CCW.
     */
    double rotationAngle;
};

struct SharpnessEnhancement
{
    // TODO: This section seems messed up.  Might only be one of these
    Kernel mtfc;
    Kernel mtfe;
};

struct ColorSpaceTransform
{
    // TODO: Needs another look
    RenderingIntent renderingIntent;
    std::string sourceProfile;
    std::string displayProfile;
    std::string iccProfile;
};

struct GeometricTransform
{
    Scaling scaling;
    Orientation orientation;
};

struct DynamicRangeAdjustment
{
    DynamicRangeAdjustment() :
        pMin(six::Init::undefined<double>()),
        pMax(six::Init::undefined<double>()),
        a(six::Init::undefined<double>()),
        b(six::Init::undefined<double>()),
        subtractor(six::Init::undefined<double>()),
        multiplier(six::Init::undefined<double>())
    {
    }

    DRAType algorithmType; //! Algorithm used for dynamic range adjustment
    double pMin; //! DRA clip low point
    double pMax; //! DRA clip high point

    double a; //! eMin modifier
    double b; //! eMax modifier
    double subtractor; //! Subtractor value used to reduce haze in the image
    double multiplier; //! Multiplier value used to brighten the image data
};

struct InteractiveProcessing
{
    GeometricTransform geometricTransform;
    SharpnessEnhancement sharpnessEnhancement;
    ColorSpaceTransform colorSpaceTransform;
    DynamicRangeAdjustment dynamicRangeAdjustment;
    mem::ScopedCopyablePtr<Kernel> TTC;
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

    virtual ~Display()
    {
    }

    /*!
     *  Contract requires Display to clone itself properly, irrespective
     *  of whether it is color or mono data.
     */
    Display* clone() const;

    /*!
     *  Defines the pixel type, based on enumeration and definition in
     *  D&E
     */
    PixelType pixelType;

    /*!
     *  (Optional) Information regarding the encoding of the pixel data.
     *  Used for 8-bit pixel types.
     */
    mem::ScopedCloneablePtr<Remap> remapInformation;

    /*!
     *  Recommended ELT magnification method for this data.
     */
    MagnificationMethod magnificationMethod;

    /*!
     *  Recommended ELT decimation method for this data.
     */
    DecimationMethod decimationMethod;

    mem::ScopedCloneablePtr<DRAHistogramOverrides> histogramOverrides;

    mem::ScopedCloneablePtr<MonitorCompensationApplied> 
            monitorCompensationApplied;

    ParameterCollection displayExtensions;
};

}
}
#endif

