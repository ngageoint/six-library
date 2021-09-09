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
 * License along with this program; if not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <assert.h>
#include <math.h>

 #include "nrt/Version.h"
#include "nrt/Utils.h"

NRTAPI(nrt_List *) nrt_Utils_splitString(const char *str, unsigned int max,
                                         nrt_Error * error)
{
    unsigned int count = 0;
    nrt_List *parts;
    const char *cur;
    const char* op;
    const char* end;
    size_t strLen;

    parts = nrt_List_construct(error);
    if (!parts)
        return NULL;

    strLen = strlen(str);
    end = str + strLen;

    op = str;

    if (max == 1)
    {
        char *val = (char* )NRT_MALLOC(strLen + 1);
        if (!val)
        {
            nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                           NRT_ERR_MEMORY);
            return NULL;
        }
        memset(val, 0, strLen + 1);
        memcpy(val, str, strLen);
        nrt_List_pushBack(parts, val, error);
    }
    else
    {
        /* strtok is not thread safe */
        while (op < end)
        {
            char *val = NULL;
            size_t sz;
            /* skip past white space */
            while (isspace(*op) && op < end)
                ++op;
            cur = op;

            while (!isspace(*op) && op < end)
                ++op;

            if (cur == op)
                break;

            sz = op - cur;
            val = (char* )NRT_MALLOC(sz + 1);
            if (!val)
            {
                nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                               NRT_ERR_MEMORY);
                return NULL;
            }
            memset(val, 0, sz + 1);
            memcpy(val, cur, sz);
            nrt_List_pushBack(parts, val, error);
            count++;

            /* check the count limit */
            if (max != 0 && count == (max - 1) && op < end)
            {
                /* push on the rest of the string - skip spaces first */
                while (isspace(*op) && op < end)
                    ++op;

                if (op < end)
                {
                    sz = end - op;
                    val = (char* )NRT_MALLOC(sz + 1);
                    if (!val)
                    {
                        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                                       NRT_ERR_MEMORY);
                        return NULL;
                    }
                    memset(val, 0, sz + 1);
                    memcpy(val, op, sz);
                    nrt_List_pushBack(parts, val, error);
                }
                break;
            }
        }
    }

    return parts;
}

NRTAPI(NRT_BOOL) nrt_Utils_isNumeric(const char *str)
{
    const char *sp = NULL;
    if (!str)
        return 0;
    sp = str + strlen(str);
    if (sp <= str)
        return 0;

    do
    {
        if (!isdigit(*(--sp)))
            return 0;
    }
    while (sp > str);
    return 1;
}

NRTAPI(NRT_BOOL) nrt_Utils_isAlpha(const char *str)
{
    const char *sp = NULL;
    if (!str)
        return 0;
    sp = str + strlen(str);
    if (sp <= str)
        return 0;

    do
    {
        if (!isalpha(*(--sp)))
            return 0;
    }
    while (sp > str);
    return 1;
}

NRTAPI(NRT_BOOL) nrt_Utils_isBlank(const char *str)
{
    const char *sp = NULL;
    if (!str)
        return 1;
    sp = str + strlen(str);
    if (sp <= str)
        return 1;

    do
    {
        if (!isspace(*(--sp)))
            return 0;
    }
    while (sp > str);
    return 1;
}

NRTAPI(void) nrt_Utils_trimString(char *str)
{
    size_t len;                 /* Length of the string */
    char* strp;                 /* Pointer into the string */
    size_t i;

    /* strip the end */
    strp = str + (strlen(str) - 1);
    while (isspace(*strp) && strp != str)
        *(strp--) = '\0';

    /* strip the front */
    len = strlen(str);
    strp = str;
    for (i = 0; i < len && isspace(*strp); ++i)
        strp++;
    if (strp != str)
    {
        len = str + len - strp;
        memmove(str, strp, len);
        str[len] = '\0';
    }
    return;
}

NRTPROT(void) nrt_Utils_replace(char *str, char oldValue, char newValue)
{
    char *p;
    while ((p = strchr(str, oldValue)) != NULL)
    {
        *p = newValue;
    }

}

