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


int showTRE(nitf_TRE* tre)
{
    nitf_Error error;
    int i = 0;
    nitf_Uint32 treLength;
    nitf_TREEnumerator* it;

    treLength = tre->handler->getCurrentSize(tre, &error);

    printf("\n--------------- %s TRE (%d) ---------------\n",
           tre->tag, treLength);

    it = nitf_TRE_begin(tre, &error);
    while(it && it->hasNext(&it))
    {
        nitf_Pair* fieldPair = it->next(it, &error);
        i++;
        if (fieldPair)
        {
            printf("%s = [", fieldPair->key);
            nitf_Field_print((nitf_Field *) fieldPair->data);
            printf("]\n");
        }
        else
        {
            printf("ERROR, no field found!\n");
        }
    }
    return 1;
}

NITF_BOOL testNestedMod(nitf_Error* error)
{
    NITF_BOOL exists;
    nitf_TRE* tre = nitf_TRE_construct("ACCHZB", NULL, error);
    if (!tre)
    {
        return NITF_FAILURE;
    }
    exists = nitf_TRE_setField(tre, "NUMACHZ", "01", 2, error);
    printf("Set ok? %s\n", exists ? "OK" : "FAIL");
    exists = nitf_TRE_setField(tre, "UNIAAH[0]", "abc", 3, error);
    printf("Set ok? %s\n", exists ? "OK" : "FAIL");
    exists = nitf_TRE_setField(tre, "AAH[0]", "00000", 5, error);
    printf("Set ok? %s\n", exists ? "OK" : "FAIL");
    exists = nitf_TRE_setField(tre, "UNIAPH[0]", "def", 3, error);
    printf("Set ok? %s\n", exists ? "OK" : "FAIL");
    exists = nitf_TRE_setField(tre, "APH[0]", "00000", 5, error);
    printf("Set ok? %s\n", exists ? "OK" : "FAIL");
    exists = nitf_TRE_setField(tre, "NUMPTS[0]", "001", 3, error);
    printf("Set ok? %s\n", exists ? "OK" : "FAIL");
    exists =
        nitf_TRE_setField(tre, "LON[0][0]", "000000000000000", 15, error);
    printf("Set ok? %s\n", exists ? "OK" : "FAIL");
    exists =
        nitf_TRE_setField(tre, "LAT[0][0]", "000000000000000", 15, error);
    printf("Set ok? %s\n", exists ? "OK" : "FAIL");

    /* print the TRE. should print the value now */
    showTRE(tre);

    /* destruct the TREs */
    nitf_TRE_destruct(&tre);
return NITF_SUCCESS;
}

NITF_BOOL testIncompleteCondMod(nitf_Error* error)
{
    NITF_BOOL exists;
    nitf_TRE* tre = nitf_TRE_construct("ACCPOB", NULL, error);
    if (!tre)
    {
        return NITF_FAILURE;
    }
    printf("Before Mods\n");
    showTRE(tre);
    printf("\n\n");

    exists = nitf_TRE_setField(tre, "NUMACPO", "01", 2, error);
    if (!exists)
    {
        return NITF_FAILURE;
    }

    printf("Again\n");
    showTRE(tre);
    printf("\n\n");

    exists = nitf_TRE_setField(tre, "UNIAAH[0]", "FT0", 3, error);
    if (!exists)
    {
        return NITF_FAILURE;
    }

    exists = nitf_TRE_setField(tre, "NUMPTS[0]", "002", 3, error);
    if (!exists)
    {
        return NITF_FAILURE;
    }


    /* print the TRE. should print the value now */
    printf("After Mods\n");
    showTRE(tre);


    /* destruct the TREs */
    nitf_TRE_destruct(&tre);
    return NITF_SUCCESS;

}

NITF_BOOL testClone(nitf_Error* error)
{

    NITF_BOOL exists;
    nitf_TRE *dolly;            /* used for clone */
    nitf_TRE* tre = nitf_TRE_construct("JITCID", NULL, error);
    if (!tre)
    {
        return NITF_FAILURE;
    }
    exists = nitf_TRE_setField(tre, "FILCMT", "fyi", 3, error);
    printf("Set ok? %s\n", exists ? "yes" : "no");

    /* print the TRE. should print the value now */
    showTRE(tre);

    /* now, clone it */
    dolly = nitf_TRE_clone(tre, error);
    if (!dolly)
    {
        return NITF_FAILURE;
    }
    printf("About to print the cloned TRE\n");
    showTRE(dolly);

    /* destruct the TREs */
    nitf_TRE_destruct(&tre);
    nitf_TRE_destruct(&dolly);
    return NITF_SUCCESS;
}

