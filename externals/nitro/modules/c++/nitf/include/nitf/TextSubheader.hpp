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

#ifndef __NITF_TEXTSUBHEADER_HPP__
#define __NITF_TEXTSUBHEADER_HPP__
#pragma once

#include <string>

#include "nitf/TextSubheader.h"
#include "nitf/NITFException.hpp"
#include "nitf/Object.hpp"
#include "nitf/FileSecurity.hpp"
#include "nitf/Extensions.hpp"
#include "nitf/Property.hpp"

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
    TextSubheader() noexcept(false);

    //! Clone
    nitf::TextSubheader clone() const;

    ~TextSubheader() = default;

    //! Get the filePartType
    nitf::Field getFilePartType() const;
    //PropertyGet<std::string> filePartType{ [&]() -> std::string { return getFilePartType(); } };

    //! Get the textID
    nitf::Field getTextID() const;

    //! Get the attachmentLevel
    nitf::Field getAttachmentLevel() const;

    //! Get the dateTime
    nitf::Field getDateTime() const;

    //! Get the title
    nitf::Field getTitle() const;

    //! Get the securityClass
    nitf::Field getSecurityClass() const;

    //! Get the securityGroup
    nitf::FileSecurity getSecurityGroup() const;

    //! Set the securityGroup
    void setSecurityGroup(nitf::FileSecurity value);

    //! Get the encrypted
    nitf::Field getEncrypted() const;

    //! Get the format
    nitf::Field getFormat() const;

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