NRTAPI(void) nrt_Utils_baseName(char *base, const char *fullName,
                                const char *extension)
{
    size_t i, begin = 0;
    const size_t len = strlen(fullName);
    size_t end = len;
    const char *p = strstr(fullName, extension);
    for (i = 0; i < len; i++)
    {
        if (*(fullName + i) == '/' || *(fullName + i) == '\\')
            begin = i + 1;

        if (fullName + i == p)
            end = i - 1;
    }
    memcpy(base, &fullName[begin], end - begin + 1);
    base[end - begin + 1] = '\0';
}

NRTAPI(NRT_BOOL) nrt_Utils_parseDecimalString(const char *d, double *decimal,
                                              nrt_Error * error)
{
    if (d == NULL)
    {
        nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_PARAMETER,
                        "Invalid decimal string: <NULL>. Should be +-dd.ddd or +-ddd.ddd");
        return NRT_FAILURE;
    }

    /* +-dd.ddd or += ddd.ddd */
    const size_t len = strlen(d);
    if (len != 7 && len != 8)
    {
        nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_PARAMETER,
                        "Invalid decimal string: '%s'. Should be +-dd.ddd or +-ddd.ddd",
                        d);
        return NRT_FAILURE;
    }
    char* decimalCopy = nrt_strdup(d);
    if (!decimalCopy)
    {
        nrt_Error_initf(error, NRT_CTXT, NRT_ERR_MEMORY,
                        "Could not allocate %zu bytes", len + 1);
        return NRT_FAILURE;
    }

    const char sign = d[0];

    /* Now replace all spaces */
    nrt_Utils_replace(decimalCopy, ' ', '0');
    *decimal = atof(&(decimalCopy[1]));

    if (sign == '-')
    {
        *decimal *= -1;
    }

    free(decimalCopy);
    return NRT_SUCCESS;
}

NRTAPI(double) nrt_Utils_getCurrentTimeMillis(void)
{
    double millis = 0;
#if defined(HAVE_PTHREAD_H) && defined(HAVE_CLOCK_GETTIME)
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    millis = (now.tv_sec + 1.0e-9 * now.tv_nsec) * 1000;
#elif defined(HAVE_SYS_TIME_H)
    struct timeval now;
    gettimeofday(&now, NULL);
    millis = (now.tv_sec + 1.0e-6 * now.tv_usec) * 1000;
#elif defined(WIN32) || defined(_WIN32)
    // Getting time twice may be inefficient but is quicker
    // than converting the SYSTEMTIME structure into milliseconds
    // We could add an additional flag here if the user
    // does not need millisecond accuracy
    SYSTEMTIME now;
    GetLocalTime(&now);
    millis = (double) time(NULL) * 1000 + now.wMilliseconds;
#else
    millis = (double) time(NULL) * 1000;
#endif
    return millis;
}

NRTAPI(int) nrt_Utils_strncasecmp(const char *s1, const char *s2, size_t n)
{
    if (n == 0)
        return 0;

    while (n-- != 0 && tolower(*s1) == tolower(*s2))
    {
        if (n == 0 || *s1 == '\0' || *s2 == '\0')
            break;
        s1++;
        s2++;
    }
    return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
}

NRTAPI(void) nrt_Utils_decimalToGeographic(double decimal, int *degrees,
                                           int *minutes, double *seconds)
{
    double remainder;
    *degrees = (int) decimal;
    remainder = fabs(decimal - (double) *degrees) * 60.0;
    *minutes = (int) remainder;
    *seconds = fabs(remainder - (double) *minutes) * 60.0;

    if (*degrees == 0 && decimal < 0)
    {
        if (*minutes == 0)
        {
            *seconds *= -1;
        }
        else
        {
            *minutes *= -1;
        }
    }
}

NRTAPI(double) nrt_Utils_geographicToDecimal(int degrees, int minutes,
                                             double seconds)
{
    double decimal = fabs(degrees);
    decimal += fabs(minutes / 60.0);
    decimal += fabs(seconds / 3600.0);

    if (degrees < 0 || minutes < 0 || seconds < 0)
    {
        decimal *= -1;
    }

    return decimal;
}

