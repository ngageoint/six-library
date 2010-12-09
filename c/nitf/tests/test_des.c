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

#define SHOW(X) printf("%s=[%s]\n", #X, ((X==0)?"(nul)":X))
#define SHOWI(X) printf("%s=[%ld]\n", #X, X)
#define SHOWLL(X) printf("%s=[%lld]\n", #X, X)
#define SHOWRGB(X) printf("%s(R,G,B)=[%02x,%02x,%02x]\n", #X, (unsigned char) X[0], (unsigned char) X[1], (unsigned char) X[2])
#define SHOW_VAL(X) printf("%s=[%.*s]\n", #X, ((X==0)?8:((X->raw==0)?5:X->length)), ((X==0)?"(nulptr)":((X->raw==0)?"(nul)":X->raw)))


/**
 * This shows the TRE
 *
 */
void showTRE(nitf_HashTable * ht, nitf_Pair * pair, NITF_DATA* userData, nitf_Error * error)
{
    if (pair)
    {
        if (pair->key)
        {
            nitf_ListIterator iter, end;
            printf("\n--------------- %s TRE ---------------\n",
                   pair->key);
            iter = nitf_List_begin(pair->data);
            end = nitf_List_end(pair->data);
            while (nitf_ListIterator_notEqualTo(&iter, &end))
            {
                nitf_TRE *tre = nitf_ListIterator_get(&iter);
                nitf_TRE_print(tre, error);
                nitf_ListIterator_increment(&iter);
            }
            printf("---------------------------------------------\n");
        }
    }
}



/**
 * This shows the security group
 *
 */
void showSecurityGroup(nitf_FileSecurity * securityGroup)
{
    if (!securityGroup)
    {
        printf("ERROR: security group = (nul)\n");
        return;
    }

    SHOW_VAL(securityGroup->classificationSystem);
    SHOW_VAL(securityGroup->codewords);
    SHOW_VAL(securityGroup->controlAndHandling);
    SHOW_VAL(securityGroup->releasingInstructions);
    SHOW_VAL(securityGroup->declassificationType);
    SHOW_VAL(securityGroup->declassificationDate);
    SHOW_VAL(securityGroup->declassificationExemption);
    SHOW_VAL(securityGroup->downgrade);
    SHOW_VAL(securityGroup->downgradeDateTime);
    SHOW_VAL(securityGroup->classificationText);
    SHOW_VAL(securityGroup->classificationAuthorityType);
    SHOW_VAL(securityGroup->classificationAuthority);
    SHOW_VAL(securityGroup->classificationReason);
    SHOW_VAL(securityGroup->securitySourceDate);
    SHOW_VAL(securityGroup->securityControlNumber);
}


/**
 * This shows the DES subheader
 *
 */
void showDESubheader(nitf_DESubheader * sub, nitf_Error * error)
{
    if (!sub)
    {
        printf("ERROR: DES subheader = (null)\n");
        return;
    }
    printf("* * * * * * * * * * * * * * * *\n");
    printf("DES Segment\n");
    printf("* * * * * * * * * * * * * * * *\n");
    SHOW_VAL(sub->filePartType);
    SHOW_VAL(sub->typeID);
    SHOW_VAL(sub->version);
    SHOW_VAL(sub->securityClass);

    if (*(sub->securityClass->raw) != 'U')
        showSecurityGroup(sub->securityGroup);

    SHOW_VAL(sub->overflowedHeaderType);
    SHOW_VAL(sub->dataItemOverflowed);
    SHOW_VAL(sub->subheaderFieldsLength);
    nitf_TRE_print(sub->subheaderFields, error);
    SHOWI(sub->dataLength);

    if (sub->userDefinedSection)
    {
        nitf_HashTable_foreach(sub->userDefinedSection->hash,
                               (NITF_HASH_FUNCTOR) showTRE, NULL, error);
    }
}



/**
 * Main
 *
 */
int main(int argc, char **argv)
{
    nitf_Error error;
    nitf_Reader *reader;
    nitf_Record *record;
    nitf_IOHandle io;
    int num, i;


    if (argc < 2)
    {
        printf("Usage: %s <nitf-file> [specific DES tag(s)]\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    io = nitf_IOHandle_create(argv[1],
                              NITF_ACCESS_READONLY,
                              NITF_OPEN_EXISTING, &error);
    if (NITF_INVALID_HANDLE(io))
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }

    reader = nitf_Reader_construct(&error);
    if (!reader)
    {
        nitf_Error_print(&error, stdout, "Exiting (1) ...");
        exit(EXIT_FAILURE);
    }

    record = nitf_Reader_read(reader, io, &error);
    if (!record)
    {
        nitf_Error_print(&error, stdout, "Exiting (3) ...");
        exit(EXIT_FAILURE);
    }

    if (record->header->numDataExtensions)
    {
        nitf_ListIterator iter = nitf_List_begin(record->dataExtensions);
        nitf_ListIterator end = nitf_List_end(record->dataExtensions);

        if (nitf_ListIterator_equals(&iter, &end))
        {
            printf("There were no DES segments in the file.\n");
        }

        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_DESegment *segment =
                (nitf_DESegment *) nitf_ListIterator_get(&iter);

            if (argc == 2)
            {
                showDESubheader(segment->subheader, &error);
            }
            else
            {
                for (i = 2; i < argc; ++i)
                {
                    if (strncmp
                            (argv[i], segment->subheader->typeID->raw,
                             strlen(argv[i])) == 0)
                    {
                        showDESubheader(segment->subheader, &error);
                        break;
                    }
                }
            }
            nitf_ListIterator_increment(&iter);
        }
    }


    nitf_IOHandle_close(io);
    nitf_Record_destruct(&record);
    nitf_Reader_destruct(&reader);
    return 0;
}
