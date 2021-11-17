/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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

#include <iostream>
#include <vector>

#include <nitf/TRE.hpp>

#include "TestCase.h"


 /* =========================================================================
  * This file is part of NITRO
  * =========================================================================
  *
  * (C) Copyright 2021, Maxar Technologies, Inc.
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
#include <stdint.h>
#include <string>

#include "nitf/TRE.hpp"
#include "nitf/exports.hpp"
#include "nitf/TREs/TREField.hpp"

// A sample "strongly-typed" TRE.  There are too many TREs (and too much unwillingness to change) to
// actually hook this up.  But it's kind of neat code that I don't want to lose.
namespace nitf
{
    namespace TREs
    {
        class NITRO_NITFCPP_API ENGRDA final
        {
            nitf::TRE tre_;

        public:
            // from TRE::getID()
            /**
             * Get the TRE identifier. This is NOT the tag, however it may be the
             * same value as the tag. The ID is used to identify a specific
             * version/incarnation of the TRE, if multiple are possible. For most TREs,
             * this value will be the same as the tag.
             */
            ENGRDA(const std::string& id = "") noexcept(false)
                : tre_("ENGRDA", id.empty() ? "ENGRDA" : id.c_str()),
                RESRC(tre_, "RESRC"),
                RECNT(tre_, "RECNT", true /*forceUpdate*/),
                ENGDTS{ tre_, "ENGDTS", RECNT },
                ENGDATC{ tre_, "ENGDATC",  RECNT },
                ENGDATA{ tre_, "ENGDATA",  RECNT }
            {
            }
            ~ENGRDA() = default;
            ENGRDA(const ENGRDA&) = delete;
            ENGRDA& operator=(const ENGRDA&) = delete;
            ENGRDA(ENGRDA&&) = default;
            ENGRDA& operator=(ENGRDA&&) = delete;

            // From ENGRDA.c
            //
            //static nitf_TREDescription description[] = {
            //    {NITF_BCS_A, 20, "Unique Source System Name", "RESRC" },
            TREField_BCS_A<20> RESRC;

            //    {NITF_BCS_N, 3, "Record Entry Count", "RECNT" },
            TREField_BCS_N<3> RECNT;

            //    {NITF_LOOP, 0, NULL, "RECNT"},
            //        {NITF_BCS_N, 2, "Engineering Data Label Length", "ENGLN" },
            //        /* This one we don't know the length of, so we have to use the special length tag */
            //        {NITF_BCS_A, NITF_TRE_CONDITIONAL_LENGTH, "Engineering Data Label",
            //                "ENGLBL", "ENGLN" },
            //        {NITF_BCS_N, 4, "Engineering Matrix Data Column Count", "ENGMTXC" },
            //        {NITF_BCS_N, 4, "Engineering Matrix Data Row Count", "ENGMTXR" },
            //        {NITF_BCS_A, 1, "Value Type of Engineering Data Element", "ENGTYP" },
            //        {NITF_BCS_N, 1, "Engineering Data Element Size", "ENGDTS" },
            IndexedField<TREField_BCS_N<1>> ENGDTS;

            //        {NITF_BCS_A, 2, "Engineering Data Units", "ENGDATU" },
            //        {NITF_BCS_N, 8, "Engineering Data Count", "ENGDATC" },
            IndexedField<TREField_BCS_N<8>> ENGDATC;

            //        /* This one we don't know the length of, so we have to use the special length tag */
            //        /* Notice that we use postfix notation to compute the length
            //         * We also don't know the type of data (it depends on ENGDTS), so
            //         * we need to override the TREHandler's read method.  If we don't do
            //         * this, not only will the field type potentially be wrong, but
            //         * strings will be endian swapped if they're of length 2 or 4. */
            //        {NITF_BINARY, NITF_TRE_CONDITIONAL_LENGTH, "Engineering Data",
            //                "ENGDATA", "ENGDATC ENGDTS *"},
            IndexedField<TREField_BCS_A<>> ENGDATA;

            //    {NITF_ENDLOOP, 0, NULL, NULL},
            //    {NITF_END, 0, NULL, NULL}
            //};

            template <typename T>
            void setFieldValue(const std::string& tag, const T& value, bool forceUpdate = false)
            {
                tre_.setFieldValue(tag, value, forceUpdate);
            }
            void setFieldValue(const std::string& tag, const void* data, size_t dataLength, bool forceUpdate = false)
            {
                tre_.setFieldValue(tag, data, dataLength, forceUpdate);
            }

            template<typename T>
            const T& getFieldValue(const std::string& tag, T& value) const
            {
                return tre_.getFieldValue(tag, value);
            }
            template<typename T>
            const T getFieldValue(const std::string& tag) const
            {
                return tre_.getFieldValue<T>(tag);
            }

            void updateFields()
            {
                tre_.updateFields();
            }
        };
    }
}

