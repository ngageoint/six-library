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

/* used for the ENGRDA test below */
/*static nitf_TREDescription description[] = {
    {NITF_BCS_A, 20, "Unique Source System Name", "RESRC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 3, "Record Entry Count", "RECNT",
     NITF_VAL_BCS_N_PLUS,
     NITF_NO_RANGE, NULL, NULL},
    {NITF_LOOP, 0, NULL, "RECNT"},
    {NITF_BCS_A, 2, "Engineering Data Label Length", "ENGLN",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 20, "Engineering Data Label", "ENGLBL",
     NITF_VAL_BCS_A_PLUS,
     NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 4, "Engineering Data Matrix Column", "ENGMTXC",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 4, "Engineering Data Matrix Row", "ENGMTXR",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 1, "Value Type of Engineering Data", "ENGTYP",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 1, "Engineering Data Element Size", "ENGDTS",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 2, "Engineering Data Units", "ENGDATU",
     NITF_VAL_BCS_A_PLUS,
     NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 8, "Engineering Data Count", "ENGDATC",
     NITF_VAL_BCS_N_PLUS,
     NITF_NO_RANGE, NULL, NULL},
    {NITF_LOOP, 0, NULL, "ENGMTXR"},
    {NITF_BCS_A, 100, "Engineering Data", "ENGDATA",
     NITF_VAL_BCS_A_PLUS,
     NITF_NO_RANGE, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};*/


/* This test should be used to verify the TRE modifications work.
 * It probably should not be added to a core canon of tests, but
 * it is useful to test any changes with the TRE API
 */
