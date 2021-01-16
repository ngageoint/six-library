/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_RECORD_HPP__
#define __NITF_RECORD_HPP__
#pragma once

#include <string>

#include "nitf/Record.h"
#include "nitf/ComplexityLevel.h"

#include "List.hpp"
#include "FileHeader.hpp"
#include "ImageSegment.hpp"
#include "GraphicSegment.hpp"
#include "TextSegment.hpp"
#include "DESegment.hpp"

/*!
 *  \file Record.hpp
 *  \brief  Contains wrapper implementation for Record
 */

namespace nitf
{

/*!
 *  \class Record
 *  \brief  The C++ wrapper for the nitf_Record
 *
 *  The Record is a representation of the data within
 *  a file.  It is structurally hierarchical in a manner
 *  that corresponds one-to-one with the components of a NITF
 *  file.
 */
DECLARE_CLASS(Record)
{
public:
    //! Copy constructor
    Record(const Record & x);

    //! Assignment Operator
    Record & operator=(const Record & x);

    //! Set native object
    Record(nitf_Record * x);

    //! Default Constructor
    Record(nitf::Version version = nitf::Version::NITF_VER_21) noexcept(false);

    //! Clone
    nitf::Record clone() const;

    ~Record() = default;

    //! Returns the NITF version
    nitf::Version getVersion() const;

    //! Get the header
    nitf::FileHeader getHeader() const;
    //! Set the header
    void setHeader(nitf::FileHeader & value);

    uint32_t getNumImages() const;
    uint32_t getNumGraphics() const;
    uint32_t getNumLabels() const;
    uint32_t getNumTexts() const;
    uint32_t getNumDataExtensions() const;
    uint32_t getNumReservedExtensions() const;

    //! Get the images
    nitf::List getImages() const;


    //! Get the graphics
    nitf::List getGraphics() const;

    //! Get the labels
    nitf::List getLabels() const;

    //! Get the texts
    nitf::List getTexts() const;

    //! Get the dataExtensions
    nitf::List getDataExtensions() const;

    //! Get the reservedExtensions
    nitf::List getReservedExtensions() const;

    //! Make and return a new ImageSegment
    nitf::ImageSegment newImageSegment(int index = -1);

    //! Make and return a new GraphicSegment
    nitf::GraphicSegment newGraphicSegment(int index = -1);

    //! Make and return a new TextSegment
    nitf::TextSegment newTextSegment(int index = -1);

    //! Make and return a new DESegment
    nitf::DESegment newDataExtensionSegment(int index = -1);

    //! Remove the image segment at the given index
    void removeImageSegment(uint32_t index);

    //! Remove the graphic segment at the given index
    void removeGraphicSegment(uint32_t index);

    //! Remove the text segment at the given index
    void removeTextSegment(uint32_t index);

    //! Remove the label segment at the given index
    void removeLabelSegment(uint32_t index);

    //! Remove the DE segment at the given index
    void removeDataExtensionSegment(uint32_t index);

    //! Remove the RE segment at the given index
    void removeReservedExtensionSegment(uint32_t index);

    //! Move the image segment from the oldIndex to the newIndex
    void moveImageSegment(uint32_t oldIndex, int newIndex = -1);

    //! Move the text segment from the oldIndex to the newIndex
    void moveTextSegment(uint32_t oldIndex, int newIndex = -1);

    //! Move the graphic segment from the oldIndex to the newIndex
    void moveGraphicSegment(uint32_t oldIndex, int newIndex = -1);

    //! Move the label segment from the oldIndex to the newIndex
    void moveLabelSegment(uint32_t oldIndex, int newIndex = -1);

    //! Move the DE segment from the oldIndex to the newIndex
    void moveDataExtensionSegment(uint32_t oldIndex, int newIndex = -1);

    //! Move the RE segment from the oldIndex to the newIndex
    void moveReservedExtensionSegment(uint32_t oldIndex, int newIndex = -1);

    /*!
     * If the CLEVEL field in this object is not set, computes the complexity
     * level of the NITF based on the Record's contents and sets it.
     *
     * \note Normally you don't have to call this method directly as the
     * underlying Writer object will compute this before writing out the NITF.
     * This is only necessary if you are using NITRO to populate the headers
     * but then using something else to do the actual writing.
     */
    void setComplexityLevelIfUnset();

private:
    mutable nitf_Error error{};
};

}

//#ifndef SWIGPYTHON
//// These are already global via C-style enum for SWIG
//constexpr auto NITF_CLEVEL_UNKNOWN = complianceLevel::NITF_CLEVEL_UNKNOWN;
//constexpr auto NITF_CLEVEL_03 = complianceLevel::NITF_CLEVEL_03;
//constexpr auto NITF_CLEVEL_05 = complianceLevel::NITF_CLEVEL_05;
//constexpr auto NITF_CLEVEL_06 = complianceLevel::NITF_CLEVEL_06;
//constexpr auto NITF_CLEVEL_07 = complianceLevel::NITF_CLEVEL_07;
//constexpr auto NITF_CLEVEL_09 = complianceLevel::NITF_CLEVEL_09;
//constexpr auto NITF_CLEVEL_CHECK_FAILED = complianceLevel::NITF_CLEVEL_CHECK_FAILED;
//#endif

#endif
