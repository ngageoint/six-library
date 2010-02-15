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

#include "nitf/Error.h"

NITFPRIV(void) _nitf_Error_fillString(char *toFill, int maxLength,
                                      const char *from)
{
    int strlenFrom = strlen(from);
    int len = (strlenFrom < maxLength) ? (strlenFrom) : (maxLength);
    memset(toFill, 0, maxLength + 1);
    memcpy(toFill, from, len);
}


NITFPROT(void) nitf_Error_init(nitf_Error * error,
                               const char *message,
                               const char *file,
                               int line, const char *func, int level)
{
    _nitf_Error_fillString(error->message, NITF_MAX_EMESSAGE, message);
    _nitf_Error_fillString(error->file, NITF_MAX_PATH, file);
    _nitf_Error_fillString(error->func, NITF_MAX_PATH, func);

    error->line = line;
    error->level = level;
}


NITFPRIV(const char *) _nitf_Error_urgency(int level)
{
    switch (level)
    {
        case NITF_ERR_MEMORY:
            return "During memory allocation";

        case NITF_ERR_OPENING_FILE:
            return "While opening file";

        case NITF_ERR_READING_FROM_FILE:
            return "While reading from file";

        case NITF_ERR_SEEKING_IN_FILE:
            return "While seeking in file";

        case NITF_ERR_WRITING_TO_FILE:
            return "While writing to file";

        case NITF_ERR_STAT_FILE:
            return "While querying file info";

        case NITF_ERR_LOADING_DLL:
            return "While loading DLL";

        case NITF_ERR_UNLOADING_DLL:
            return "While unloading DLL";

        case NITF_ERR_RETRIEVING_DLL_HOOK:
            return "While retrieving DLL hook";

        case NITF_ERR_UNINITIALIZED_DLL_READ:
            return "Trying to read from uninitialized DLL";

        case NITF_ERR_INVALID_PARAMETER:
            return "Attempt to make use of invalid parameter";

        case NITF_ERR_INVALID_OBJECT:
            return "Trying to perform an operation on an invalid object";

        case NITF_ERR_INVALID_FILE:
            return "Invalid file";

        case NITF_ERR_COMPRESSION:
            return "Invalid compression type";

        case NITF_ERR_DECOMPRESSION:
            return "Invalid decompression";

    }
    return "UNK";
}


void nitf_Error_flogf(nitf_Error * error,
                      FILE * file, int level, const char *format, ...)
{
    va_list args;
    if (error->level < level)
        return;
    va_start(args, format);

    fprintf(file, "Error [%s] (%s, %d, %s): '%s' : ",
            _nitf_Error_urgency(error->level),
            error->file,
            error->line,
            error->func,
            (error->level ==
             NITF_ERR_UNK) ? ("Unknown Error") : (error->message));
    vfprintf(file, format, args);
    va_end(args);
}


void nitf_Error_fprintf(nitf_Error * error,
                        FILE * file, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(file, "Error [%s] (%s, %d, %s): '%s' : ",
            _nitf_Error_urgency(error->level),
            error->file,
            error->line,
            error->func,
            (error->level ==
             NITF_ERR_UNK) ? ("Unknown Error") : (error->message));
    vfprintf(file, format, args);
    va_end(args);
}


NITFAPI(void) nitf_Error_print(nitf_Error * error,
                               FILE * file, const char *userMessage)
{
    nitf_Error_fprintf(error, file, "%s\n", userMessage);
}


NITFPROT(void) nitf_Error_initf(nitf_Error * error,
                                const char *file,
                                int line,
                                const char *func,
                                int level, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    NITF_VSNPRINTF(error->message, NITF_MAX_EMESSAGE + 1, format, args);
    va_end(args);

    _nitf_Error_fillString(error->file, NITF_MAX_PATH, file);
    _nitf_Error_fillString(error->func, NITF_MAX_PATH, func);

    error->line = line;
    error->level = level;

    return;
}