int main(int argc, char **argv)
{
    /*  Get the error object       */
    nitf_Error error;
    NITF_BOOL exists;
    nitf_TRE *dolly;            /* used for clone */

    /* construct a tre */
    nitf_TRE *tre = nitf_TRE_construct("ACFTA", NULL, 154, &error);
    if (!tre)
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }

    /* set the description - this provides a shell for the TRE */
    /*nitf_TRE_setDescription(tre, "ACFTA", NULL, &error);
    if (!tre->descrip)
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }*/

    /* print the TRE -- should print fields, but all blank */
    nitf_TRE_print(tre, &error);

    /* set a value. make sure it went ok */
    printf("Now, set the value of 'AC_MSN_ID' to 'fly-by'. Set ok? ");
    exists = nitf_TRE_setValue(tre, "AC_MSN_ID", "fly-by", 6, &error);
    printf("%s\n", exists ? "yes" : "no");

    printf("Now, print the TRE:\n----------\n");
    /* print the TRE. should print the value now */
    nitf_TRE_print(tre, &error);
    printf("----------\n");

    /* re-set the same value. make sure it went ok */
    printf("Now, re-set the value of 'misid' to 'sky-photo'. Set ok? ");
    exists = nitf_TRE_setValue(tre, "misid", "sky-photo", 9, &error);
    printf("%s\n", exists ? "yes" : "no");

    printf("Now, print the TRE:\n----------\n");
    /* print the TRE. should print the value now */
    nitf_TRE_print(tre, &error);
    printf("----------\n");

    /* make sure that we can't set an invalid tag */
    printf("Try to set an invalid tag. Can we? ");
    exists = nitf_TRE_setValue(tre, "invalid-tag", "sky-photo", 9, &error);
    printf("%s\n", exists ? "yes" : "no");

    printf("Now, print the TRE:\n----------\n");
    /* print the TRE. should print the value now */
    nitf_TRE_print(tre, &error);
    printf("----------\n");

    /* destruct the TRE */
    nitf_TRE_destruct(&tre);


    /******************************************************/
    /* Now, try to create a VALID TRE                     */
    /******************************************************/
    /* construct a tre */
    tre = nitf_TRE_construct("JITCID", NULL, NITF_TRE_DEFAULT_LENGTH, &error);
    if (!tre)
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }
    /*
    nitf_TRE_setDescription(tre, "JITCID", NULL, &error);

    if (!tre->descrip)
    {
        nitf_Error_print(&error, stdout, "Exiting...");
    }
    */
    exists = nitf_TRE_setValue(tre, "FILCMT", "fyi", 3, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");

    /* print the TRE. should print the value now */
    nitf_TRE_print(tre, &error);

    /* validate */
    exists = nitf_TRE_isSane(tre);
    printf("Sane? %s\n", exists ? "yes" : "no");

    /* now, clone it */
    dolly = nitf_TRE_clone(tre, &error);
    if (!dolly)
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }

    printf("About to print the cloned TRE\n");
    nitf_TRE_print(dolly, &error);

    /* validate */
    exists = nitf_TRE_isSane(dolly);
    printf("Sane? %s\n", exists ? "yes" : "no");

    /* destruct the TREs */
    nitf_TRE_destruct(&tre);
    nitf_TRE_destruct(&dolly);


    tre = nitf_TRE_construct("AIMIDB", NULL, NITF_TRE_DEFAULT_LENGTH, &error);
    if (!tre)
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }
    /*
    nitf_TRE_setDescription(tre, "AIMIDB", NULL, &error);
    */
    printf("TRE Length = %d\n", tre->length);
    printf("Computed Length = %d\n", nitf_TRE_computeLength(tre));

    /* destruct the TRE */
    nitf_TRE_destruct(&tre);


    /******************************************************/
    /* Create a TRE, and set the description ourselves!! */
    /******************************************************/
    /* construct a tre */
    tre = nitf_TRE_construct("ENGRDA",
                             NULL,
                             NITF_TRE_DEFAULT_LENGTH, &error);
    if (!tre)
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }
    /*
    if (!nitf_TRE_setDescription(tre, "ENGRDA", NULL, &error))
    {
        printf("Error setting description\n");
    }
    */
    exists =
        nitf_TRE_setValue(tre, "RESRC", "TEST_RESOURCE       ", 20,
                          &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists = nitf_TRE_setValue(tre, "RECNT", "001", 3, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists = nitf_TRE_setValue(tre, "ENGLN[0]", "20", 2, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists =
        nitf_TRE_setValue(tre, "ENGLBL[0]", "CONTROL_POINT       ", 20,
                          &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists = nitf_TRE_setValue(tre, "ENGMTXC[0]", "0100", 4, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists = nitf_TRE_setValue(tre, "ENGMTXR[0]", "0001", 4, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists = nitf_TRE_setValue(tre, "ENGTYP[0]", "A", 1, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists = nitf_TRE_setValue(tre, "ENGDTS[0]", "1", 1, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists = nitf_TRE_setValue(tre, "ENGDATU[0]", "NA", 2, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists = nitf_TRE_setValue(tre, "ENGDATC[0]", "00000100", 8, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists =
        nitf_TRE_setValue(tre, "ENGDATA[0]",
                          "                                                                                                    ",
                          100, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");

    /* print the TRE. should print the value now */
    nitf_TRE_print(tre, &error);

    /* validate */
    exists = nitf_TRE_isSane(tre);
    printf("Sane? %s\n", exists ? "yes" : "no");

    /* destruct the TREs */
    nitf_TRE_destruct(&tre);


    /******************************************************/
    /* Test a TRE that has nested loops                   */
    /******************************************************/
    /* construct a tre */
    tre = nitf_TRE_construct("ACCHZB", NULL, NITF_TRE_DEFAULT_LENGTH, &error);
    if (!tre)
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }
    /*
    if (!nitf_TRE_setDescription(tre, "ACCHZB", NULL, &error))
    {
        printf("Error setting description\n");
    }
    */
    printf("TESTING ACCHZB!!!\n------------------------\n");

    exists = nitf_TRE_setValue(tre, "NUMACHZ", "01", 2, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists = nitf_TRE_setValue(tre, "UNIAAH[0]", "   ", 3, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists = nitf_TRE_setValue(tre, "AAH[0]", "00000", 5, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists = nitf_TRE_setValue(tre, "UNIAPH[0]", "   ", 3, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists = nitf_TRE_setValue(tre, "APH[0]", "00000", 5, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists = nitf_TRE_setValue(tre, "NUMPTS[0]", "001", 3, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists =
        nitf_TRE_setValue(tre, "LON[0][0]", "000000000000000", 15, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");
    exists =
        nitf_TRE_setValue(tre, "LAT[0][0]", "000000000000000", 15, &error);
    printf("Set ok? %s\n", exists ? "yes" : "no");

    /* print the TRE. should print the value now */
    nitf_TRE_print(tre, &error);

    /* validate */
    exists = nitf_TRE_isSane(tre);
    printf("Sane? %s\n", exists ? "yes" : "no");

    /* destruct the TREs */
    nitf_TRE_destruct(&tre);
    
    /* now, let's try to construct based on a description set */
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
            tre = nitf_TRE_construct("ACFTA", infoPtr->description, -1, &error);
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
    }
    
    return 0;
}