NRTAPI(NRT_BOOL) nrt_Utils_parseGeographicString(const char *dms, int *degrees,
                                                 int *minutes, double *seconds,
                                                 nrt_Error * error)
{
    if (dms == NULL)
    {
        nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_PARAMETER,
                        "Invalid decimal string: <NULL>. Should be ddmmssX or dddmmssY");
        return NRT_FAILURE;
    }

    int degreeOffset = 0;
    const size_t len = strlen(dms);

    char d[4];
    char m[3];
    char s[3];

    /* ddmmssX or dddmmssY */
    if (len == 7)
    {
        degreeOffset = 2;
    }
    else if (len == 8)
    {
        degreeOffset = 3;
    }
    else
    {
        nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_PARAMETER,
                        "Invalid decimal string: %s. Should be ddmmssX or dddmmssY",
                        dms);
        return NRT_FAILURE;
    }
    const char dir = dms[len - 1];
    if (dir != 'N' && dir != 'S' && dir != 'E' && dir != 'W')
    {
        nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_PARAMETER,
                        "Invalid direction: %c. Should be [NSEW]\n"
                        "(Full DMS field is %s)", dir, dms);
        return NRT_FAILURE;
    }

    /* Now replace all spaces */
    char* dmsCopy = nrt_strdup(dms);
    if (!dmsCopy)
    {
        nrt_Error_initf(error, NRT_CTXT, NRT_ERR_MEMORY,
                        "Could not allocate %zu bytes.", strlen(dms) + 1);
        return NRT_FAILURE;
    }
    nrt_Utils_replace(dmsCopy, ' ', '0');

    /* Now get the corners out as geographic coords */
    d[degreeOffset] = 0;
    memcpy(d, dmsCopy, degreeOffset);

    memcpy(m, &dmsCopy[degreeOffset], 2);
    m[2] = 0;

    memcpy(s, &dmsCopy[degreeOffset + 2], 2);
    s[2] = 0;

    *degrees = NRT_ATO32(d);
    *minutes = NRT_ATO32(m);
    *seconds = (double) NRT_ATO32(s);

    if ((degreeOffset == 2 && dir == 'S') || (degreeOffset == 3 && dir == 'W'))
    {
        if (*degrees != 0)
        {
            *degrees *= -1;
        }
        else if (*minutes != 0)
        {
            *minutes *= -1;
        }
        else if (*seconds != 0)
        {
            *seconds *= -1;
        }
        // else everything is 0, so the sign is by default correct
    }

    free(dmsCopy);

    if ((dir == 'N') || (dir == 'S'))
    {
        if (!nrt_Utils_isGeographicLat(*degrees, *minutes, *seconds))
        {
            nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_PARAMETER,
                "Invalid latitude value; full DMS field is '%s'.", dms);
            return NRT_FAILURE;
        }
    }
    else if ((dir == 'E') || (dir == 'W'))
    {
        if (!nrt_Utils_isGeographicLon(*degrees, *minutes, *seconds))
        {
            nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_PARAMETER,
                "Invalid longitude value; full DMS field is '%s'.", dms);
            return NRT_FAILURE;
        }
    }

    return NRT_SUCCESS;
}

NRTAPI(char) nrt_Utils_cornersTypeAsCoordRep(nrt_CornersType type)
{
    char cornerRep = ' ';

    switch (type)
    {
    case NRT_CORNERS_UTM:
        cornerRep = 'U';
        break;

    case NRT_CORNERS_UTM_UPS_S:
        cornerRep = 'S';
        break;

    case NRT_CORNERS_UTM_UPS_N:
        cornerRep = 'N';
        break;

    case NRT_CORNERS_GEO:
        cornerRep = 'G';
        break;

    case NRT_CORNERS_DECIMAL:
        cornerRep = 'D';
        break;

    case NRT_CORNERS_UNKNOWN:
    default:
        break;
    }
    return cornerRep;
}

static NRT_BOOL is_valid_dms(int degrees, int minutes, double seconds)
{
    // these are maximums; values for lat/lon may be smaller
    if (abs(degrees) >= 360)
    {
        return NRT_FALSE;
    }
    if (abs(minutes) >= 60)
    {
        return NRT_FALSE;
    }
    if (fabs(seconds) >= 60.0)
    {
        return NRT_FALSE;
    }

    // A negative value is allowed for the first non-zero value
    if (degrees != 0)
    {
        if ((minutes < 0) || (seconds < 0.0))
        {
            return NRT_FALSE;
        }
    }
    else if (minutes != 0)
    {
        if (seconds < 0.0)
        {
            return NRT_FALSE;
        }
    }

    return NRT_TRUE;
}

