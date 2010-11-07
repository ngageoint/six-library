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

#include <import/nitf.hpp>

int main(int argc, char **argv)
{
    try
    {
        /*  Iterator to a list  */
        nitf::ListIterator it;

        /*  Iterator to the end of list  */
        nitf::ListIterator endList;

        /*  Integer for iteration  */
        int i;

        /*  Construct a new list  */
        nitf::List l;

        /*  Put in  */
        for (i = 0; i < argc; i++)
        {
            /*  Push the data back  */
            l.pushBack((NITF_DATA*)argv[i]);

        }
        /*  Recall  */
        it = l.begin();

        /*  End of list pointer  */
        endList = l.end();

        /*  While we are not at the end  */
        while ( it != endList)
        {
            /*  Get the last data  */
            char* p = (char*)it.get();
            /*  Make sure  */
            assert(p != NULL);

            /*  Show the data  */
            printf("Found data: [%s]\n", p);

            /*  Increment the list iterator  */
            it++;
        }

        /*  Set the list to the beginning  */
        it = l.begin();

        /*  Pop the front off, until the list is empty  */
        while ( !l.isEmpty() )
        {
            char* p = (char*)l.popFront();
            printf("Popping data value [%s]\n", p);
        }

        /*  Double check  */
        //assert( l.getHandle() == NULL );
    }
    catch (except::Throwable& t)
    {
        std::cout << t.getTrace() << std::endl;
    }
}
