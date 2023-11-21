/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * © Copyright 2023, Maxar Technologies, Inc.
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

#pragma once

#include "TREField.hpp"

 // A sample (and simple) "strongly-typed" TRE; see ENGRDA in the
// unittests for something a bit more elaborate (not included because it's incomplete).
namespace nitf
{
namespace TREs
{
    class TEST_DES final
    {
        nitf::TRE tre_;
        static constexpr const char* tag = "TEST_DES";

    public:
        // from TRE::getID()
        /**
            * Get the TRE identifier. This is NOT the tag, however it may be the
            * same value as the tag. The ID is used to identify a specific
            * version/incarnation of the TRE, if multiple are possible. For most TREs,
            * this value will be the same as the tag.
            */
        TEST_DES(const std::string& id = "") noexcept(false)
            : tre_(tag, id.empty() ? tag : id.c_str()),
            TEST_DES_COUNT(tre_, "TEST_DES_COUNT"),
            TEST_DES_START(tre_, "TEST_DES_START"),
            TEST_DES_INCREMENT(tre_, "TEST_DES_INCREMENT")
        {
        }
        ~TEST_DES() = default;
        TEST_DES(const TEST_DES&) = delete;
        TEST_DES& operator=(const TEST_DES&) = delete;
        TEST_DES(TEST_DES&&) = default;
        TEST_DES& operator=(TEST_DES&&) = delete;

        // From TEST_DES.c
        /*
        static nitf_TREDescription TEST_DES_description[] = {
            {NITF_BCS_N, 2, "Number of data values", "TEST_DES_COUNT" },
            {NITF_BCS_N, 3, "Start value in ramp", "TEST_DES_START" },
            {NITF_BCS_N, 2, "Increment between values in ramp", "TEST_DES_INCREMENT" },
            {NITF_END, 0, NULL, NULL}
        };
        */
        nitf::TREField_BCS_N<2> TEST_DES_COUNT;
        nitf::TREField_BCS_N<3> TEST_DES_START;
        nitf::TREField_BCS_N<2> TEST_DES_INCREMENT;

        void updateFields()
        {
            tre_.updateFields();
        }
    };

    class TEST_PRELOADED_DES final
    {
        nitf::TRE tre_;
        static constexpr const char* tag = "TEST_PRELOADED_DES";

    public:
        // from TRE::getID()
        /**
            * Get the TRE identifier. This is NOT the tag, however it may be the
            * same value as the tag. The ID is used to identify a specific
            * version/incarnation of the TRE, if multiple are possible. For most TREs,
            * this value will be the same as the tag.
            */
        TEST_PRELOADED_DES(const std::string& id = "") noexcept(false) : tre_(tag, id.empty() ? tag : id.c_str()),
            COUNT(tre_, "COUNT"), START(tre_, "START"), INCREMENT(tre_, "INCREMENT") { }
        ~TEST_PRELOADED_DES() = default;
        TEST_PRELOADED_DES(const TEST_PRELOADED_DES&) = delete;
        TEST_PRELOADED_DES& operator=(const TEST_PRELOADED_DES&) = delete;
        TEST_PRELOADED_DES(TEST_PRELOADED_DES&&) = default;
        TEST_PRELOADED_DES& operator=(TEST_PRELOADED_DES&&) = delete;

        // From TREs.c
        /*
        static nitf_TREDescription TEST_PRELOADED_DES_description[] = {
            {NITF_BCS_N, 2, "Number of data values", "COUNT" },
            {NITF_BCS_N, 3, "Start value in ramp", "START" },
            {NITF_BCS_N, 2, "Increment between values in ramp", "INCREMENT" },
            {NITF_END, 0, NULL, NULL}
        };
        */
        nitf::TREField_BCS_N<2> COUNT;
        nitf::TREField_BCS_N<3> START;
        nitf::TREField_BCS_N<2> INCREMENT;

        void updateFields()
        {
            tre_.updateFields();
        }
    };

} // namespace TREs
} // namespace nitf