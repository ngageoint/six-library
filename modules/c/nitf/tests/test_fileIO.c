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

/*******************************************************************************
**  status = test_large_fileIO(fullFileName)
**
**PURPOSE:
**  This function tests the file IO functions.  It is intended to reveal
**  problems related to 32 vs 64 bit referencing (i.e. try files > 4GB).
**
**INPUT ARGUMENTS:
**  fullFileName    - A C style string indicating the file to test on
**
**RETURN VALUES:
**  status          - An integer indicating the return status.  1 if successful
**                    and 0 otherwise
**
*******************************************************************************/
#include <import/nitf.h>
#include <string.h>

/*******************************************************************************
***************************   Function Prototypes   ****************************
*******************************************************************************/

/********************************************************************************
**PURPOSE:
**  This function parses the input arguments and checks them for errors.  If an
**  error occures 0 is returned and a nitf_Error object is populated.
**
**INPUT ARGUMENTS:
**  argc            - Number of input arguments
**  argv            - Pointer to an array ofinput parameters
**  errorPtr        - Pointer to a nitf error object
**  fullFileName    - Pointer to a pointer to the fullFileName (test file)
**
**RETURN VALUES:
**  status          - An integer indicating the return status.  1 if successful
**                    and 0 otherwise
**
*******************************************************************************/
int parseInputs(int argc, char** argv, nitf_Error* errorPtr, char** fullFileName);


/********************************************************************************
**PURPOSE:
**  This function tests the IO create and close methods.
**
**INPUT ARGUMENTS:
** fullFileName         - Pointer to a c-style string describing the fully
**                        qualified filename
** scratchFullFileName  - Pointer to a c-style string describing the fully
**                        qualified filename of the scratch file
** errorPtr             - Pointer to a nitf error object
**
**RETURN VALUES:
**  status          - An integer indicating the return status.  1 if successful
**                    and 0 otherwise
**
*******************************************************************************/
int checkCreateClose(char* fullFileName, char* scratchFullFileName, nitf_Error* errorPtr);


/********************************************************************************
**PURPOSE:
**  This function tests the getSize method
**
**INPUT ARGUMENTS:
** fullFileName         - Pointer to a c-style string describing the fully
**                        qualified filename
** scratchFullFileName  - Pointer to a c-style string describing the fully
**                        qualified filename of the scratch file
** errorPtr             - Pointer to a nitf error object
**
**RETURN VALUES:
**  fileSize            - An nitf_Off indicating the file size or -1 on failure
**
*******************************************************************************/
nitf_Off checkGetSize(char* fullFileName, char* scratchFullFileName, nitf_Error* errorPtr);


/********************************************************************************
**PURPOSE:
**  This function tests the seek/tell methods
**
**INPUT ARGUMENTS:
** fullFileName         - Pointer to a c-style string describing the fully
**                        qualified filename
** fileSize             - Size of the input file in bytes
** errorPtr             - Pointer to a nitf error object
**
**RETURN VALUES:
**  status          - An integer indicating the return status.  1 if successful
**                    and 0 otherwise
**
*******************************************************************************/
int checkSeekTell(char* fullFileName, nitf_Off fileSize, nitf_Error* errorPtr);
int checkSeekTell_test(nitf_IOHandle ioHandle, nitf_Off fileSize, nitf_Error* errorPtr);


/********************************************************************************
**PURPOSE:
**  This function tests the read/write methods
**
**INPUT ARGUMENTS:
** fullFileName         - Pointer to a c-style string describing the fully
**                        qualified filename
** fileSize             - Size of the input file in bytes
** errorPtr             - Pointer to a nitf error object
**
**RETURN VALUES:
**  status              - 1 if successful and 0 otherwise
**
*******************************************************************************/
int checkReadWrite(char* fullFileName, nitf_Off fileSize, nitf_Error* errorPtr);


