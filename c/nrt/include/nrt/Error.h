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

#ifndef __NRT_ERROR_H__
#define __NRT_ERROR_H__

#include "nrt/Defines.h"
#include "nrt/Types.h"

#define  NRT_MAX_EMESSAGE 1024
#define  NRT_CTXT NRT_FILE, NRT_LINE, NRT_FUNC

#ifdef WIN32
#   define NRT_ERRNO GetLastError()
#   define NRT_STRERROR(E) strerror(E)
#else
#   define  NRT_ERRNO errno
#   define  NRT_STRERROR(E) strerror(E)
#endif

NRT_CXX_GUARD
/*
 *
 *  An error level can be low, medium, high, or critical.
 *
 */
enum
{
    NRT_NO_ERR = 0,
    NRT_ERR_MEMORY,
    NRT_ERR_OPENING_FILE,
    NRT_ERR_READING_FROM_FILE,
    NRT_ERR_SEEKING_IN_FILE,
    NRT_ERR_WRITING_TO_FILE,
    NRT_ERR_STAT_FILE,
    NRT_ERR_LOADING_DLL,
    NRT_ERR_UNLOADING_DLL,
    NRT_ERR_RETRIEVING_DLL_HOOK,
    NRT_ERR_UNINITIALIZED_DLL_READ,
    NRT_ERR_INVALID_PARAMETER,
    NRT_ERR_INVALID_OBJECT,
    NRT_ERR_INVALID_FILE,
    NRT_ERR_COMPRESSION,
    NRT_ERR_DECOMPRESSION,
    NRT_ERR_PARSING_FILE,
    NRT_ERR_INT_STACK_OVERFLOW,
    NRT_ERR_UNK
};

/*!
 *  \struct nrt_Error
 *  \brief This structure contains nitf information
 *
 *  The important components of an error are stored here
 */
typedef struct _NRT_Error
{
    char message[NRT_MAX_EMESSAGE + 1];
    char file[NRT_MAX_PATH + 1];
    int line;
    char func[NRT_MAX_PATH + 1];
    int level;
} nrt_Error;

/*!
 *  \fn nrt_Error_init
 *  \brief Initialize an error object (since these arent malloc'ed)
 *
 *  This method sets up an error by initializing it with the
 *  context information, and the error level (which is defineds as
 *  one of these enum values below) and a message.
 *
 *  \param error The error to init
 *  \param message The error message to init (a string)
 *  \param file Context info from NRT_CTXT
 *  \param line Context info from NRT_CTXT
 *  \param func Context info from NRT_CTXT (if C99)
 &  \param level The level of error (This is an enum value)
 */
NRTPROT(void) nrt_Error_init(nrt_Error * error, const char *message,
                             const char *file, int line, const char *func,
                             int level);

/*!
 *  \fn nrt_Error_flogf
 *  \brief Print an existing error to a file handle
 *  \param error An existing error
 *  \param file An open file stream (could be stderr/stdout)
 *  \param level Not necessary [REMOVE ME]
 *  \param format The format string
 */
NRTAPI(void) nrt_Error_flogf(nrt_Error * error, FILE * file, int level,
                             const char *format, ...);

/*!
 *  \fn nrt_Error_printf
 *  \brief Print an existing error to a file handle
 *  \param error An existing error
 *  \param file An open file stream (could be stderr/stdout)
 *  \param format The format string
 */
NRTAPI(void) nrt_Error_fprintf(nrt_Error * error, FILE * file,
                               const char *format, ...);

/*!
 *  \fn nrt_Error_initf
 *  \brief Initialize an error using a format string.  This should only be
 *  done by library functions.
 *  \param file The file nane
 *  \param line The line number
 *  \param func The function (if identifiable)
 *  \param level The type of error (an enum value)
 *  \param format A format string
 */
NRTPROT(void) nrt_Error_initf(nrt_Error * error, const char *file, int line,
                              const char *func, int level, const char *format,
                              ...);

/*!
 *  \fn nrt_Error_print
 *  \brief Print the error to a file stream
 *
 *  \param error The error to print out
 *  \param file An open file stream to write to
 *  \param userMessage Any user message data to be displayed
 */
NRTAPI(void) nrt_Error_print(nrt_Error * error, FILE * file,
                             const char *userMessage);

NRT_CXX_ENDGUARD
#endif
