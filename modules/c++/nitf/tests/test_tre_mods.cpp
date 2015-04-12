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
#include <iostream>

/* This test should be used to verify the TRE modifications work.
 * It probably should not be added to a core canon of tests, but
 * it is useful to test any changes with the TRE API
 */
int main(int argc, char **argv)
{
    try
    {
        std::string divider(
            "---------------------------------------------------------");

        //create an ACFTA TRE
        nitf::TRE tre("ACFTA");
        //tre.print(); //print it
        std::cout << divider << std::endl;

        //set a field
        tre.setField("AC_MSN_ID", "fly-by");
        //tre.print();
        std::cout << divider << std::endl;

        //re-set the field
        tre.setField("AC_MSN_ID", 1.2345678);
        //tre.print();
        std::cout << divider << std::endl;

        //try setting an invalid tag
        try
        {
            tre.setField("invalid-tag", "some data");
        }
        catch (except::Throwable & t)
        {
            std::cout << t.getMessage() << std::endl;
        }

        // Now, try to create a SANE TRE
        tre = nitf::TRE("JITCID");
        tre.setField("FILCMT", "fyi");
        //tre.print();
        std::cout << divider << std::endl;

        //is sane?
        //std::cout << "Is Sane? " << (tre.isSane() ? "yes" : "no") << std::endl;

        // try cloning the tre
        nitf::TRE dolly = tre.clone();
        //dolly.print();
        std::cout << divider << std::endl;

        //the two should NOT be equal -- underlying object is different
        std::cout << "Equal? : " << (tre == dolly ? "yes" : "no") << std::endl;

        //is dolly sane?
        //std::cout << "Is Sane? " << (dolly.isSane() ? "yes" : "no") << std::endl;


        //let's try getting the TREDescriptions ourselves

/*        nitf::TREDescriptionInfo *infoPtr = NULL;
        nitf::TREDescriptionSet *descriptions =
            nitf::TRE::getTREDescriptionSet("ACFTA");
        int numDescriptions = 0;
        infoPtr = descriptions->descriptions;
        while (infoPtr && (infoPtr->description != NULL))
        {
            numDescriptions++;
            infoPtr++;
        }
        std::cout << "Found " << numDescriptions << " descriptions for ACFTA" << std::endl;

        infoPtr = descriptions->descriptions;
        while (infoPtr && (infoPtr->description != NULL))
        {
            std::cout << "Name: " << infoPtr->name << ", Length: " <<
                infoPtr->lengthMatch << std::endl << divider << std::endl;
            tre = nitf::TRE("ACFTA", infoPtr->description);
            tre.print();
            std::cout << divider << std::endl;
            infoPtr++;
        }
*/
    }
    catch (except::Throwable & t)
    {
        std::cout << t.getMessage() << std::endl;
    }
    return 0;
}
