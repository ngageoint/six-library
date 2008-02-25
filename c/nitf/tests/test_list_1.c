/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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

int main(int argc, char **argv)
{
    /*  We need the error  */
    nitf_Error e;

    /*  Create a list  */
    nitf_List *l;

    /*  Iterator to a list  */
    nitf_ListIterator it;

    /*  Iterator to the end of list  */
    nitf_ListIterator endList;

    /*  Integer for iteration  */
    int i;

    /*  Construct a new list  */
    l = nitf_List_construct(&e);


    if (!l)
    {
        /*  Print an error die  */
        nitf_Error_fprintf(&e, stdout, "Exiting at [%d]", NITF_LINE);
        exit(EXIT_FAILURE);
    }

    /*  Put in  */
    for (i = 0; i < argc; i++)
    {
        /*  Push the data back  */
        if (!nitf_List_pushBack(l, (NITF_DATA*)argv[i], &e))
        {
            nitf_Error_fprintf(&e, stdout, "Exiting at [%d]", NITF_LINE);
            exit(EXIT_FAILURE);
        }

    }
    /*  Recall  */
    it      = nitf_List_begin(l);

    /*  End of list pointer  */
    endList = nitf_List_end(l);

    /*  While we are not at the end  */
    while ( nitf_ListIterator_notEqualTo( &it, &endList ) )
    {
        /*  Get the last data  */
        char* p = (char*)nitf_ListIterator_get(&it);
        /*  Make sure  */
        assert(p != NULL);

        /*  Show the data  */
        printf("Found data: [%s]\n", p);

        /*  Increment the list iterator  */
        nitf_ListIterator_increment(&it);
    }

    /*  Set the list to the beginning  */
    it      = nitf_List_begin(l);

    /*  Pop the front off, until the list is empty  */
    while ( ! nitf_List_isEmpty(l) )
    {
        char* p = (char*)nitf_List_popFront(l);
        printf("Popping data value [%s]\n", p);
    }
    /*  Destroy the list  */
    nitf_List_destruct(&l);

    /*  Double check  */
    assert( l == NULL );
    return 0;
}
