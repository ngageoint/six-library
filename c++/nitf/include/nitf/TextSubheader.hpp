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

#ifndef __NITF_TEXTSUBHEADER_HPP__
#define __NITF_TEXTSUBHEADER_HPP__

#include "nitf/TextSubheader.h"
#include "nitf/NITFException.hpp"
#include "nitf/Object.hpp"
#include "nitf/FileSecurity.hpp"
#include "nitf/Extensions.hpp"
#include <string>

/*!
 *  \file TextSubheader.hpp
 *  \brief  Contains wrapper implementation for TextSubheader
 */

namespace nitf
{

/*!
 *  \class TextSubheader
 *  \brief  The C++ wrapper for the nitf_TextSubheader
 */
DECLARE_CLASS(TextSubheader)
{
public:

    //! Copy constructor
    TextSubheader(const TextSubheader & x);

    //! Assignment Operator
    TextSubheader & operator=(const TextSubheader & x);

    //! Set native object
    TextSubheader(nitf_TextSubheader * x);

    //! Default Constructor
    TextSubheader() throw(nitf::NITFException);

    //! Clone
    nitf::TextSubheader clone() throw(nitf::NITFException);

    ~TextSubheader();

    //! Get the filePartType
    nitf::Field getFilePartType();

    //! Get the textID
    nitf::Field getTextID();

    //! Get the attachmentLevel
    nitf::Field getAttachmentLevel();

    //! Get the dateTime
    nitf::Field getDateTime();

    //! Get the title
    nitf::Field getTitle();

    //! Get the securityClass
    nitf::Field getSecurityClass();

    //! Get the securityGroup
    nitf::FileSecurity getSecurityGroup();

    //! Set the securityGroup
    void setSecurityGroup(nitf::FileSecurity value);

    //! Get the encrypted
    nitf::Field getEncrypted();

    //! Get the format
    nitf::Field getFormat();

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
