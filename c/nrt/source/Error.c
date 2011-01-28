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

#include "nrt/Error.h"

NRTPRIV(void) _NRT_Error_fillString(char *toFill, int maxLength,
                                    const char *from)
{
    int strlenFrom = strlen(from);
    int len = (strlenFrom < maxLength) ? (strlenFrom) : (maxLength);
    memset(toFill, 0, maxLength + 1);
    memcpy(toFill, from, len);
}

NRTPROT(void) nrt_Error_init(nrt_Error * error, const char *message,
                             const char *file, int line, const char *func,
                             int level)
{
    _NRT_Error_fillString(error->message, NRT_MAX_EMESSAGE, message);
    _NRT_Error_fillString(error->file, NRT_MAX_PATH, file);
    _NRT_Error_fillString(error->func, NRT_MAX_PATH, func);

    error->line = line;
    error->level = level;
}

NRTPRIV(const char *) _NRT_Error_urgency(int level)
{
    switch (level)
    {
    case NRT_ERR_MEMORY:
        return "During memory allocation";

    case NRT_ERR_OPENING_FILE:
        return "While opening file";

    case NRT_ERR_READING_FROM_FILE:
        return "While reading from file";

    case NRT_ERR_SEEKING_IN_FILE:
        return "While seeking in file";

    case NRT_ERR_WRITING_TO_FILE:
        return "While writing to file";

    case NRT_ERR_STAT_FILE:
        return "While querying file info";

    case NRT_ERR_LOADING_DLL:
        return "While loading DLL";

    case NRT_ERR_UNLOADING_DLL:
        return "While unloading DLL";

    case NRT_ERR_RETRIEVING_DLL_HOOK:
        return "While retrieving DLL hook";

    case NRT_ERR_UNINITIALIZED_DLL_READ:
        return "Trying to read from uninitialized DLL";

    case NRT_ERR_INVALID_PARAMETER:
        return "Attempt to make use of invalid parameter";

    case NRT_ERR_INVALID_OBJECT:
        return "Trying to perform an operation on an invalid object";

    case NRT_ERR_INVALID_FILE:
        return "Invalid file";

    case NRT_ERR_COMPRESSION:
        return "Invalid compression type";

    case NRT_ERR_DECOMPRESSION:
        return "Invalid decompression";

    }
    return "UNK";
}

void nrt_Error_flogf(nrt_Error * error, FILE * file, int level,
                     const char *format, ...)
{
    va_list args;
    if (error->level < level)
        return;
    va_start(args, format);

    fprintf(file, "Error [%s] (%s, %d, %s): '%s' : ",
            _NRT_Error_urgency(error->level), error->file, error->line,
            error->func,
            (error->level ==
             NRT_ERR_UNK) ? ("Unknown Error") : (error->message));
    vfprintf(file, format, args);
    va_end(args);
}

void nrt_Error_fprintf(nrt_Error * error, FILE * file, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(file, "Error [%s] (%s, %d, %s): '%s' : ",
            _NRT_Error_urgency(error->level), error->file, error->line,
            error->func,
            (error->level ==
             NRT_ERR_UNK) ? ("Unknown Error") : (error->message));
    vfprintf(file, format, args);
    va_end(args);
}

NRTAPI(void) nrt_Error_print(nrt_Error * error, FILE * file,
                             const char *userMessage)
{
    nrt_Error_fprintf(error, file, "%s\n", userMessage);
}

NRTPROT(void) nrt_Error_initf(nrt_Error * error, const char *file, int line,
                              const char *func, int level, const char *format,
                              ...)
{
    va_list args;
    va_start(args, format);

    NRT_VSNPRINTF(error->message, NRT_MAX_EMESSAGE + 1, format, args);
    va_end(args);

    _NRT_Error_fillString(error->file, NRT_MAX_PATH, file);
    _NRT_Error_fillString(error->func, NRT_MAX_PATH, func);

    error->line = line;
    error->level = level;

    return;
}