/*******************************************************************************
************************************   Main   **********************************
*******************************************************************************/
int main(int argc, char** argv)
{
    /****************************
    *** Variable Declarations ***
    ****************************/
    nitf_Error error;                       /* Error object */
    nitf_Off fileSize;                         /* size in bytes of the test file */
    char* fullFileName;                     /* C style string containing the test file*/
    char scratchFullFileName[1024];          /* C style string for a scratch file to be created */


    /*********************************
    *** Parse the input parameters ***
    *********************************/
    if ( !parseInputs(argc, argv, &error, &fullFileName) )
    {
        printf("Error Parsing Input Parameters: %s\n", error.message);
        exit( EXIT_FAILURE );
    }


    /**********************************************
    *** Get a filename to use as a scratch file ***
    **********************************************/
    strcpy(scratchFullFileName, fullFileName);
    strcat(scratchFullFileName, "_temporary_IOTest_scratch");


    /****************************
    *** Display size of nitf_Off ***
    ****************************/
    printf("Size of nitf_Off: %i bits \n", (int)sizeof(nitf_Off)*8 );


    /********************************
    *** Test create/close methods ***
    ********************************/
    printf("\nTesting create/close methods... \n");
    if ( !checkCreateClose(fullFileName, scratchFullFileName, &error) )
    {
        printf("Error in create/close methods: Further testing has been skipped");
        exit( EXIT_FAILURE );
    }


    /*****************************
    *** Test getSize operation ***
    *****************************/
    printf("\nTesting getSize method... \n");
    fileSize = checkGetSize(fullFileName, scratchFullFileName, &error);
    if ( fileSize == -1)
    {
        printf("Error in getSize method: Further testing has been skipped");
        exit( EXIT_FAILURE );
    }


    /********************************
    *** Test seek/tell operations ***
    ********************************/
    printf("\nTesting seek/tell methods... \n");
    if ( !checkSeekTell(fullFileName, fileSize, &error) )
    {
        printf("Error in seek/tell methods: Further testing has been skipped");
        exit( EXIT_FAILURE );
    }


    /*********************************
    *** Test read/write operations ***
    *********************************/
    printf("\nTesting read/write methods... \n");
    if ( !checkReadWrite(fullFileName, fileSize, &error) )
    {
        printf("Error in read/write methods: Further testing has been skipped");
        exit( EXIT_FAILURE );
    }

    printf("\nAll tests passed. \n\nDon't forget to delete the scratch file: \n%s", scratchFullFileName);
    return( EXIT_SUCCESS );
}


/*******************************************************************************
********************************************************************************
**************************   Function Declarations   ***************************
********************************************************************************
*******************************************************************************/


