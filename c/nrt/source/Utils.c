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
 * License along with this program; if not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include "nrt/Utils.h"

NRTAPI(nrt_List *) nrt_Utils_splitString(char *str, unsigned int max,
                                         nrt_Error * error)
{
    unsigned int count = 0;
    nrt_List *parts;
    char *op, *cur, *end;
    size_t strLen;

    parts = nrt_List_construct(error);
    if (!parts)
        return NULL;

    strLen = strlen(str);
    end = str + strLen;

    op = str;

    if (max == 1)
    {
        char *val = NRT_MALLOC(strLen + 1);
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
            int sz;
            /* skip past white space */
            while (isspace(*op) && op < end)
                ++op;
            cur = op;

            while (!isspace(*op) && op < end)
                ++op;

            if (cur == op)
                break;

            sz = op - cur;
            val = NRT_MALLOC(sz + 1);
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
                    val = NRT_MALLOC(sz + 1);
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

NRTAPI(NRT_BOOL) nrt_Utils_isNumeric(char *str)
{
    char *sp = NULL;
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

NRTAPI(NRT_BOOL) nrt_Utils_isAlpha(char *str)
{
    char *sp = NULL;
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

NRTAPI(NRT_BOOL) nrt_Utils_isBlank(char *str)
{
    char *sp = NULL;
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
    nrt_Uint32 len;             /* Length of the string */
    char *strp;                 /* Pointer into the string */
    nrt_Uint32 i;

    /* strip the end */
    strp = (char *) (str + strlen(str) - 1);
    while (isspace(*strp) && strp != str)
        *(strp--) = 0;

    /* strip the front */
    len = strlen(str);
    strp = str;
    for (i = 0; i < len && isspace(*strp); ++i)
        strp++;
    if (strp != str)
    {
        len = str + len - strp;
        memmove(str, strp, len);
        str[len] = 0;
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
    int i, begin = 0;
    int end = strlen(fullName);
    char *p = strstr(fullName, extension);
    for (i = 0; i < strlen(fullName); i++)
    {
        if (*(fullName + i) == '/' || *(fullName + i) == '\\')
            begin = i + 1;

        if (fullName + i == p)
            end = i - 1;
    }
    memcpy(base, &fullName[begin], end - begin + 1);
    base[end - begin + 1] = 0;
}

NRTAPI(NRT_BOOL) nrt_Utils_parseDecimalString(char *d, double *decimal,
                                              nrt_Error * error)
{
    /* +-dd.ddd or += ddd.ddd */
    int degreeOffset = 0;
    int len = strlen(d);
    char sign = d[0];
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
                        "Invalid decimal string: '%s'. Should be +-dd.ddd or +-ddd.ddd",
                        d);
        return NRT_FAILURE;
    }
    /* Now replace all spaces */
    nrt_Utils_replace(d, ' ', '0');
    *decimal = atof(&(d[1]));

    if (sign == '-')
    {
        *decimal *= -1;
    }

    return NRT_SUCCESS;
}

NRTAPI(double) nrt_Utils_getCurrentTimeMillis()
{
    double millis = 0;
#if defined(__POSIX) && defined(USE_CLOCK_GETTIME)
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    millis = (now.tv_sec + 1.0e-9 * now.tv_nsec) * 1000;
#elif defined(HAVE_SYS_TIME_H)
    struct timeval now;
    gettimeofday(&now, NULL);
    millis = (now.tv_sec + 1.0e-6 * now.tv_usec) * 1000;
#elif defined(WIN32)
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

NRTAPI(int) nrt_Utils_strncasecmp(char *s1, char *s2, size_t n)
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

}

NRTAPI(double) nrt_Utils_geographicToDecimal(int degrees, int minutes,
                                             double seconds)
{
    double decimal = fabs(degrees);
    decimal += ((double) minutes / 60.0);
    decimal += (seconds / 3600.0);

    if (degrees < 0)
    {
        decimal *= -1;
    }

    return decimal;
}

NRTAPI(NRT_BOOL) nrt_Utils_parseGeographicString(char *dms, int *degrees,
                                                 int *minutes, double *seconds,
                                                 nrt_Error * error)
{
    int degreeOffset = 0;
    int len = strlen(dms);
    char dir;

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
    dir = dms[len - 1];
    if (dir != 'N' && dir != 'S' && dir != 'E' && dir != 'W')
    {
        nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_PARAMETER,
                        "Invalid direction: %s. Should be [NSEW]", dms);
        return NRT_FAILURE;
    }

    /* Now replace all spaces */
    nrt_Utils_replace(dms, ' ', '0');

    /* Now get the corners out as geographic coords */
    d[degreeOffset] = 0;
    memcpy(d, dms, degreeOffset);

    memcpy(m, &dms[degreeOffset], 2);
    m[2] = 0;

    memcpy(s, &dms[degreeOffset + 2], 2);
    s[2] = 0;

    *degrees = NRT_ATO32(d);
    *minutes = NRT_ATO32(m);
    *seconds = (double) NRT_ATO32(s);

    if ((degreeOffset == 2 && dir == 'S') || (degreeOffset == 3 && dir == 'W'))
    {
        *degrees *= -1;
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
    default:
        break;
    }
    return cornerRep;
}

NRTPROT(void) nrt_Utils_geographicLatToCharArray(int degrees, int minutes,
                                                 double seconds, char *buffer7)
{
    int wtf = 0;
    char dir = 'N';
    if (degrees < 0)
    {
        dir = 'S';
        degrees *= -1;
    }

    /* Round seconds. */
    seconds += 0.5;

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

    NRT_SNPRINTF(buffer7, 8, "%02d%02d%02d%c", degrees, minutes,
                 (int) seconds, dir);
}

NRTPROT(void) nrt_Utils_geographicLonToCharArray(int degrees, int minutes,
                                                 double seconds, char *buffer8)
{
    char dir = 'E';
    int wtf = 0;
    if (degrees < 0)
    {
        dir = 'W';
        degrees *= -1;
    }

    /* Round seconds. */
    seconds += 0.5;

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

    NRT_SNPRINTF(buffer8, 9, "%03d%02d%02d%c", degrees, minutes,
                 (int) seconds, dir);
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
