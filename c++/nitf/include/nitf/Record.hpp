/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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

#include "nitf/Record.h"
#include "nitf/System.hpp"
#include "nitf/List.hpp"
#include "nitf/FileHeader.hpp"
#include "nitf/ImageSegment.hpp"
#include "nitf/GraphicSegment.hpp"
#include "nitf/LabelSegment.hpp"
#include "nitf/TextSegment.hpp"
#include "nitf/DESegment.hpp"
#include "nitf/RESegment.hpp"
#include "nitf/Object.hpp"
#include <string>

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
    Record(nitf::Version version = NITF_VER_21) throw(nitf::NITFException);

    //! Clone
    nitf::Record clone() throw(nitf::NITFException);

    ~Record();

    //! Returns the NITF version
    nitf::Version getVersion();

    //! Get the header
    nitf::FileHeader getHeader();
    //! Set the header
    void setHeader(nitf::FileHeader & value);

    nitf::Uint32 getNumImages();
    nitf::Uint32 getNumGraphics();
    nitf::Uint32 getNumLabels();
    nitf::Uint32 getNumTexts();
    nitf::Uint32 getNumDataExtensions();
    nitf::Uint32 getNumReservedExtensions();

    //! Get the images
    nitf::List getImages();


    //! Get the graphics
    nitf::List getGraphics();

    //! Get the labels
    nitf::List getLabels();

    //! Get the texts
    nitf::List getTexts();

    //! Get the dataExtensions
    nitf::List getDataExtensions();

    //! Get the reservedExtensions
    nitf::List getReservedExtensions();

    //! Make and return a new ImageSegment
    nitf::ImageSegment newImageSegment(int index = -1);

    //! Make and return a new GraphicSegment
    nitf::GraphicSegment newGraphicSegment(int index = -1);

    //! Make and return a new TextSegment
    nitf::TextSegment newTextSegment(int index = -1);

    //! Make and return a new DESegment
    nitf::DESegment newDataExtensionSegment(int index = -1);

    //! Remove the image segment at the given index
    void removeImageSegment(nitf::Uint32 index);

    //! Remove the graphic segment at the given index
    void removeGraphicSegment(nitf::Uint32 index);

    //! Remove the text segment at the given index
    void removeTextSegment(nitf::Uint32 index);

    //! Remove the label segment at the given index
    void removeLabelSegment(nitf::Uint32 index);

    //! Remove the DE segment at the given index
    void removeDataExtensionSegment(nitf::Uint32 index);

    //! Remove the RE segment at the given index
    void removeReservedExtensionSegment(nitf::Uint32 index);

    //! Move the image segment from the oldIndex to the newIndex
    void moveImageSegment(nitf::Uint32 oldIndex, int newIndex = -1);

    //! Move the text segment from the oldIndex to the newIndex
    void moveTextSegment(nitf::Uint32 oldIndex, int newIndex = -1);

    //! Move the graphic segment from the oldIndex to the newIndex
    void moveGraphicSegment(nitf::Uint32 oldIndex, int newIndex = -1);

    //! Move the label segment from the oldIndex to the newIndex
    void moveLabelSegment(nitf::Uint32 oldIndex, int newIndex = -1);

    //! Move the DE segment from the oldIndex to the newIndex
    void moveDataExtensionSegment(nitf::Uint32 oldIndex, int newIndex = -1);

    //! Move the RE segment from the oldIndex to the newIndex
    void moveReservedExtensionSegment(nitf::Uint32 oldIndex, int newIndex = -1);

private:
    nitf_Error error;
};

}
#endif
