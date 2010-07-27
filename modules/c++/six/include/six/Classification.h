/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#include "six/Types.h"
#include "six/Options.h"

namespace six
{

/*!
 *  \struct ClassificationGuidance
 *  \brief Represents SIDD classification guidance block
 *
 *  This struct attempts to populate both SICD and SIDD guidance
 *  information.  SICD does not actually have such a structure,
 *  but its components are also used to initialize the SICD container
 */
struct ClassificationGuidance
{
    ClassificationGuidance()
    {
    }

    ~ClassificationGuidance()
    {
    }

    ClassificationGuidance* clone() const
    {
        ClassificationGuidance* cg = new ClassificationGuidance();
        cg->date = date;
        cg->authority = authority;
        return cg;
    }

    DateTime date;
    std::string authority;

};

/*!
 *  \struct Classification
 *  \brief Security record block
 *
 *  This structure is used by both the DerivedData and ComplexData objects
 *  to store security information.  Only the SIDD requires this block
 *  to be populated in the XML data, however, we also use it to write
 *  container specific information using the free parameters.
 *  
 *  SICD writers should be able to create a resonable classification block
 *  in the NITF by simply populating the base parameters in this structure,
 *  however, occasionally, the developer may need to match a spec. that
 *  diverges from the stock SIDD population requirements.  In that case,
 *  users can also get the free parameters to set NITF fields
 *  directly.  The free parameters are applied last, by the file format
 *  container.  Currently only NITF free parameters are supported.
 */
struct Classification
{
    /*!
     *  Constructor.  Takes an optional ClassificationGuidance argument.
     *
     *  \param classGuidance Classification guidance information
     */
    Classification(ClassificationGuidance* classGuidance = NULL) :
        guidance(classGuidance)
    {
    }

    //!  Destructor
    ~Classification();

    //!  Copy constructor.  
    //!  TODO: This should probably be a clone() instead
    Classification(const Classification& c);

    //!  Assignment operator
    Classification& operator=(const Classification& c);

    //!  This lets you override the subheader classification
    static const char OPT_ISCLAS[];
    //!  File security classification system
    static const char OPT_ISCLSY[];
    //!  File classification text
    static const char OPT_ISCLTX[];
    //!  File security codewors
    static const char OPT_ISCODE[];
    //!  File security reason 
    static const char OPT_ISCRSN[];
    //!  File control and handling
    static const char OPT_ISCTLH[];
    //!  File security control number
    static const char OPT_ISCTLN[];
    //!  File security declassification date
    static const char OPT_ISDCDT[];
    //!  File security declassification type
    static const char OPT_ISDCTP[];
    //!  File security declassification exemption
    static const char OPT_ISDCXM[];
    //!  File security downgrade
    static const char OPT_ISDG[];
    //!  File security downgrade date
    static const char OPT_ISDGDT[];
    //!  File releasing instructions
    static const char OPT_ISREL[];
    //!  File security source date
    static const char OPT_ISSRDT[];

    //!  This lets you override the subheader classification
    static const char OPT_DESCLAS[];
    //!  File security classification system
    static const char OPT_DESCLSY[];
    //!  File classification text
    static const char OPT_DESCLTX[];
    //!  File security codewors
    static const char OPT_DESCODE[];
    //!  File security reason 
    static const char OPT_DESCRSN[];
    //!  File control and handling
    static const char OPT_DESCTLH[];
    //!  File security control number
    static const char OPT_DESCTLN[];
    //!  File security declassification date
    static const char OPT_DESDCDT[];
    //!  File security declassification type
    static const char OPT_DESDCTP[];
    //!  File security declassification exemption
    static const char OPT_DESDCXM[];
    //!  File security downgrade
    static const char OPT_DESDG[];
    //!  File security downgrade date
    static const char OPT_DESDGDT[];
    //!  File releasing instructions
    static const char OPT_DESREL[];
    //!  File security source date
    static const char OPT_DESSRDT[];

    //!  Within both SICD and SIDD, populate the actual class level
    std::string level;

    //! Any security markings  
    std::vector<std::string> securityMarkings;

    //! Use this to fit in anything you might need for the file container
    Options fileOptions;

    //! ClassificationGuidance object
    ClassificationGuidance* guidance;
};

}

//! Print operator (utility only).
std::ostream& operator<<(std::ostream& os, const six::Classification& c);

#endif

