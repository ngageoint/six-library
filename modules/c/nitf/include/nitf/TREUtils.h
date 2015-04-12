#ifndef __NITF_TRE_UTILS_H__
#define __NITF_TRE_UTILS_H__

#include "nitf/TREDescription.h"
#include "nitf/TRE.h"
#include "nitf/TRECursor.h"
#include "nitf/TREPrivateData.h"

NITF_CXX_GUARD

NITFAPI(int) nitf_TREUtils_computeLength(nitf_TRE * tre);

NITFAPI(NITF_BOOL) nitf_TREUtils_setDescription(nitf_TRE* tre,
                                                nitf_Uint32 length,
                                                nitf_Error* error);

NITFAPI(NITF_BOOL) nitf_TREUtils_fillData(nitf_TRE * tre,
                                          const nitf_TREDescription* descrip,
                                          nitf_Error * error);

NITFAPI(NITF_BOOL) nitf_TREUtils_setValue(nitf_TRE * tre,
                                          const char *tag,
                                          NITF_DATA * data,
                                          size_t dataLength, nitf_Error * error);

NITFAPI(NITF_BOOL) nitf_TREUtils_readField(nitf_IOInterface* io,
                                           char *fld,
                                           int length,
                                           nitf_Error * error);

NITFAPI(char *) nitf_TREUtils_getRawData(nitf_TRE * tre,
                                         nitf_Uint32* treLength,
                                         nitf_Error * error);

NITFAPI(NITF_BOOL) nitf_TREUtils_isSane(nitf_TRE * tre);

/*!
 *  Spit out the TRE for debugging purposes
 *  \param tre The TRE
 *  \param desc_ptr The TRE description
 *  \param error The error to populate on failure
 *  \return One on success, zero on failure
 */
NITFAPI(int) nitf_TREUtils_print(nitf_TRE * tre, nitf_Error * error);

NITFAPI(int) nitf_TREUtils_parse(nitf_TRE * tre,
                                 char *bufptr,
                                 nitf_Error * error);

NITFAPI(nitf_TREHandler*)
    nitf_TREUtils_createBasicHandler(nitf_TREDescriptionSet* set,
                                     nitf_TREHandler *handler,
                                     nitf_Error* error);

/*!
 * The "basic" functions used by the basic handler.
 * If you're creating your own handler, you can use these for the functions
 * that you don't need to override
 */

NITFAPI(NITF_BOOL) nitf_TREUtils_basicInit(nitf_TRE * tre,
                                           const char* id,
                                           nitf_Error * error);

NITFAPI(const char*) nitf_TREUtils_basicGetID(nitf_TRE *tre);

NITFAPI(NITF_BOOL) nitf_TREUtils_basicRead(nitf_IOInterface* io,
                                           nitf_Uint32 length,
                                           nitf_TRE* tre,
                                           struct _nitf_Record* record,
                                           nitf_Error* error);

NITFAPI(NITF_BOOL) nitf_TREUtils_basicSetField(nitf_TRE* tre,
                                               const char* tag,
                                               NITF_DATA* data,
                                               size_t dataLength,
                                               nitf_Error* error);

NITFAPI(nitf_Field*) nitf_TREUtils_basicGetField(nitf_TRE* tre,
                                                 const char* tag);

NITFAPI(nitf_List*) nitf_TREUtils_basicFind(nitf_TRE* tre,
                                            const char* pattern,
                                            nitf_Error* error);

NITFAPI(NITF_BOOL) nitf_TREUtils_basicWrite(nitf_IOInterface* io,
                                            nitf_TRE* tre,
                                            struct _nitf_Record* record,
                                            nitf_Error* error);

NITFAPI(nitf_TREEnumerator*) nitf_TREUtils_basicBegin(nitf_TRE* tre,
                                                      nitf_Error* error);

NITFAPI(int) nitf_TREUtils_basicGetCurrentSize(nitf_TRE* tre,
                                               nitf_Error* error);

NITFAPI(NITF_BOOL) nitf_TREUtils_basicClone(nitf_TRE *source,
                                            nitf_TRE *tre,
                                            nitf_Error *error);

NITFAPI(void) nitf_TREUtils_basicDestruct(nitf_TRE* tre);

NITF_CXX_ENDGUARD

#endif
