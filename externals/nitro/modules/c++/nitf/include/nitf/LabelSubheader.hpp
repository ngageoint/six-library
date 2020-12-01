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

#ifndef __NITF_LABELSUBHEADER_HPP__
#define __NITF_LABELSUBHEADER_HPP__

#include "nitf/LabelSubheader.h"
#include "nitf/Object.hpp"
#include "nitf/Field.hpp"
#include "nitf/FileSecurity.hpp"
#include "nitf/Extensions.hpp"
#include <string>

/*!
 *  \file LabelSubheader.hpp
 *  \brief  Contains wrapper implementation for LabelSubheader
 */

namespace nitf
{

/*!
 *  \class LabelSubheader
 *  \brief  The C++ wrapper for the nitf_LabelSubheader
 */
DECLARE_CLASS(LabelSubheader)
{
public:

    //! Copy constructor
    LabelSubheader(const LabelSubheader & x);

    //! Assignment Operator
    LabelSubheader & operator=(const LabelSubheader & x);

    //! Set native object
    LabelSubheader(nitf_LabelSubheader * x);

    //! Default Constructor
    LabelSubheader() noexcept(false);

    //! Clone
    nitf::LabelSubheader clone() const;

    ~LabelSubheader() = default;

    //! Get the filePartType
    nitf::Field getFilePartType() const;

    //! Get the labelID
    nitf::Field getLabelID() const;

    //! Get the securityClass
    nitf::Field getSecurityClass() const;

    //! Get the securityGroup
    nitf::FileSecurity getSecurityGroup() const;

    //! Set the securityGroup
    void setSecurityGroup(nitf::FileSecurity value);

    //! Get the encrypted
    nitf::Field getEncrypted() const;

    //! Get the fontStyle
    nitf::Field getFontStyle() const;

    //! Get the cellWidth
    nitf::Field getCellWidth() const;

    //! Get the cellHeight
    nitf::Field getCellHeight() const;

    //! Get the displayLevel
    nitf::Field getDisplayLevel() const;

    //! Get the attachmentLevel
    nitf::Field getAttachmentLevel() const;

    //! Get the locationRow
    nitf::Field getLocationRow() const;

    //! Get the locationColumn
    nitf::Field getLocationColumn() const;

    //! Get the textColor
    nitf::Field getTextColor() const;

    //! Get the backgroundColor
    nitf::Field getBackgroundColor() const;

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
