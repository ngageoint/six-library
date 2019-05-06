/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_CLASSIFICATION_H__
#define __SIX_CLASSIFICATION_H__

#include "logging/Logger.h"
#include "nitf/FileSecurity.h"
#include "six/Types.h"
#include "six/Options.h"

namespace six
{
/*!
 *  \class Classification
 *  \brief Security record block
 *
 *  This structure is used by both the DerivedData and ComplexData objects
 *  to store security information.  Only the SIDD requires this block
 *  to be populated in the XML data, however, we also use it to write
 *  container specific information using the free parameters.
 *  
 *  SICD writers should be able to create a reasonable classification block
 *  in the NITF by simply populating the base parameters in this structure,
 *  however, occasionally, the developer may need to match a spec. that
 *  diverges from the stock SIDD population requirements.  In that case,
 *  users can also get the free parameters to set NITF fields
 *  directly.  The free parameters are applied last, by the file format
 *  container.  Currently only NITF free parameters are supported.
 */
class Classification
{
public:
    virtual ~Classification()
    {
    }

    // SICD spells this out, SIDD does not
    virtual std::string getLevel() const = 0;

    bool isUnclassified() const
    {
        const std::string level(getLevel());
        return (!level.empty() && ::toupper(level[0]) == 'U');
    }

    virtual void setSecurity(const std::string& /*prefix*/,
                             logging::Logger& /*log*/,
                             nitf::FileSecurity /*security*/) const
    {
    }

    // Print any fields specific to the derived class
    // operator<< below will print the fileOptions
    virtual std::ostream& put(std::ostream& os) const = 0;

    //! Use this to fit in anything you might need for the file container
    Options fileOptions;

    friend bool operator==(const Classification& lhs, const Classification& rhs)
    {
        return lhs.equalTo(rhs);
    }
    friend bool operator!=(const Classification& lhs, const Classification& rhs)
    {
        return !(lhs == rhs);
    }

private:
    virtual bool equalTo(const Classification& rhs) const = 0;
};
}

//! Print operator (utility only).
std::ostream& operator<<(std::ostream& os, const six::Classification& c);

#endif


