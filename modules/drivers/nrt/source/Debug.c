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

#include "nrt/Debug.h"

#ifdef NRT_DEBUG

#ifndef WIN32
#    include <unistd.h>
#    include <sys/types.h>
#endif

NRTPROT(void *) nrt_Debug_malloc(const char *file, int line, size_t sz)
{
    /* The pointer to allocate */
    void *p;
    FILE *f;
    char name[512];

#ifndef WIN32
    NRT_SNPRINTF(name, 512, "%s.%d", NRT_MEM_LOG, getpid());
#else
    /* This can easily be modified to use GetCurrentProcessId() */
    strcpy(name, NRT_MEM_LOG);
#endif

    f = fopen(name, "a");
    assert(f);

    fprintf(f, "REQUEST: malloc\t[%d]\n", sz);

    p = malloc(sz);
    fprintf(f, "\tMALLOC\t%p\t%d\t%s\t%d\n", p, sz, file, line);

    fclose(f);
    return p;
}

NRTPROT(void *) nrt_Debug_realloc(const char *file, int line, void *ptr,
                                  size_t sz)
{
    void *p;
    char name[512];
    FILE *f;

#ifndef WIN32
    NRT_SNPRINTF(name, 512, "%s.%d", NRT_MEM_LOG, getpid());
#else
    /* This can easily be modified to use GetCurrentProcessId() */
    strcpy(name, NRT_MEM_LOG);
#endif

    f = fopen(name, "a");
    assert(f);

    fprintf(f, "REQUEST: realloc\t[%p]\t[%d]\n", ptr, sz);
    p = realloc(ptr, sz);
    fprintf(f, "\tREALLOC\t%p\t%p\t%d\t%s\t%d\n", p, ptr, sz, file, line);

    fclose(f);
    return p;
}

NRTPROT(void) nrt_Debug_free(const char *file, int line, void *ptr)
{
    FILE *f;
    char name[512];

#ifndef WIN32
    NRT_SNPRINTF(name, 512, "%s.%d", NRT_MEM_LOG, getpid());
#else
    /* This can easily be modified to use GetCurrentProcessId() */
    strcpy(name, NRT_MEM_LOG);
#endif

    f = fopen(name, "a");
    assert(f);

    fprintf(f, "REQUEST: free\t[%p]\n", ptr);

    free(ptr);

    fprintf(f, "\tFREE\t%s\t%d\n", file, line);

    fclose(f);

}
#endif

NRTPROT(void) nrt_Debug_flogf(FILE * file, const char *format, ...)
{
#if defined(NRT_DEBUG)
    va_list args;
    va_start(args, format);
    vfprintf(file, format, args);
    va_end(args);
#endif
}
