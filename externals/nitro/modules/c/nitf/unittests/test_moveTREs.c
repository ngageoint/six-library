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


/**
 * This test serves as an example to show how one can construct and write
 * a NITF from scratch.
 */


#include <import/nitf.h>
#include "Test.h"

static const char* const DATE_TIME = "20120126000000";

static
NITF_BOOL insertTRE(nitf_Extensions* ext, nitf_Error* error)
{
  char buffer[1024];

  memset(buffer, 0, 1024);
  nitf_TRE* tre = nitf_TRE_construct("MTXFIL", "MTXFIL", error);

  nitf_TRE_setField(tre, "raw_data", (NITF_DATA*)buffer, 1024, error);
  if (!nitf_Extensions_appendTRE(ext, tre, error))
    return NITF_FAILURE;

  return NITF_SUCCESS;
}

static
NITF_BOOL initializeTREs(nitf_FileHeader* header, nitf_Error* error)
{
  int i = 0;
  char buffer[1024];

  memset(buffer, 0, 1024);

  /* Create a large number of TREs that will force a TRE_OVERFLOW DES */
  for (i = 0; i < 100; ++i)
  {
    insertTRE(header->extendedSection, error);
  }

  return NITF_SUCCESS;
}

static
NITF_BOOL initializeHeader(nitf_FileHeader* header, nitf_Error* error)
{
    return (nitf_Field_setString(header->fileHeader, "NITF", error) &&
            nitf_Field_setString(header->fileVersion, "02.10", error) &&
            nitf_Field_setUint32(header->complianceLevel, 3, error) &&
            nitf_Field_setString(header->systemType, "BF01", error) &&
            nitf_Field_setString(header->originStationID, "mdaus", error) &&
            nitf_Field_setString(header->fileDateTime, DATE_TIME, error) &&
            nitf_Field_setString(header->fileTitle, "TRE Overflow Test", error) &&
            nitf_Field_setString(header->classification, "U", error) &&
            nitf_Field_setUint32(header->encrypted, 0, error) &&
            initializeTREs(header, error));
}


NITF_BOOL unmergeTREs(nitf_Record *record, nitf_Error *error)
{
    if (!nitf_Record_unmergeTREs(record, error))
        goto CATCH_ERROR;

    return NITF_SUCCESS;

  CATCH_ERROR:
    return NITF_FAILURE;
}


TEST_CASE_ARGS(testUnmerge)
{
    nitf_Version version = NITF_VER_21;
    nitf_Record *record = NULL;
    nitf_Error error;
    nitf_Uint32 treSize;

    record = nitf_Record_construct(version, &error);
    TEST_ASSERT(record);
    TEST_ASSERT(initializeHeader(record->header, &error));

    TEST_ASSERT(unmergeTREs(record, &error));

    /* Extension Segment should be less then 99999 now after the unmerge */
    treSize = nitf_Extensions_computeLength(record->header->extendedSection, version, &error);
    TEST_ASSERT(treSize < 99999);

    /* Add another TRE */
    TEST_ASSERT(insertTRE(record->header->extendedSection, &error));

    /* Unmerge again, this should move the new TRE to the existing TRE_OVERFLOW */
    TEST_ASSERT(unmergeTREs(record, &error));

    treSize = nitf_Extensions_computeLength(record->header->extendedSection, version, &error);
    TEST_ASSERT(treSize < 99999);


    nitf_Record_destruct(&record);
}

int main(int argc, char **argv)
{
    CHECK_ARGS(testUnmerge);
    return 0;
}

