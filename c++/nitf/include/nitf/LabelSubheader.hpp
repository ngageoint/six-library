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
    LabelSubheader() throw(nitf::NITFException);

    //! Clone
    nitf::LabelSubheader clone() throw(nitf::NITFException);

    ~LabelSubheader();

    //! Get the filePartType
    nitf::Field getFilePartType();

    //! Get the labelID
    nitf::Field getLabelID();

    //! Get the securityClass
    nitf::Field getSecurityClass();

    //! Get the securityGroup
    nitf::FileSecurity getSecurityGroup();

    //! Set the securityGroup
    void setSecurityGroup(nitf::FileSecurity value);

    //! Get the encrypted
    nitf::Field getEncrypted();

    //! Get the fontStyle
    nitf::Field getFontStyle();

    //! Get the cellWidth
    nitf::Field getCellWidth();

    //! Get the cellHeight
    nitf::Field getCellHeight();

    //! Get the displayLevel
    nitf::Field getDisplayLevel();

    //! Get the attachmentLevel
    nitf::Field getAttachmentLevel();

    //! Get the locationRow
    nitf::Field getLocationRow();

    //! Get the locationColumn
    nitf::Field getLocationColumn();

    //! Get the textColor
    nitf::Field getTextColor();

    //! Get the backgroundColor
    nitf::Field getBackgroundColor();

    //! Get the extendedHeaderLength
    nitf::Field getExtendedHeaderLength();

    //! Get the extendedHeaderOverflow
    nitf::Field getExtendedHeaderOverflow();

    //! Get the extendedSection
    nitf::Extensions getExtendedSection();

    //! Set the extendedSection
    void setExtendedSection(nitf::Extensions value);

private:
    nitf_Error error;
};

}
#endif
