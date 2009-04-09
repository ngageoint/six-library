#ifndef __NITF_PLUGIN_IDENTIFIER_H__
#define __NITF_PLUGIN_IDENTIFIER_H__

#define NITF_PLUGIN_INIT_SUFFIX "_init"
#define NITF_PLUGIN_CLEANUP "cleanup"
#define NITF_PLUGIN_HOOK_SUFFIX "_handler"
#define NITF_PLUGIN_CONSTRUCT_SUFFIX "_construct"
#define NITF_PLUGIN_DESTRUCT_SUFFIX "_destruct"

#include "nitf/System.h"
#include "nitf/TRE.h"

/* -- Plugin keys -- */

/*! The TRE plugin key is dependent on the NITF library version. Thus, only
 * plugins built with the same version of the library will be loaded
 */
#define NITF_PLUGIN_TRE_KEY             "TRE:"NITF_LIB_VERSION
#define NITF_PLUGIN_COMPRESSION_KEY     "COMPRESSION"
#define NITF_PLUGIN_DECOMPRESSION_KEY   "DECOMPRESSION"


NITF_CXX_GUARD

struct _nitf_Record;
typedef char **(*NITF_PLUGIN_INIT_FUNCTION) (nitf_Error * error);
typedef void (*NITF_PLUGIN_CLEANUP_FUNCTION) (void);

/*
 * \brief NITF_PLUGIN_TRE_HANDLER_FUNCTION - Function pointer for TRE handler
 *
typedef int (*NITF_PLUGIN_TRE_HANDLER_FUNCTION)
(
    nitf_IOInterface* io,
    nitf_TRE * tre,
    struct _nitf_Record* rec,
    nitf_Error * error
);
*/

typedef nitf_TREHandler* (*NITF_PLUGIN_TRE_HANDLER_FUNCTION)(nitf_Error * error);


/*
  \brief NITF_PLUGIN_COMPRESSION_HANDLER_FUNCTION - Function pointer for
  compression handler

  The function compresses data from the user supplied buffer into a buffer
  allocated by the function. The allocated buffer is returned to the user.
  The function will use the system memory allocation and error handling
  facilities.

  \ar input     - The input buffer
  \ar inputLen  - Length of the input buffer in bytes
  \ar output    - Returns the output buffer
  \ar outputLen - Returns length of the output buffer in bytes
  \ar error     - For error returns

  \return Returns FALSE on error

  On error, the error object is initialized.
*/

typedef int (*NITF_PLUGIN_COMPRESSION_HANDLER_FUNCTION)
(
    nitf_Uint8 *input,
    nitf_Uint32 inputLen,
    nitf_Uint8 **output,
    nitf_Uint32 *outputLen,
    nitf_Error *error
);

/*
  \brief NITF_PLUGIN_DECOMPRESSION_CONSTRUCT_FUNCTION - Function pointer for
  decompression interface object construction.

  The function creates a decompression interface object. The return type is
  void * to avoid a dependency the nitf_ImageIO object. The type is actually
  nitf_DecompressionInterface *

  \ar compressionType - Compression type code
  \ar error           - Error object

  \return Returns the new object or NULL on error.

  On error, the error object is initialized.
*/
typedef void * (*NITF_PLUGIN_DECOMPRESSION_CONSTRUCT_FUNCTION)
(
    const char *compressionType,
    nitf_Error* error
);


typedef void * (*NITF_PLUGIN_COMPRESSION_CONSTRUCT_FUNCTION)
(
    const char *compressionType,
    nitf_Error* error
);

/*
  \brief NITF_PLUGIN_DECOMPRESSION_DESTRUCT_FUNCTION - Function pointer for
  decompression interface object destruction.

  The function destroys a decompression interface object. The input object type
  is void ** to avoid a dependency on the nitf_ImageIO object. The type i
  actually nitf_DecompressionInterface **. The object argument is set to NULL
  on return

  \ar object  - Object to destroy
  \ar error      - Error object

  \return Returns FALSE on error.

  On error, the error object is initialized.
*/
/*
typedef int (*NITF_PLUGIN_DECOMPRESSION_DESTRUCT_FUNCTION)
  (
  void **object,
  nitf_Error* error
  );
*/

NITF_CXX_ENDGUARD

#endif