namespace
{
TEST_CASE(setFields)
{
    // create an ACFTA TRE
    nitf::TRE tre("ACFTA");

    // set a field
    tre.setField("AC_MSN_ID", "fly-by");
    TEST_ASSERT_EQ(tre.getField("AC_MSN_ID").toString(),
                   std::string("fly-by    "));

    // re-set the field
    tre.setField("AC_MSN_ID", 1.2345678);
    TEST_ASSERT_EQ(tre.getField("AC_MSN_ID").toString(),
                   std::string("1.2345678 "));

    // try setting an invalid tag
    TEST_EXCEPTION(tre.setField("invalid-tag", "some data"));
}

TEST_CASE(setBinaryFields)
{
    nitf::TRE tre("RPFHDR");
    const int value = 123;
    tre.setField("LOCSEC", value);

    nitf::Field field = tre.getField("LOCSEC");
    const int readValue = *reinterpret_cast<int*>(field.getRawData());
    TEST_ASSERT_EQ(readValue, value);
}

TEST_CASE(cloneTRE)
{
    nitf::TRE tre("JITCID");
    tre.setField("FILCMT", "fyi");

    // try cloning the tre
    nitf::TRE dolly = tre.clone();

    // the two should NOT be equal -- underlying object is different
    TEST_ASSERT(tre != dolly);
    TEST_ASSERT_EQ(tre.getField("FILCMT").toString(), std::string("fyi"));
}

TEST_CASE(basicIteration)
{
    nitf::TRE tre("ACCPOB");

    // The entire TRE is one loop, and we haven't told it
    // how many elements there are, so there's nothing to
    // iterate over
    size_t numFields = 0;
    for (const auto& unused : tre)
    {
        (void)unused;
        ++numFields;
    }
    TEST_ASSERT_EQ(numFields, static_cast<size_t>(1));

    numFields = 0;
    tre.setField("NUMACPO", 2, true);
    tre.setField("NUMPTS[0]", 3);
    tre.setField("NUMPTS[1]", 2);
    for (const auto& unused : tre)
    {
        (void)unused;
        ++numFields;
    }
    TEST_ASSERT_EQ(numFields, static_cast<size_t>(29));
}

TEST_CASE(use_ENGRDA)
{
    nitf::TRE engrda("ENGRDA", "ENGRDA");

    engrda.setField("RESRC", "HSS");
    engrda.setField("RECNT", 1, true /*forceUpdate*/);

    // From ENGRDA.c
    ///* This one we don't know the length of, so we have to use the special length tag */
    ///* Notice that we use postfix notation to compute the length
    // * We also don't know the type of data (it depends on ENGDTS), so
    // * we need to override the TREHandler's read method.  If we don't do
    // * this, not only will the field type potentially be wrong, but
    // * strings will be endian swapped if they're of length 2 or 4. */
    //{NITF_BINARY, NITF_TRE_CONDITIONAL_LENGTH, "Engineering Data",
    //    "ENGDATA", "ENGDATC ENGDTS *"},
    engrda.setField("ENGDTS[0]", 3); // size
    engrda.setField("ENGDATC[0]", 1); // count
    engrda.updateFields();
    engrda.setField("ENGDATA[0]", "ABC");
}

TEST_CASE(use_typed_ENGRDA)
{
    nitf::TREs::ENGRDA engrda; // nitf::TRE engrda("ENGRDA", "ENGRDA");

    engrda.RESRC = "HSS"; // engrda.setField("RESRC", "HSS");
    const std::string RESRC = engrda.RESRC;
    engrda.RECNT = 1; // engrda.setField("RECNT", 1, true /*forceUpdate*/);
    const int64_t RECNT = engrda.RECNT;

    engrda.ENGDTS[0] = 3; // engrda.setField("ENGDTS[0]", 3); // size
    const int64_t ENGDTS_0 = engrda.ENGDTS[0];
    engrda.ENGDATC[0] = 1; // engrda.setField("ENGDATC[0]", 1); // count
    const int64_t ENGDATC_0 = engrda.ENGDATC[0];
    engrda.updateFields();
    engrda.ENGDATA[0] = "ABC"; // engrda.setField("ENGDATA[0]", "ABC");
    const auto& engrda_ = engrda;
    const auto ENGDATA_0 = engrda_.ENGDATA[0];

    try
    {
        (void)engrda_.ENGDATA.at(999);
        TEST_ASSERT_FALSE(false);
    }
    catch (const std::out_of_range&)
    {
        TEST_ASSERT_TRUE(true);
    }

    try
    {
        (void)engrda_.ENGDATA[999]; // unchecked can still throw, just a different exception
        TEST_ASSERT_FALSE(false);
    }
    catch (const except::NoSuchKeyException&)
    {
        TEST_ASSERT_TRUE(true);
    }

}

TEST_CASE(populateWhileIterating)
{
    nitf::TRE tre("ACCPOB");
    size_t numFields = 0;
    for (nitf::TRE::Iterator it = tre.begin(); it != tre.end(); ++it)
    {
        ++numFields;
        const std::string fieldName((*it).first());
        if (fieldName == "NUMACPO")
        {
            tre.setField("NUMACPO", 2);
        }
        else if (fieldName == "NUMPTS[0]")
        {
            tre.setField("NUMPTS[0]", 3);
        }
        else if (fieldName == "NUMPTS[1]")
        {
            tre.setField("NUMPTS[1]", 2);
        }
    }
    TEST_ASSERT_EQ(numFields, static_cast<size_t>(29));
}

TEST_CASE(overflowingNumericFields)
{
    nitf::TRE tre("CSCRNA");

    // This field has a length of 9, so check that it's properly
    // truncated
    tre.setField("ULCNR_LAT", 1.0 / 9);
    TEST_ASSERT_EQ(tre.getField("ULCNR_LAT").toString(), "0.1111111");
    TEST_ASSERT_EQ(tre.getFieldValue<std::string>("ULCNR_LAT"), "0.1111111");
    std::string value;
    TEST_ASSERT_EQ(tre.getFieldValue("ULCNR_LAT", value), "0.1111111");

    tre.setField("ULCNR_LAT", 123456789);
    TEST_ASSERT_EQ(tre.getField("ULCNR_LAT").toString(), "123456789");

    tre.setField("ULCNR_LAT", 12345678.);
    TEST_ASSERT_EQ(tre.getField("ULCNR_LAT").toString(), "012345678");

    tre.setField("ULCNR_LAT", 12345678.9);
    TEST_ASSERT_EQ(tre.getField("ULCNR_LAT").toString(), "012345678");

    tre.setField("ULCNR_LAT", 1);
    TEST_ASSERT_EQ(tre.getField("ULCNR_LAT").toString(), "000000001");

    // If we run out of digits before hitting the decimal, there's no
    // saving it
    TEST_EXCEPTION(tre.setField("ULCNR_LAT", 123456789012LL));
}
}

TEST_MAIN(
    TEST_CHECK(setFields);
    TEST_CHECK(setBinaryFields);
    TEST_CHECK(cloneTRE);
    TEST_CHECK(basicIteration);
    TEST_CHECK(use_ENGRDA);
    TEST_CHECK(use_typed_ENGRDA);
    TEST_CHECK(populateWhileIterating);
    TEST_CHECK(overflowingNumericFields);
    )

