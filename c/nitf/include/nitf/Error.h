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

#ifndef __NITF_ERROR_H__
#define __NITF_ERROR_H__

#include "nitf/Defines.h"
#include "nitf/Types.h"

#define  NITF_MAX_EMESSAGE 1024
#define  NITF_CTXT NITF_FILE, NITF_LINE, NITF_FUNC

#ifdef WIN32
#  define NITF_ERRNO GetLastError()
#  define NITF_STRERROR(E) strerror(E)
#else
#  define  NITF_ERRNO errno
#  define  NITF_STRERROR(E) strerror(E)
#endif
NITF_CXX_GUARD


/*
 *
 *  An error level can be low, medium, high, or critical.
 *
 */
enum
{
    NITF_NO_ERR = 0,
    NITF_ERR_MEMORY,
    NITF_ERR_OPENING_FILE,
    NITF_ERR_READING_FROM_FILE,
    NITF_ERR_SEEKING_IN_FILE,
    NITF_ERR_WRITING_TO_FILE,
    NITF_ERR_STAT_FILE,
    NITF_ERR_LOADING_DLL,
    NITF_ERR_UNLOADING_DLL,
    NITF_ERR_RETRIEVING_DLL_HOOK,
    NITF_ERR_UNINITIALIZED_DLL_READ,
    NITF_ERR_INVALID_PARAMETER,
    NITF_ERR_INVALID_OBJECT,
    NITF_ERR_INVALID_FILE,
    NITF_ERR_COMPRESSION,
    NITF_ERR_DECOMPRESSION,
    NITF_ERR_PARSING_FILE,
    NITF_ERR_INT_STACK_OVERFLOW,
    NITF_ERR_UNK
};

/*!
 *  \struct nitf_Error
 *  \brief This structure contains nitf information
 *
 *  The important components of an error are stored here
 */
typedef struct _nitf_Error
{
    char message[NITF_MAX_EMESSAGE + 1];
    char file[NITF_MAX_PATH + 1];
    int line;
    char func[NITF_MAX_PATH + 1];
    int level;
}
nitf_Error;



/*!
 *  \fn nitf_Error_init
 *  \brief Initialize an error object (since these arent malloc'ed)
 *
 *  This method sets up an error by initializing it with the
 *  context information, and the error level (which is defineds as
 *  one of these enum values below) and a message.
 *
 *  \param error The error to init
 *  \param message The error message to init (a string)
 *  \param file Context info from NITF_CTXT
 *  \param line Context info from NITF_CTXT
 *  \param func Context info from NITF_CTXT (if C99)
 &  \param level The level of error (This is an enum value)
 */
NITFPROT(void) nitf_Error_init(nitf_Error * error,
                               const char *message,
                               const char *file,
                               int line, const char *func, int level);

/*!
 *  \fn nitf_Error_flogf
 *  \brief Print an existing error to a file handle
 *  \param error An existing error
 *  \param file An open file stream (could be stderr/stdout)
 *  \param level Not necessary [REMOVE ME]
 *  \param format The format string
 */
NITFAPI(void) nitf_Error_flogf(nitf_Error * error,
                               FILE * file,
                               int level, const char *format, ...);

/*!
 *  \fn nitf_Error_printf
 *  \brief Print an existing error to a file handle
 *  \param error An existing error
 *  \param file An open file stream (could be stderr/stdout)
 *  \param format The format string
 */
NITFAPI(void) nitf_Error_fprintf(nitf_Error * error,
                                 FILE * file, const char *format, ...);

/*!
 *  \fn nitf_Error_initf
 *  \brief Initialize an error using a format string.  This should only be
 *  done by library functions.
 *  \param file The file nane
 *  \param line The line number
 *  \param func The function (if identifiable)
 *  \param level The type of error (an enum value)
 *  \param format A format string
 */
NITFPROT(void) nitf_Error_initf(nitf_Error * error,
                                const char *file,
                                int line,
                                const char *func,
                                int level, const char *format, ...);

/*!
 *  \fn nitf_Error_print
 *  \brief Print the error to a file stream
 *
 *  \param error The error to print out
 *  \param file An open file stream to write to
 *  \param userMessage Any user message data to be displayed
 */
NITFAPI(void) nitf_Error_print(nitf_Error * error,
                               FILE * file, const char *userMessage);


NITF_CXX_ENDGUARD

#endif
