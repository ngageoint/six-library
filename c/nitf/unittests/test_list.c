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

#include <import/nitf.h>
#include "Test.h"

TEST_CASE(testCreate)
{
    nitf_Error e;
    nitf_List *l = nitf_List_construct(&e);
    TEST_ASSERT(l);
    nitf_List_destruct(&l);
    TEST_ASSERT_NULL(l);
}

TEST_CASE(testPushPop)
{
    nitf_Error e;
    nitf_List *l = NULL;
    nitf_ListIterator it, endList;
    int i = 0;

    l = nitf_List_construct(&e);
    TEST_ASSERT(l);

    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)"NITRO", &e));
    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)"Rocks!", &e));

    TEST_ASSERT_EQ_INT(2, nitf_List_size(l));

    /* test iterating */
    it = nitf_List_begin(l);
    endList = nitf_List_end(l);
    for(; nitf_ListIterator_notEqualTo( &it, &endList ); ++i)
    {
        char* p = (char*)nitf_ListIterator_get(&it);
        TEST_ASSERT(p);
        nitf_ListIterator_increment(&it);
    }
    TEST_ASSERT_EQ_INT(2, i);

    /* test emptying the list */
    it = nitf_List_begin(l);
    while (!nitf_List_isEmpty(l))
    {
        char* p = (char*)nitf_List_popFront(l);
        TEST_ASSERT(p);
    }
    TEST_ASSERT_EQ_INT(0, nitf_List_size(l));
    nitf_List_destruct(&l);
    TEST_ASSERT_NULL(l);
}

char* cloneString(char* data, nitf_Error* error)
{
    int data_len = strlen(data);
    char* new_data = (char*) NITF_MALLOC(data_len + 1);
    new_data[data_len] = 0;
    assert(new_data);
    strcpy(new_data, data);
    return new_data;
}

TEST_CASE(testClone)
{
    nitf_Uint32 i;
    nitf_Error e;
    nitf_List *l = nitf_List_construct(&e), *dolly = NULL;
    TEST_ASSERT(l);

    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)cloneString("1", NULL), &e));
    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)cloneString("2", NULL), &e));
    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)cloneString("3", NULL), &e));
    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)cloneString("4", NULL), &e));
    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)cloneString("5", NULL), &e));

    dolly = nitf_List_clone(l, (NITF_DATA_ITEM_CLONE)cloneString, &e);
    TEST_ASSERT(dolly);
    TEST_ASSERT_EQ_INT(nitf_List_size(l), nitf_List_size(dolly));

    i = 0;
    while (!nitf_List_isEmpty(dolly))
    {
        char* p = (char*)nitf_List_popFront(dolly);
        TEST_ASSERT(p);
        TEST_ASSERT_EQ_INT(NITF_ATO32(p), ++i);
    }

    TEST_ASSERT_EQ_INT(0, nitf_List_size(dolly));
    nitf_List_destruct(&dolly);
    TEST_ASSERT_NULL(dolly);
    nitf_List_destruct(&l);
    TEST_ASSERT_NULL(l);
}

TEST_CASE(testIterate)
{
    nitf_Uint32 i;
    nitf_Error e;
    nitf_List *l = nitf_List_construct(&e), *dolly = NULL;
    nitf_ListIterator it, end;

    TEST_ASSERT(l);

    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)cloneString("1", NULL), &e));
    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)cloneString("2", NULL), &e));
    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)cloneString("3", NULL), &e));
    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)cloneString("4", NULL), &e));
    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)cloneString("5", NULL), &e));

    it = nitf_List_begin(l);
    end = nitf_List_end(l);

    i = 0;
    while ( nitf_ListIterator_notEqualTo( &it, &end ) )
    {
        char* p = (char*)nitf_ListIterator_get(&it);
        TEST_ASSERT(p);
        TEST_ASSERT_EQ_INT(NITF_ATO32(p), ++i);
        nitf_ListIterator_increment(&it);
    }

    nitf_List_destruct(&l);
    TEST_ASSERT_NULL(l);
}

TEST_CASE(testIterateRemove)
{
    nitf_Error e;
    nitf_List *l = nitf_List_construct(&e), *dolly = NULL;
    nitf_ListIterator it, end;

    TEST_ASSERT(l);

    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)cloneString("1", NULL), &e));
    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)cloneString("2", NULL), &e));
    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)cloneString("3", NULL), &e));
    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)cloneString("4", NULL), &e));
    TEST_ASSERT(nitf_List_pushBack(l, (NITF_DATA*)cloneString("5", NULL), &e));

    it = nitf_List_begin(l);
    end = nitf_List_end(l);

    while ( nitf_ListIterator_notEqualTo( &it, &end ) )
    {
        char* p = (char*)nitf_List_remove(l, &it);
        TEST_ASSERT(p);
    }
    TEST_ASSERT_EQ_INT(0, nitf_List_size(l));

    nitf_List_destruct(&l);
    TEST_ASSERT_NULL(l);
}

int main(int argc, char **argv)
{
    CHECK(testCreate);
    CHECK(testPushPop);
    CHECK(testClone);
    CHECK(testIterate);
    CHECK(testIterateRemove);
    return 0;
}
