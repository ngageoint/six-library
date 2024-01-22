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

#include <import/nitf.h>
#include <nitf/UnitTests.hpp>
#include "TestCase.h"

TEST_CASE(testNestedMod)
{
    TEST_ASSERT_TRUE( nitf_PluginRegistry_PreloadedTREHandlerEnable("ACCHZB", NRT_TRUE) );

    nitf_Error error;
    NITF_BOOL exists;
    nitf_TRE* tre = nitf_TRE_construct("ACCHZB", NULL, &error);
    TEST_ASSERT(tre != NULL);
    exists = nitf_TRE_setField(tre, "NUMACHZ", "02", 2, &error);
    TEST_ASSERT(exists);
    exists = nitf_TRE_setField(tre, "UNIAAH[0]", "abc", 3, &error);
    TEST_ASSERT(exists);
    exists = nitf_TRE_setField(tre, "UNIAAH[1]", "cba", 3, &error);
    TEST_ASSERT(exists);
    TEST_ASSERT_EQ_STR(nitf_TRE_getField(tre, "UNIAAH[0]")->raw, "abc");
    TEST_ASSERT_EQ_STR(nitf_TRE_getField(tre, "UNIAAH[1]")->raw, "cba");
    exists = nitf_TRE_setField(tre, "AAH[0]", "00000", 5, &error);
    TEST_ASSERT(exists);
    exists = nitf_TRE_setField(tre, "UNIAPH[0]", "def", 3, &error);
    TEST_ASSERT(exists);
    exists = nitf_TRE_setField(tre, "APH[0]", "00000", 5, &error);
    TEST_ASSERT(exists);
    exists = nitf_TRE_setField(tre, "NUMPTS[0]", "001", 3, &error);
    TEST_ASSERT(exists);
    exists =
        nitf_TRE_setField(tre, "LON[0][0]", "000000000000000", 15, &error);
    TEST_ASSERT(exists);
    exists =
        nitf_TRE_setField(tre, "LAT[0][0]", "000000000000000", 15, &error);
    TEST_ASSERT(exists);

    /* destruct the TREs */
    nitf_TRE_destruct(&tre);
}

TEST_CASE(testIncompleteCondMod)
{
    TEST_ASSERT_TRUE( nitf_PluginRegistry_PreloadedTREHandlerEnable("ACCPOB", NRT_TRUE) );

    nitf_Error error;
    NITF_BOOL exists;
    nitf_TRE* tre = nitf_TRE_construct("ACCPOB", NULL, &error);
    TEST_ASSERT(tre != NULL);
    int treLength = tre->handler->getCurrentSize(tre, &error);
    TEST_ASSERT_EQ(treLength, 2);

    exists = nitf_TRE_setField(tre, "NUMACPO", "01", 2, &error);
    TEST_ASSERT(exists);

    exists = nitf_TRE_setField(tre, "UNIAAH[0]", "FT0", 3, &error);
    TEST_ASSERT(exists);
    treLength = tre->handler->getCurrentSize(tre, &error);
    TEST_ASSERT_EQ(treLength, 37);

    exists = nitf_TRE_setField(tre, "NUMPTS[0]", "002", 3, &error);
    TEST_ASSERT(exists);
    treLength = tre->handler->getCurrentSize(tre, &error);
    TEST_ASSERT_EQ(treLength, 97);

    /* destruct the TREs */
    nitf_TRE_destruct(&tre);
}

TEST_CASE(testClone)
{
    NITF_BOOL exists;
    nitf_TRE* dolly;            /* used for clone */
    nitf_Field* clonedField = NULL;
    nitf_Error error;

    TEST_ASSERT_TRUE( nitf_PluginRegistry_PreloadedTREHandlerEnable("JITCID", NRT_TRUE) );
    nitf_TRE* tre = nitf_TRE_construct("JITCID", NULL, &error);
    TEST_ASSERT(tre != NULL);

    exists = nitf_TRE_setField(tre, "FILCMT", "fyi", 3, &error);
    TEST_ASSERT(exists);

    dolly = nitf_TRE_clone(tre, &error);
    TEST_ASSERT(dolly != NULL);

    clonedField = nitf_TRE_getField(dolly, "FILCMT");
    TEST_ASSERT(clonedField != NULL);
    TEST_ASSERT_EQ_STR(clonedField->raw, "fyi");

    /* destruct the TREs */
    nitf_TRE_destruct(&tre);
    nitf_TRE_destruct(&dolly);
}