int parseInputs(int argc, char** argv, nitf_Error* errorPtr, char** fullFileName)
{
    /* Check the number of input arg */
    if ( argc != 2 )
    {
        nitf_Error_init(errorPtr, "Invalid number of input arguments", NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return( 0 );
    }

    /* Assume that the file name is correct (This will be tested during the open) */
    *fullFileName = argv[1];

    return( 1 );
}


int checkCreateClose(char* fullFileName, char* scratchFullFileName, nitf_Error* errorPtr)
{
    /****************************
    *** Variable Declarations ***
    ****************************/
    nitf_IOHandle ioHandle;                 /* IO handle */


    /*********************
    *** Test read-only ***
    *********************/
    ioHandle =  nitf_IOHandle_create(fullFileName,
                                     NITF_OPEN_EXISTING,
                                     NITF_ACCESS_READONLY,
                                     errorPtr);
    if ( NITF_IO_SUCCESS(ioHandle) )
    {
        nitf_IOHandle_close(ioHandle);
        printf("Read-Only Passed \n");
    }
    else
    {
        nitf_Error_print(errorPtr, stdout, "Unable create read-only ioHandle \n");
        return( 0 );
    }


    /**********************
    *** Test write-only ***
    **********************/
    ioHandle =  nitf_IOHandle_create(fullFileName,
                                     NITF_OPEN_EXISTING,
                                     NITF_ACCESS_WRITEONLY, errorPtr);
    if ( NITF_IO_SUCCESS(ioHandle) )
    {
        nitf_IOHandle_close(ioHandle);
        printf("Write-Only Passed \n");
    }
    else
    {
        nitf_Error_print(errorPtr, stdout, "Unable create write-only ioHandle \n");
        return( 0 );
    }


    /**********************
    *** Test read/write ***
    **********************/
    ioHandle =  nitf_IOHandle_create(fullFileName,
                                     NITF_OPEN_EXISTING,
                                     NITF_READWRITE, errorPtr);
    if ( NITF_IO_SUCCESS(ioHandle) )
    {
        nitf_IOHandle_close(ioHandle);
        printf("Read-Write Passed \n");
    }
    else
    {
        nitf_Error_print(errorPtr, stdout, "Unable create read-write ioHandle \n");
        return( 0 );
    }


    /******************
    *** Test create ***
    ******************/
    /* Test the creation of a new file */
    ioHandle =  nitf_IOHandle_create(scratchFullFileName,
                                     NITF_CREATE,
                                     NITF_ACCESS_READONLY,
                                     errorPtr);
    if ( NITF_IO_SUCCESS(ioHandle) )
    {
        nitf_IOHandle_close(ioHandle);
    }
    else
    {
        nitf_Error_print(errorPtr, stdout, "Unable create create ioHandle \n");
        return( 0 );
    }

    /* Test the overwriting of a file */
    ioHandle =  nitf_IOHandle_create(scratchFullFileName,
                                     NITF_CREATE,
                                     NITF_ACCESS_READONLY, errorPtr);
    if ( NITF_IO_SUCCESS(ioHandle) )
    {
        nitf_IOHandle_close(ioHandle);
        printf("Create Passed \n");
    }
    else
    {
        nitf_Error_print(errorPtr, stdout, "Unable create create ioHandle \n");
        return( 0 );
    }

    return( 1 );
}


nitf_Off checkGetSize(char* fullFileName, char* scratchFullFileName, nitf_Error* errorPtr)
{
    /****************************
    *** Variable Declarations ***
    ****************************/
    nitf_IOHandle ioHandle;                 /* IO handle */
    nitf_Off fileSize;
    nitf_Off tempFileSize;


    /***********
    *** Test ***
    ***********/

    /* Open the file read-only */
    ioHandle =  nitf_IOHandle_create(fullFileName,
                                     NITF_OPEN_EXISTING,
                                     NITF_ACCESS_READONLY, errorPtr);
    if ( !NITF_IO_SUCCESS(ioHandle) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable create read-only ioHandle \n");
        return( -1 );
    }

    /* Get the fileSize */
    fileSize = nitf_IOHandle_getSize(ioHandle, errorPtr);

    /* Close the ioHandle */
    nitf_IOHandle_close(ioHandle);

    /* Check the fileSize */
    if ( fileSize < 0 )
    {
        nitf_Error_print(errorPtr, stdout, "Unable get fileSize \n");
        return( -1 );
    }


    /* Open the file write-only */
    ioHandle =  nitf_IOHandle_create(fullFileName,
                                     NITF_OPEN_EXISTING,
                                     NITF_WRITEONLY, errorPtr);
    if ( !NITF_IO_SUCCESS(ioHandle) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable create write-only ioHandle \n");
        return( -1 );
    }

    /* Get the fileSize */
    tempFileSize = nitf_IOHandle_getSize(ioHandle, errorPtr);

    /* Close the ioHandle */
    nitf_IOHandle_close(ioHandle);

    /* Check the fileSize */
    if (tempFileSize != fileSize )
    {
        nitf_Error_print(errorPtr, stdout, "Unable get fileSize \n");
        return( -1 );
    }


    /* Open the file read-write */
    ioHandle =  nitf_IOHandle_create(fullFileName, NITF_READWRITE, errorPtr);
    if ( !NITF_IO_SUCCESS(ioHandle) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable create write-only ioHandle \n");
        return( -1 );
    }

    /* Get the fileSize */
    tempFileSize = nitf_IOHandle_getSize(ioHandle, errorPtr);

    /* Close the ioHandle */
    nitf_IOHandle_close(ioHandle);

    /* Check the fileSize */
    if (tempFileSize != fileSize )
    {
        nitf_Error_print(errorPtr, stdout, "Unable get fileSize \n");
        return( -1 );
    }


    /* Open the file with create */
    ioHandle =  nitf_IOHandle_create(scratchFullFileName,
                                     NITF_ACCESS_READONLY,
                                     NITF_CREATE, errorPtr);
    if ( !NITF_IO_SUCCESS(ioHandle) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable create create ioHandle \n");
        return( -1 );
    }

    /* Get the fileSize */
    tempFileSize = nitf_IOHandle_getSize(ioHandle, errorPtr);

    /* Close the ioHandle */
    nitf_IOHandle_close(ioHandle);

    /* Check the fileSize */
    if (tempFileSize < 0 )
    {
        nitf_Error_print(errorPtr, stdout, "Unable get fileSize \n");
        return( -1 );
    }

    printf("Passed: File Size = %.0lf \n", (double)fileSize );
    return( fileSize );
}


int checkSeekTell(char* fullFileName, nitf_Off fileSize, nitf_Error* errorPtr)
{
    /****************************
    *** Variable Declarations ***
    ****************************/
    nitf_IOHandle ioHandle;                                         /* IO handle */
    int passFlag;


    /*********************
    *** Test read-only ***
    *********************/

    ioHandle =  nitf_IOHandle_create(fullFileName,
                                     NITF_OPEN_EXISTING,
                                     NITF_ACCESS_READONLY, errorPtr);
    if ( !NITF_IO_SUCCESS(ioHandle) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable create read-only ioHandle \n");
        return( 0 );
    }

    passFlag = checkSeekTell_test(ioHandle, fileSize, errorPtr);
    nitf_IOHandle_close(ioHandle);

    if ( !passFlag )
    {
        return( 0 );
    }


    /**********************
    *** Test write-only ***
    **********************/
    ioHandle =  nitf_IOHandle_create(fullFileName,
                                     NITF_OPEN_EXISTING,
                                     NITF_WRITEONLY, errorPtr);
    if ( !NITF_IO_SUCCESS(ioHandle) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable create read-only ioHandle \n");
        return( 0 );
    }

    passFlag = checkSeekTell_test(ioHandle, fileSize, errorPtr);
    nitf_IOHandle_close(ioHandle);

    if ( !passFlag )
    {
        return( 0 );
    }


    /**********************
    *** Test read-write ***
    **********************/
    ioHandle =  nitf_IOHandle_create(fullFileName,
                                     NITF_OPEN_EXISTING,
                                     NITF_ACCESS_READWRITE, errorPtr);
    if ( !NITF_IO_SUCCESS(ioHandle) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable create read-only ioHandle \n");
        return( 0 );
    }

    passFlag = checkSeekTell_test(ioHandle, fileSize, errorPtr);
    nitf_IOHandle_close(ioHandle);

    if ( !passFlag )
    {
        return( 0 );
    }


    printf("Passed \n");
    return( 1 );
}


int checkSeekTell_test(nitf_IOHandle ioHandle, nitf_Off fileSize, nitf_Error* errorPtr)
{

    /* From start of file */
    if ( !NITF_IO_SUCCESS( nitf_IOHandle_seek(ioHandle, 0, NITF_SEEK_SET, errorPtr) ) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to seek in file \n");
        return( 0 );
    }
    if ( nitf_IOHandle_tell(ioHandle, errorPtr) != 0 )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to query file position \n");
        return( 0 );
    }
    if ( !NITF_IO_SUCCESS( nitf_IOHandle_seek(ioHandle, fileSize, NITF_SEEK_SET, errorPtr) ) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to seek in file \n");
        return( 0 );
    }
    if ( nitf_IOHandle_tell(ioHandle, errorPtr) != fileSize )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to query file position \n");
        return( 0 );
    }
    if ( !NITF_IO_SUCCESS( nitf_IOHandle_seek(ioHandle, 0, NITF_SEEK_SET, errorPtr) ) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to seek in file \n");
        return( 0 );
    }
    if ( nitf_IOHandle_tell(ioHandle, errorPtr) != 0 )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to query file position \n");
        return( 0 );
    }

    /* From current position */
    if ( !NITF_IO_SUCCESS( nitf_IOHandle_seek(ioHandle, 0, NITF_SEEK_CUR, errorPtr) ) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to seek in file \n");
        return( 0 );
    }
    if ( nitf_IOHandle_tell(ioHandle, errorPtr) != 0 )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to query file position \n");
        return( 0 );
    }
    if ( !NITF_IO_SUCCESS( nitf_IOHandle_seek(ioHandle, fileSize, NITF_SEEK_CUR, errorPtr) ) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to seek in file \n");
        return( 0 );
    }
    if ( nitf_IOHandle_tell(ioHandle, errorPtr) != fileSize )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to query file position \n");
        return( 0 );
    }
    if ( !NITF_IO_SUCCESS( nitf_IOHandle_seek(ioHandle, -fileSize, NITF_SEEK_CUR, errorPtr) ) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to seek in file \n");
        return( 0 );
    }
    if ( nitf_IOHandle_tell(ioHandle, errorPtr) != 0 )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to query file position \n");
        return( 0 );
    }

    /* From end of file */
    if ( !NITF_IO_SUCCESS( nitf_IOHandle_seek(ioHandle, -fileSize, NITF_SEEK_END, errorPtr) ) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to seek in file \n");
        return( 0 );
    }
    if ( nitf_IOHandle_tell(ioHandle, errorPtr) != 0 )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to query file position \n");
        return( 0 );
    }
    if ( !NITF_IO_SUCCESS( nitf_IOHandle_seek(ioHandle, 0, NITF_SEEK_END, errorPtr) ) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to seek in file \n");
        return( 0 );
    }
    if ( nitf_IOHandle_tell(ioHandle, errorPtr) != fileSize )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to query file position \n");
        return( 0 );
    }
    if ( !NITF_IO_SUCCESS( nitf_IOHandle_seek(ioHandle, -fileSize, NITF_SEEK_END, errorPtr) ) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to seek in file \n");
        return( 0 );
    }
    if ( nitf_IOHandle_tell(ioHandle, errorPtr) != 0 )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to query file position \n");
        return( 0 );
    }

    return( 1 );
}


