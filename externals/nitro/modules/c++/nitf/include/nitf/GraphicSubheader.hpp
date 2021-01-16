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

#ifndef __NITF_GRAPHICSUBHEADER_HPP__
#define __NITF_GRAPHICSUBHEADER_HPP__

#include "nitf/GraphicSubheader.h"
#include "nitf/Object.hpp"
#include "nitf/Field.hpp"
#include "nitf/FileSecurity.hpp"
#include "nitf/Extensions.hpp"
#include <string>

/*!
 *  \file GraphicSubheader.hpp
 *  \brief  Contains wrapper implementation for GraphicSubheader
 */

namespace nitf
{

/*!
 *  \class GraphicSubheader
 *  \brief  The C++ wrapper for the nitf_GraphicSubheader
 */
DECLARE_CLASS(GraphicSubheader)
{
public:
    //! Copy constructor
    GraphicSubheader(const GraphicSubheader & x);

    //! Assignment Operator
    GraphicSubheader & operator=(const GraphicSubheader & x);

    //! Set native object
    GraphicSubheader(nitf_GraphicSubheader * x);

    //! Default Constructor
    GraphicSubheader();

    //! Clone
    nitf::GraphicSubheader clone() const;

    ~GraphicSubheader() = default;

    //! Get the filePartType
    nitf::Field getFilePartType() const;

    //! Get the graphicID
    nitf::Field getGraphicID() const;

    //! Get the name
    nitf::Field getName() const;

    //! Get the securityClass
    nitf::Field getSecurityClass() const;

    //! Get the securityGroup
    nitf::FileSecurity getSecurityGroup() const;

    //! Set the securityGroup
    void setSecurityGroup(nitf::FileSecurity value);

    //! Get the encrypted
    nitf::Field getEncrypted() const;

    //! Get the stype
    nitf::Field getStype() const;

    //! Get the res1
    nitf::Field getRes1() const;

    //! Get the displayLevel
    nitf::Field getDisplayLevel() const;

    //! Get the attachmentLevel
    nitf::Field getAttachmentLevel() const;

    //! Get the location
    nitf::Field getLocation() const;

    //! Get the bound1Loc
    nitf::Field getBound1Loc() const;

    //! Get the color
    nitf::Field getColor() const;

    //! Get the bound2Loc
    nitf::Field getBound2Loc() const;

    //! Get the res2
    nitf::Field getRes2() const;

    //! Get the extendedHeaderLength
    nitf::Field getExtendedHeaderLength() const;

    //! Get the extendedHeaderOverflow
    nitf::Field getExtendedHeaderOverflow() const;

    //! Get the extendedSection
    nitf::Extensions getExtendedSection() const;

    //! Set the extendedSection
    void setExtendedSection(nitf::Extensions value);

private:
    mutable nitf_Error error{};
};

}
#endif
