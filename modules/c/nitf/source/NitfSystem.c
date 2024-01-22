#include "nitf/System.h"

NRTAPI(char*) nitf_strdup(const char* src)
{
    if (src != NULL)
    {
        const size_t len = strlen(src);
        char* retval = (char*) NITF_MALLOC(len + 1);
        if (retval != NULL)
        {
            nrt_strcpy_s(retval, len + 1, src);
            retval[len] = '\0';
            return retval;
        }
    }
    return NULL;
}