int checkReadWrite(char* fullFileName, nitf_Off fileSize, nitf_Error* errorPtr)
{
    /****************************
    *** Variable Declarations ***
    ****************************/
#define READ_BUFFER_SIZE 16777216
    int result;                                     /* return status */
    nitf_IOHandle ioHandle;                         /* IO handle */
    char* readBuffer;
    int longReadSize;                               /* Number of bytes in a "long" read */
    nitf_Off longOffset;                               /* Offset to test the 32 bit limit if possible */


    /*******************************
    *** Allocate the read buffer ***
    *******************************/
    readBuffer = NITF_MALLOC(sizeof(char) * READ_BUFFER_SIZE);
    if (readBuffer == NULL)
    {
        printf("UNABLE TO ALLOCATE DYNAMIC MEMORY!");
        exit(0);
    }


    /***********************************************
    *** Open the file with read-write permission ***
    ***********************************************/
    ioHandle = nitf_IOHandle_create(fullFileName, NITF_READWRITE, errorPtr);
    if ( !NITF_IO_SUCCESS(ioHandle) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable create read-write ioHandle \n");
        goto ERROR;
    }


    /*******************************
    *** Determine long read size ***
    *******************************/
    if ( fileSize >= READ_BUFFER_SIZE )
        longReadSize = READ_BUFFER_SIZE;
    else
        longReadSize = fileSize;


    /*******************
    *** Test Reading ***
    *******************/

    /* Read 1 byte from the beginning of the file */
    if ( !NITF_IO_SUCCESS( nitf_IOHandle_read(ioHandle, readBuffer, 1, errorPtr) ) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to read from file \n");
        goto ERROR;
    }

    /* Seek back to the beginning */
    if ( !NITF_IO_SUCCESS( nitf_IOHandle_seek(ioHandle, 0, NITF_SEEK_SET, errorPtr) ) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to seek in file \n");
        goto ERROR;
    }

    /* Make a long read from the beginning of the file */
    if ( !NITF_IO_SUCCESS( nitf_IOHandle_read(ioHandle, readBuffer, longReadSize, errorPtr) ) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to read from file \n");
        goto ERROR;
    }

    /* Look for the 32 bit limit or the end of the file */
    if (fileSize > 2147483648)     /* NOTE: There is typically a sign bit */
        longOffset = 2147483648 - longReadSize + 1;
    else
        longOffset = fileSize - longReadSize;

    /* Seek to longOffset */
    if ( !NITF_IO_SUCCESS( nitf_IOHandle_seek(ioHandle, longOffset, NITF_SEEK_SET, errorPtr) ) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to seek in file \n");
        goto ERROR;
    }

    /* Make a read across the 32 bit limit if possible */
    if ( !NITF_IO_SUCCESS( nitf_IOHandle_read(ioHandle, readBuffer, longReadSize, errorPtr) ) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to read from file \n");
        goto ERROR;
    }

    /* Seek to the end of the file minus 1 byte */
    if ( !NITF_IO_SUCCESS( nitf_IOHandle_seek(ioHandle, -1, NITF_SEEK_END, errorPtr) ) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to seek in file \n");
        goto ERROR;
    }

    /* Read 1 byte from the end of the file */
    if ( !NITF_IO_SUCCESS( nitf_IOHandle_read(ioHandle, readBuffer, 1, errorPtr) ) )
    {
        nitf_Error_print(errorPtr, stdout, "Unable to read from file \n");
        goto ERROR;
    }

    /* We Succeeded! */
    goto SUCCESS;

    /* Handle Any Errors */
ERROR:

    nitf_IOHandle_close(ioHandle);
    result = 0;
    goto EXIT;

    /* Handle Success */
SUCCESS:

    nitf_IOHandle_close(ioHandle);
    result = 1;
    printf("Passed \n");
    goto EXIT;

EXIT:
    NITF_FREE(readBuffer);
    return( result );
}
