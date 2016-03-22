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

#include <import/mem.h>

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"
#include "six/ParameterCollection.h"

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

    friend bool operator==(const Remap& lhs, const Remap& rhs)
    {
        return lhs.equalTo(rhs);
    }

    friend bool operator!=(const Remap& lhs, const Remap& rhs)
    {
        return !(lhs == rhs);
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

    virtual bool equalTo(const Remap& rhs) const;

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

    virtual bool equalTo(const Remap& rhs) const;
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

    bool operator==(const Display& rhs) const;
    bool operator!=(const Display& rhs) const
    {
        return !(*this == rhs);
    }
};

}
}
#endif