TEST_CASE(testBasicMod)
{
    TEST_ASSERT_TRUE( nitf_PluginRegistry_PreloadedTREHandlerEnable("ACFTA", NRT_TRUE) );

    /* construct a tre */
    NITF_BOOL exists;
    nitf_Error error;
    nitf_TRE *tre = nitf_TRE_construct("ACFTA", "ACFTA_132", &error);
    TEST_ASSERT(tre != NULL);
    nitf_Field* field = (nitf_TRE_getField(tre, "AC_MSN_ID"));
    TEST_ASSERT_EQ_STR(field->raw, "          ");

    exists = nitf_TRE_setField(tre, "AC_MSN_ID", "fly-by", 6, &error);
    TEST_ASSERT(exists);
    field = nitf_TRE_getField(tre, "AC_MSN_ID");
    TEST_ASSERT_EQ_STR(field->raw, "fly-by    ");

    exists = nitf_TRE_setField(tre, "AC_MSN_ID", "sky-photo", 9, &error);
    TEST_ASSERT(exists);
    field = nitf_TRE_getField(tre, "AC_MSN_ID");
    TEST_ASSERT_EQ_STR(field->raw, "sky-photo ");

    /* make sure that we can't set an invalid tag */
    exists = nitf_TRE_setField(tre, "invalid-tag", "sky-photo", 9, &error);
    TEST_ASSERT(!exists);

    /* destruct the TRE */
    nitf_TRE_destruct(&tre);
}

TEST_CASE(testSize)
{
    TEST_ASSERT_TRUE( nitf_PluginRegistry_PreloadedTREHandlerEnable("AIMIDB", NRT_TRUE) );
    nitf_Error error;
    int treLength;
    nitf_TRE* tre = nitf_TRE_construct("AIMIDB", NULL, &error);

    TEST_ASSERT(tre != NULL);
    treLength = tre->handler->getCurrentSize(tre, &error);
    TEST_ASSERT_EQ(treLength, 89);

    /* destruct the TRE */
    nitf_TRE_destruct(&tre);
}

TEST_CASE(iterateUnfilled)
{
    nitf_Error error;
    nitf_TRECursor cursor;
    nitf_TRE* tre = nitf_TRE_construct("ACCPOB", NULL, &error);
    uint32_t numFields = 0;
    TEST_ASSERT(tre != NULL);
    cursor = nitf_TRECursor_begin(tre);

    while (!nitf_TRECursor_isDone(&cursor))
    {
        TEST_ASSERT(nitf_TRECursor_iterate(&cursor, &error) != 0);
        ++numFields;
    }

    TEST_ASSERT_EQ(numFields, (uint32_t)1);

    nitf_TRECursor_cleanup(&cursor);
    nitf_TRE_destruct(&tre);
}

TEST_CASE(populateThenIterate)
{
    TEST_ASSERT_TRUE( nitf_PluginRegistry_PreloadedTREHandlerEnable("ACCPOB", NRT_TRUE) );
    nitf_Error error;
    nitf_TRE* tre = nitf_TRE_construct("ACCPOB", NULL, &error);
    TEST_ASSERT_NOT_NULL(tre);

    uint32_t numFields = 0;
    nitf_TRE_setField(tre, "NUMACPO", "2", 1, &error);
    nitf_TRE_setField(tre, "NUMPTS[0]", "3", 1, &error);
    nitf_TRE_setField(tre, "NUMPTS[1]", "2", 1, &error);

    nitf_TRECursor cursor = nitf_TRECursor_begin(tre);
    while (!nitf_TRECursor_isDone(&cursor))
    {
        TEST_ASSERT(nitf_TRECursor_iterate(&cursor, &error) != 0);
        ++numFields;
    }

    TEST_ASSERT_EQ(numFields, (uint32_t)29);

    nitf_TRECursor_cleanup(&cursor);
    nitf_TRE_destruct(&tre);
}

TEST_CASE(populateWhileIterating)
{
    TEST_ASSERT_TRUE( nitf_PluginRegistry_PreloadedTREHandlerEnable("ACCPOB", NRT_TRUE) );
    nitf_Error error;
    nitf_TRE* tre = nitf_TRE_construct("ACCPOB", NULL, &error);
    TEST_ASSERT_NOT_NULL(tre);

    uint32_t numFields = 0;
    nitf_TRECursor cursor = nitf_TRECursor_begin(tre);
    while (!nitf_TRECursor_isDone(&cursor))
    {
        TEST_ASSERT(nitf_TRECursor_iterate(&cursor, &error) != 0);
        ++numFields;
        if (strcmp(cursor.tag_str, "NUMACPO") == 0)
        {
            nitf_TRE_setField(cursor.tre, cursor.tag_str, "2", 1, &error);
        }
        else if (strcmp(cursor.tag_str, "NUMPTS[0]") == 0)
        {
            nitf_TRE_setField(cursor.tre, cursor.tag_str, "3", 1, &error);
        }
        else if (strcmp(cursor.tag_str, "NUMPTS[1]") == 0)
        {
            nitf_TRE_setField(cursor.tre, cursor.tag_str, "2", 1, &error);
        }
    }
    TEST_ASSERT_EQ(numFields, (uint32_t)29);

    nitf_TRECursor_cleanup(&cursor);
    nitf_TRE_destruct(&tre);
}

TEST_MAIN(
    TEST_CHECK(testClone);
    TEST_CHECK(testSize);
    TEST_CHECK(testBasicMod);
    TEST_CHECK(testNestedMod);
    TEST_CHECK(testIncompleteCondMod);
    TEST_CHECK(iterateUnfilled);
    TEST_CHECK(populateThenIterate);
    TEST_CHECK(populateWhileIterating);
    )