NRTAPI(NRT_BOOL) nrt_Utils_isGeographicLat(int degrees, int minutes, double seconds)
{
    if (abs(degrees) > 90)
    {
        return NRT_FALSE;
    }
    return is_valid_dms(degrees, minutes, seconds);
}

NRTPROT(void) nrt_Utils_geographicLatToCharArray(int degrees, int minutes,
                                                 double seconds, char *buffer7)
{
    assert(buffer7 != NULL);
    char dir = 'N';

    if (nrt_Utils_isGeographicLat(degrees, minutes, seconds))
    {
        if (degrees <= 0)
        {
            if (degrees < 0)
            {
                dir = 'S';
                degrees *= -1;
            }
            else if (minutes < 0)
            {
                dir = 'S';
                minutes *= -1;
            }
            else if (minutes == 0 && seconds < 0)
            {
                dir = 'S';
                seconds *= -1;
            }
        }

        seconds = round(seconds);

        /* Ensure seconds and minutes are still within valid range. */
        if (seconds >= 60.0)
        {
            seconds -= 60.0;

            if (++minutes >= 60)
            {
                minutes -= 60;
                ++degrees;
            }
        }
    }
    else
    {
        // setup an error result
        degrees = minutes = 99;
        seconds = 99.0;
    }

    char degrees_buffer[11]; // "2147483647"
    NRT_SNPRINTF(degrees_buffer, 11, "%02d", degrees);
    char minutes_buffer[11];
    NRT_SNPRINTF(minutes_buffer, 11, "%02d", minutes);
    char seconds_buffer[11];
    NRT_SNPRINTF(seconds_buffer, 11, "%02d", (int)seconds);

    NRT_SNPRINTF(buffer7, 8, "%c%c%c%c%c%c%c",
        degrees_buffer[0], degrees_buffer[1],
        minutes_buffer[0], minutes_buffer[1],
        seconds_buffer[0], seconds_buffer[1],
        dir);
}

NRTAPI(NRT_BOOL) nrt_Utils_isGeographicLon(int degrees, int minutes, double seconds)
{
    if (abs(degrees) > 180)
    {
        return NRT_FALSE;
    }
    return is_valid_dms(degrees, minutes, seconds);
}

NRTPROT(void) nrt_Utils_geographicLonToCharArray(int degrees, int minutes,
                                                 double seconds, char *buffer8)
{
    assert(buffer8 != NULL);

    char dir = 'E';

    if (nrt_Utils_isGeographicLon(degrees, minutes, seconds))
    {
        if (degrees <= 0)
        {
            if (degrees < 0)
            {
                dir = 'W';
                degrees *= -1;
            }
            else if (minutes < 0)
            {
                minutes *= -1;
                dir = 'W';
            }
            else if (minutes == 0 && seconds < 0)
            {
                seconds *= -1;
                dir = 'W';
            }
        }

        seconds = round(seconds);

        /* Ensure seconds and minutes are still within valid range. */
        if (seconds >= 60.0)
        {
            seconds -= 60.0;

            if (++minutes >= 60)
            {
                minutes -= 60;
                ++degrees;
            }
        }
    }
    else
    {
        // setup an error result
        degrees = 999;
        minutes = 99;
        seconds = 99.0;
    }

    char degrees_buffer[11]; // "2147483647"
    NRT_SNPRINTF(degrees_buffer, 11, "%03d", degrees);
    char minutes_buffer[11];
    NRT_SNPRINTF(minutes_buffer, 11, "%02d", minutes);
    char seconds_buffer[11];
    NRT_SNPRINTF(seconds_buffer, 11, "%02d", (int)seconds);

    NRT_SNPRINTF(buffer8, 9, "%c%c%c%c%c%c%c%c",
        degrees_buffer[0], degrees_buffer[1], degrees_buffer[2],
        minutes_buffer[0], minutes_buffer[1],
        seconds_buffer[0], seconds_buffer[1],
        dir);
}

