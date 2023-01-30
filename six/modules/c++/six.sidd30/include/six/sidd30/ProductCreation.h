/* =========================================================================
 * This file is part of six.sidd30-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sidd30-c++ is free software; you can redistribute it and/or modify
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
#ifndef SIX_six_sicd30_ProductCreation_h_INCLUDED_
#define SIX_six_sicd30_ProductCreation_h_INCLUDED_
#pragma once

#include <memory>

#include "six/Types.h"
#include "six/Init.h"
#include "six/ParameterCollection.h"
#include "six/sidd30/DerivedClassification.h"

namespace six
{
namespace sidd30
{
struct ProcessorInformation
{
    std::string application;
    DateTime processingDateTime;
    std::string site;

    // Optional
    std::string profile;

    //! Equality operators
    bool operator==(const ProcessorInformation& rhs) const;
    bool operator!=(const ProcessorInformation& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct ProductCreation
 *  \brief SIDD ProductCreation parameters
 *
 *  Contains general information about product creation
 */
class ProductCreation
{
public:
    //!  Details regarding processor
    ProcessorInformation processorInformation;

    //!  The overall classification of the product
    DerivedClassification classification;

    //!  The output product name defined by the processor
    std::string productName;

    //!  Class of product.
    std::string productClass;

    /*! 
     *  (Optional) Information on the type of prodcut
     *  Omit if a single product suite
     */
    std::string productType;

    /*!
     *  (Optional, Unbounded) Extensible params used to support
     *  profile-specific needs related to product creation
     */
    ParameterCollection productCreationExtensions;

    //! Equality operators
    bool operator==(const ProductCreation& rhs) const;
    bool operator!=(const ProductCreation& rhs) const
    {
        return !(*this == rhs);
    }
};
}
}
#endif // SIX_six_sicd30_ProductCreation_h_INCLUDED_
