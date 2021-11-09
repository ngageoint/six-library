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

#include <import/nrt.h>
#include "Test.h"

TEST_CASE(testCreate)
{
    nrt_Error e;
    nrt_List *l = nrt_List_construct(&e);
    TEST_ASSERT(l);
    nrt_List_destruct(&l);
    TEST_ASSERT_NULL(l);
}

TEST_CASE(testPushPop)
{
    nrt_Error e;
    nrt_List *l = NULL;
    nrt_ListIterator it, endList;
    int i = 0;

    l = nrt_List_construct(&e);
    TEST_ASSERT(l);

    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) "NITRO", &e));
    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) "Rocks!", &e));

    TEST_ASSERT_EQ_INT(2, nrt_List_size(l));

    /* test iterating */
    it = nrt_List_begin(l);
    endList = nrt_List_end(l);
    for (; nrt_ListIterator_notEqualTo(&it, &endList); ++i)
    {
        char *p = (char *) nrt_ListIterator_get(&it);
        TEST_ASSERT(p);
        nrt_ListIterator_increment(&it);
    }
    TEST_ASSERT_EQ_INT(2, i);

    /* test emptying the list */
    it = nrt_List_begin(l);
    while (!nrt_List_isEmpty(l))
    {
        char *p = (char *) nrt_List_popFront(l);
        TEST_ASSERT(p);
    }
    TEST_ASSERT_EQ_INT(0, nrt_List_size(l));
    nrt_List_destruct(&l);
    TEST_ASSERT_NULL(l);
}

char *cloneString(const char *data, nrt_Error * error)
{
    (void)error;

    char* new_data = nrt_strdup(data);
    assert(new_data);
    return new_data;
}

TEST_CASE(testClone)
{
    nrt_Error e;
    nrt_List *l = nrt_List_construct(&e), *dolly = NULL;
    TEST_ASSERT(l);

    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) cloneString("1", NULL), &e));
    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) cloneString("2", NULL), &e));
    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) cloneString("3", NULL), &e));
    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) cloneString("4", NULL), &e));
    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) cloneString("5", NULL), &e));

    dolly = nrt_List_clone(l, (NRT_DATA_ITEM_CLONE) cloneString, &e);
    TEST_ASSERT(dolly);
    TEST_ASSERT_EQ_INT(nrt_List_size(l), nrt_List_size(dolly));

    int32_t i = 0;
    while (!nrt_List_isEmpty(dolly))
    {
        char *p = (char *) nrt_List_popFront(dolly);
        TEST_ASSERT(p);
        TEST_ASSERT_EQ_INT(NRT_ATO32(p), ++i);
    }

    TEST_ASSERT_EQ_INT(0, nrt_List_size(dolly));
    nrt_List_destruct(&dolly);
    TEST_ASSERT_NULL(dolly);
    nrt_List_destruct(&l);
    TEST_ASSERT_NULL(l);
}

TEST_CASE(testIterate)
{
    int32_t i;
    nrt_Error e;
    nrt_List *l = nrt_List_construct(&e);
    nrt_ListIterator it, end;

    TEST_ASSERT(l);

    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) cloneString("1", NULL), &e));
    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) cloneString("2", NULL), &e));
    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) cloneString("3", NULL), &e));
    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) cloneString("4", NULL), &e));
    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) cloneString("5", NULL), &e));

    it = nrt_List_begin(l);
    end = nrt_List_end(l);

    i = 0;
    while (nrt_ListIterator_notEqualTo(&it, &end))
    {
        char *p = (char *) nrt_ListIterator_get(&it);
        TEST_ASSERT(p);
        TEST_ASSERT_EQ_INT(NRT_ATO32(p), ++i);
        nrt_ListIterator_increment(&it);
    }

    nrt_List_destruct(&l);
    TEST_ASSERT_NULL(l);
}

TEST_CASE(testIterateRemove)
{
    nrt_Error e;
    nrt_List *l = nrt_List_construct(&e);
    nrt_ListIterator it, end;

    TEST_ASSERT(l);

    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) cloneString("1", NULL), &e));
    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) cloneString("2", NULL), &e));
    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) cloneString("3", NULL), &e));
    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) cloneString("4", NULL), &e));
    TEST_ASSERT(nrt_List_pushBack(l, (NRT_DATA *) cloneString("5", NULL), &e));

    it = nrt_List_begin(l);
    end = nrt_List_end(l);

    while (nrt_ListIterator_notEqualTo(&it, &end))
    {
        char *p = (char *) nrt_List_remove(l, &it);
        TEST_ASSERT(p);
    }
    TEST_ASSERT_EQ_INT(0, nrt_List_size(l));

    nrt_List_destruct(&l);
    TEST_ASSERT_NULL(l);
}

TEST_MAIN(
    (void)argc;
    (void)argv;

    CHECK(testCreate);
    CHECK(testPushPop);
    CHECK(testClone);
    CHECK(testIterate);
    CHECK(testIterateRemove);
)