NRTPROT(void) nrt_Utils_decimalLatToCharArray(double decimal, char *buffer7)
{
    NRT_SNPRINTF(buffer7, 8, "%+07.3f", decimal);
}

NRTPROT(void) nrt_Utils_decimalLonToCharArray(double decimal, char *buffer8)
{
    NRT_SNPRINTF(buffer8, 9, "%+08.3f", decimal);
}

NRTPROT(void) nrt_Utils_decimalLatToGeoCharArray(double decimal, char *buffer7)
{
    int d, m;
    double s;

    nrt_Utils_decimalToGeographic(decimal, &d, &m, &s);
    nrt_Utils_geographicLatToCharArray(d, m, s, buffer7);
}

NRTPROT(void) nrt_Utils_decimalLonToGeoCharArray(double decimal, char *buffer8)
{
    int d, m;
    double s;

    nrt_Utils_decimalToGeographic(decimal, &d, &m, &s);
    nrt_Utils_geographicLonToCharArray(d, m, s, buffer8);
}

/*!
 * Helper function to actually perform a byte-swap.
 *
 * \param value Pointer to value being swapped
 * \param indexOne Index of first byte to be swapped
 * \param indexTwo Index of second byte to be swapped
 */
/*
 * Older versions of Visual Studio do not support `inline` for C
 * Using `__inline` for Windows instead
 */
NRTPRIV(void)
#if defined(WIN32) || defined(_WIN32)
__inline
#else
inline
#endif
nrt_Utils_swap(uint8_t* value, size_t indexOne,
        size_t indexTwo)
{
    uint8_t temp;
    temp = value[indexOne];
    value[indexOne] = value[indexTwo];
    value[indexTwo] = temp;
}

NRTAPI(void) nrt_Utils_byteSwap(uint8_t *value, size_t size)
{
    switch(size)
    {
    case 2:
        nrt_Utils_swap(value, 0, 1);
        break;

    case 4:
        nrt_Utils_swap(value, 0, 3);
        nrt_Utils_swap(value, 1, 2);
        break;

    case 8:
        nrt_Utils_swap(value, 0, 7);
        nrt_Utils_swap(value, 1, 6);
        nrt_Utils_swap(value, 2, 5);
        nrt_Utils_swap(value, 3, 4);
        break;

    default:
        /* Not handled */
        break;
    }
}

NRTAPI(void) nrt_strcpy_s(char* dest, size_t sz, const char* src)
{
    assert(sz > 0);
    #ifdef _MSC_VER // str*_s() is in C11
    (void) strcpy_s(dest, sz, src);
    #else
   (void)strcpy(dest, src);
    #endif       
}

NRTAPI(void) nrt_strncpy_s(char* dest, size_t dest_sz, const char* src, size_t src_chars)
{
    assert(dest_sz > 0);
    #ifdef _MSC_VER // str*_s() is in C11
    (void) strncpy_s(dest, dest_sz, src, src_chars);
    #else
    (void)strncpy(dest, src, src_chars);
    #endif       
}

NRTAPI(void) nrt_strcat_s(char* dest, size_t sz, const char* src)
{
    assert(sz > 0);
    #ifdef _MSC_VER // str*_s() is in C11
    (void) strcat_s(dest, sz, src);
    #else
    (void) strcat(dest, src);
    #endif
}

NRTAPI(char*) nrt_strdup(const char* src)
{
    if (src != NULL)
    {
        const size_t len = strlen(src);
        char* retval = NRT_MALLOC(len + 1);
        if (retval != NULL)
        {
            nrt_strcpy_s(retval, len + 1, src);
            retval[len] = '\0';
            return retval;
        }
    }
    return NULL;
}

NRTAPI(size_t) nrt_strlen(const char* src)
{
    return strlen(src);
}
NRTAPI(uint32_t) nrt_strlen32(const char* src)
{
    return (uint32_t) strlen(src);
}
NRTAPI(uint16_t) nrt_strlen16(const char* src)
{
    return (uint16_t)strlen(src);
}
NRTAPI(uint8_t) nrt_strlen8(const char* src)
{
    return (uint8_t)strlen(src);
}
