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

#ifndef __NITF_BANDINFO_HPP__
#define __NITF_BANDINFO_HPP__
#pragma once

#include <string>

#include "nitf/BandInfo.h"
#include "nitf/System.hpp"
#include "nitf/LookupTable.hpp"
#include "nitf/Field.hpp"
#include "nitf/Object.hpp"
#include "nitf/Property.hpp"

/*!
 *  \file BandInfo.hpp
 *  \brief  Contains wrapper implementation for BandInfo
 */

namespace nitf
{

    class Representation final
    {
        std::string representation_;
        Representation(const std::string& r) : representation_(r) {}

    public:
        const std::string& string() const noexcept {
            return representation_;
        }
        static const Representation R;
        static const Representation G;
        static const Representation B;
        static const Representation M;
        static const Representation LU;
        static const Representation& get(const std::string&) noexcept(false);
    };

/*!
 *  \class BandInfo
 *  \brief  The C++ wrapper for the nitf_BandInfo
 */
DECLARE_CLASS(BandInfo)
{
public:

    //! Copy constructor
    BandInfo(const BandInfo & x);

    //! Assignment Operator
    BandInfo & operator=(const BandInfo & x);

    //! Set native object
    BandInfo(nitf_BandInfo * x);

    //! Constructor
    BandInfo() noexcept(false);

    ~BandInfo() = default;

    //! Get the representation
    nitf::Field getRepresentation() const;
    nitf::Property<Representation> representation{
        [&]() ->Representation { return Representation::get(getRepresentation()); },
        [&](const Representation& v) -> void {  getRepresentation().set(v.string()); }
    };

    //! Get the subcategory
    nitf::Field getSubcategory() const;
    nitf::PropertyGet<std::string> subcategory{ [&]() -> std::string { return getSubcategory(); } };

    //! Get the imageFilterCondition
    nitf::Field getImageFilterCondition() const;

    //! Get the imageFilterCode
    nitf::Field getImageFilterCode() const;

    //! Get the numLUTs
    nitf::Field getNumLUTs() const;

    //! Get the bandEntriesPerLUT
    nitf::Field getBandEntriesPerLUT() const;

    //! Get the LookUpTable associated with this BandInfo
    nitf::LookupTable getLookupTable() const;

    /*!
     * Initialize the BandInfo with the given data
     *
     * \param representation        The band representation
     * \param subcategory           The band subcategory
     * \param imageFilterCondition  The band filter condition
     * \param imageFilterCode       The band standard image filter code
     * \param numLUTs               The number of look-up tables
     * \param bandEntriesPerLUT     The number of entries/LUT
     * \param lut                   The look-up tables
     */
    void init(const std::string& representation,
              const std::string& subcategory,
              const std::string& imageFilterCondition,
              const std::string& imageFilterCode,
              uint32_t numLUTs,
              uint32_t bandEntriesPerLUT,
              nitf::LookupTable& lut);
    void init(const Representation&,
              const std::string& subcategory,
              const std::string& imageFilterCondition,
              const std::string& imageFilterCode,
              uint32_t numLUTs,
              uint32_t bandEntriesPerLUT,
              nitf::LookupTable& lut);

    /*!
     * Initialize the BandInfo with the given data. This assumes there is no
     * LUT data.
     *
     * \param representation        The band representation
     * \param subcategory           The band subcategory
     * \param imageFilterCondition  The band filter condition
     * \param imageFilterCode       The band standard image filter code
     */
    void init(const std::string& representation,
              const std::string& subcategory,
              const std::string& imageFilterCondition,
              const std::string& imageFilterCode);
    void init(const Representation&,
              const std::string& subcategory,
              const std::string& imageFilterCondition,
              const std::string& imageFilterCode);

private:
    mutable nitf_Error error{};
};

}
#endif