NITF_BOOL testBasicMod(nitf_Error* error)
{
    /* construct a tre */
    NITF_BOOL exists;
    nitf_TRE *tre = nitf_TRE_construct("ACFTA", "ACFTA_132", error);
    if (!tre)
    {
        return NITF_FAILURE;
    }

    /* print the TRE -- should print fields, but all blank */
    showTRE(tre);

    /* set a value. make sure it went ok */
    printf("Now, set the value of 'AC_MSN_ID' to 'fly-by'. Set ok? ");
    exists = nitf_TRE_setField(tre, "AC_MSN_ID", "fly-by", 6, error);
    printf("%s\n", exists ? "OK" : "FAIL");

    printf("Now, print the TRE:\n----------\n");
    /* print the TRE. should print the value now */
    showTRE(tre);
    printf("----------\n");

    /* re-set the same value. make sure it went ok */
    printf("Now, re-set the value of 'AC_MSN_ID' to 'sky-photo'. Set ok? ");
    exists = nitf_TRE_setField(tre, "AC_MSN_ID", "sky-photo", 9, error);
    printf("%s\n", exists ? "OK" : "FAIL");

    printf("Now, print the TRE:\n----------\n");
    /* print the TRE. should print the value now */
    showTRE(tre);
    printf("----------\n");

    /* make sure that we can't set an invalid tag */
    printf("Try to set an invalid tag. Can we? ");
    exists = nitf_TRE_setField(tre, "invalid-tag", "sky-photo", 9, error);
    printf("%s\n", exists ? "Yes. FAIL" : "No. GOOD!");

    printf("Now, print the TRE:\n----------\n");
    /* print the TRE. should print the value now */
    showTRE(tre);
    printf("----------\n");

    /* destruct the TRE */
    nitf_TRE_destruct(&tre);

    return NITF_SUCCESS;
}

NITF_BOOL testSize(nitf_Error* error)
{
    nitf_TRE* tre = nitf_TRE_construct("AIMIDB", NULL, error);
    int treLength;

    if (!tre)
    {
        nitf_Error_print(error, stdout, "Exiting...");
        return NITF_FAILURE;
    }

    treLength = tre->handler->getCurrentSize(tre, error);

    printf("Computed TRE Length = %d\n", treLength);

    /* destruct the TRE */
    nitf_TRE_destruct(&tre);
    return NITF_SUCCESS;
}

/* This test should be used to verify the TRE modifications work.
 * It probably should not be added to a core canon of tests, but
 * it is useful to test any changes with the TRE API
 */
int main(int argc, char **argv)
{
    /*  Get the error object       */
    nitf_Error error;

    if (!testClone(&error))
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }

    if (!testSize(&error))
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);

    }

    if (!testBasicMod(&error))
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);

    }

    if (!testNestedMod(&error))
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);

    }

    if (!testIncompleteCondMod(&error))
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);

    }


    /* TODO: this *is* important, but in order to continue
       other test, I need to block it out for now */

    /* now, let's try to construct based on a description set
    {
        nitf_TREDescriptionInfo *infoPtr = NULL;
        nitf_TREDescriptionSet *descriptions =
            nitf_TRE_getDescriptionSet("ACFTA", &error);
        int numDescriptions = 0;
        infoPtr = descriptions->descriptions;
        while (infoPtr && (infoPtr->description != NULL))
        {
            numDescriptions++;
            infoPtr++;
        }
        printf("Found %d descriptions for ACFTA\n", numDescriptions);

        infoPtr = descriptions->descriptions;
        while (infoPtr && (infoPtr->description != NULL))
        {
            printf("Name: %s, Length: %d\n--------------------------------\n",
                infoPtr->name, infoPtr->lengthMatch);
            tre = nitf_TRE_construct("ACFTA", NULL, &error);
            if (!tre)
            {
                nitf_Error_print(&error, stdout, "Exiting...");
                exit(EXIT_FAILURE);
            }
            nitf_TRE_print(tre, &error);
            infoPtr++;
            nitf_TRE_destruct(&tre);
            printf("--------------------------------\n");
        }
        }*/

    return 0;
}
