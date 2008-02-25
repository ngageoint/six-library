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

#ifndef __NITF_TRE_H__
#define __NITF_TRE_H__

#include "nitf/System.h"
#include "nitf/HashTable.h"
#include "nitf/Field.h"
#include "nitf/IntStack.h"
#include "nitf/Validation.h" /* need this for the TRE Descriptions, for now */

#define NITF_ETAG_SZ 6
#define NITF_EL_SZ 5


/** This is used in the constructor, to specify
  * that you want to use the default length for
  * the TRE
  */
#define NITF_TRE_DEFAULT_LENGTH 0

/** If the length field of a TREDescription contains this value,
  * it means that the field should be computed based on a previous field value.
  * Any field that uses this for its length MUST be preceded by a NITF_COMP_LEN
  * field.
  */
#define NITF_TRE_CONDITIONAL_LENGTH -100

/** This is used in the TRE Descriptions, for the length field
  * if we want the rest of the data to fill the current field
  */
#define NITF_TRE_GOBBLE -1


NITF_CXX_GUARD

/*!
 * Enumeration defining different ranges
 */
typedef enum _nitf_Range
{
    NITF_NO_RANGE = 0,
    NITF_U32_RANGE,
    NITF_S32_RANGE,
    NITF_U64_RANGE,
    NITF_S64_RANGE
} nitf_Range;

/* Control values that are put in the TREDescription data_type field */
#define NITF_LOOP       NITF_BINARY + 1     /*!< data_type control loop */
#define NITF_ENDLOOP    NITF_LOOP + 1       /*!< data_type control end loop */
#define NITF_IF         NITF_ENDLOOP + 1    /*!< data_type control if */
#define NITF_ENDIF      NITF_IF + 1         /*!< data_type control endif */
#define NITF_COMP_LEN   NITF_ENDIF + 1      /*!< data_type computes the length of the next field */
#define NITF_END        NITF_COMP_LEN + 1   /*!< data_type end marker */


/* This can be used if the user wants to loop a constant number of times */
/* Set the TREDescription label to NITF_CONST_N, and the number in the tag */
/* Example: {NITF_LOOP, 0, NITF_CONST_N, "4096"} */
#define NITF_CONST_N    "CONSTANT"          /*!< TREDescription label value */

/*!
 * The TREDescription structure encapsulates the metadata that describes one
 * field description in a TRE.
 */
typedef struct _nitf_TREDescription
{
    int data_type;                  /*!< the type of field */
    int data_count;                 /*!< the size of the field */
    char *label;                    /*!< description */
    char *tag;                      /*!< unique tag */
    const char *pattern;            /*!< regular expression */
    nitf_Range rangeCategory;       /*!< type of range to apply */
    const char *minValue;           /*!< minimum range value */
    const char *maxValue;           /*!< maximum range value */
} nitf_TREDescription;


#define NITF_TRE_DESC_NO_LENGTH      -1

/*!
 * Information about one TREDescription object
 */
typedef struct _nitf_TREDescriptionInfo
{
    char *name; /*! The name to associate with the Description */
    nitf_TREDescription *description;   /*! The TREDescription */
    int lengthMatch;    /*! The length to match against TREs with; used to choose TREs */
} nitf_TREDescriptionInfo;

/*!
 * Contains a set, or array, of TREDescriptionInfos*
 */
typedef struct _nitf_TREDescriptionSet
{
    int defaultIndex;   /*! Set to the index of the default description */
    nitf_TREDescriptionInfo* descriptions;
} nitf_TREDescriptionSet;

/*!
 * The TRE structure, which contains the description of the TRE, as well as
 * the Field data.
 */
typedef struct _nitf_TRE
{
    size_t length;  /* The length of the tag */
    nitf_TREDescription *descrip;   /* A structure to guide unraveling the data */
    nitf_HashTable *hash;   /* key is field, data is char* pointing at tag[field_off] */
    char tag[NITF_ETAG_SZ + 1]; /* the TRE tag */
} nitf_TRE;


/*!
 *  A structure for keeping track of the
 *  state of the TRE Description when processing it
 *
 *  This is currently public, but it may be a good idea
 *  to move everything related to the nitf_TRECursor
 *  out of the public realm.
 */
typedef struct _nitf_TRECursor
{
    /* DO NOT TOUCH THESE! */
    int numItems;               /* holds the number of descPtr items */
    int index;                  /* holds the current index to process */
    int looping;                /* keeps track of how deep we are in the loops */
    nitf_IntStack *loop;        /* holds loopcount for each level of loops */
    nitf_IntStack *loop_idx;    /* holds the indexes for each level of loops (arrays) */
    nitf_IntStack *loop_rtn;    /* holds the endloop bookmark for each level of loops */
    nitf_TRE *tre;              /* the TRE associated with this cursor */

    /* YOU CAN REFER TO THE MEMBERS BELOW IN YOUR CODE */
    nitf_TREDescription *prev_ptr; /* holds the previous description */
    nitf_TREDescription *desc_ptr;      /* pointer to the current nitf_TREDescription */
    char tag_str[256];          /* holds the fully qualified tag name of the current tag */
    int length;                 /* the length of the field
                                     * This should be used over the TREDescription length because
                                     * the field may need to be computed. This will contain the
                                     * actual, possibly computed length. */
}
nitf_TRECursor;


/*!
 *  Defines a function pointer to be used in conjunction with
 *  the nitf_TRE_foreach function. Users can define their own
 *  "hooks" when looping over the TRE.
 *
 *  \param tre The input TRE
 *  \param dptr The TREDescription of the current Field being processed
 *  \param fieldName The fully qualified name of the Field (including brackets)
 *  \param field The current Field object
 *  \param warningList An optional List to use for keeping track of warnings
 *  \param error The error to populate on failure
 *  \return 1 on success, otherwise 0
 */
typedef int (*NITF_TRE_FUNCTOR) (nitf_TRE * tre,
                                 nitf_TREDescription * dptr,
                                 char *fieldName,
                                 nitf_Field* field,
                                 nitf_List * warningList,
                                 NITF_DATA* userData,
                                 nitf_Error * error);


/*!
 *  This function allows for users to add a hook into the
 *  processing steps of the TRE. The function essentially
 *  performs a cursory loop over the TRE structure, and
 *  calls the passed in function pointer with the appropriate data.
 *
 *  \param tre The input TRE
 *  \param fn The function pointer to the processing function
 *  \param warningList An optional List to use for keeping track of warnings
 *  \param error The error to populate on failure
 *  \return 1 on success, otherwise 0
 */
NITFAPI(NITF_BOOL) nitf_TRE_foreach(nitf_TRE * tre,
                                    NITF_TRE_FUNCTOR fn,
                                    nitf_List * warningList,
                                    NITF_DATA* userData,
                                    nitf_Error * error);

/*!
 *  Initializes the cursor
 *
 *  \param tre The input TRE
 *  \return A cursor for the TRE
 */
NITFAPI(nitf_TRECursor) nitf_TRE_begin(nitf_TRE * tre);


/*!
 *  Returns NITF_BOOL value noting if the cursor has reached the end
 *
 *  \param tre_cursor The TRECursor
 *  \return 1 if done, 0 otherwise
 */
NITFAPI(NITF_BOOL) nitf_TRE_isDone(nitf_TRECursor * tre_cursor);


/*!
 *  Cleans up any allocated members of the TRECursor
 *  This should be called when finished with an cursor
 *
 *  \param tre_cursor The TRECursor
 *  \return 1 if done, 0 otherwise
 */
NITFAPI(void) nitf_TRE_cleanup(nitf_TRECursor * tre_cursor);


/*!
 *  Iterate to the next tag in the TRE.
 *  USERS - Please use the nitf_TRE_foreach function instead of this.
 *
 *  \param tre_cursor The cursor to use
 *  \param error The error to populate on failure
 *  \return NITF_SUCCESS on sucess or NITF_FAILURE otherwise
 */
NITFAPI(int) nitf_TRE_iterate(nitf_TRECursor * tre_cursor,
                              nitf_Error * error);


/*!
 *  Construct a TRE Skeleton (for Reader).
 *  and the offset within the file
 *  \param tag The name of the TRE
 *  \param length This is the length of the TRE data (NITF_TRE_DEFAULT_LENGTH)
 *  \param error The error to populate on failure
 *  \return The newly constructed TRE
 *
 */
NITFPROT(nitf_TRE *) nitf_TRE_createSkeleton(const char* tag,
        int length,
        nitf_Error * error);

/*!
 *  Construct an actual TRE (for users of the library)
 *  \param tag Name of TRE
 *  \param desc a description structure, or if null, uses default for name
 *  \param length The length to construct, or uses default if
 *  NITF_TRE_DEFAULT_LENGTH
 *  \param error Error to populate if something bad happens
 *  \return A new TRE or NULL on failure
 */
NITFAPI(nitf_TRE *) nitf_TRE_construct(const char* tag,
                                       const nitf_TREDescription* descrip,
                                       int length,
                                       nitf_Error * error);



/*!
 *  Clone this object.  This is a deep copy operation.
 *  \todo  This cannot clone the hash table without copying
 *  This is a serious problem since this is the first thing
 *  people will want to do.  Possible solution: put desc inside
 *  of the TRE.  Alternate possibility: call TRE plugin
 *  handler explicitly
 *  \param source The source object
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFAPI(nitf_TRE *) nitf_TRE_clone(nitf_TRE * tre, nitf_Error * error);


/*!
 *  Destroy the TRE.
 *  \param tre The tre to destroy
 */
NITFAPI(void) nitf_TRE_destruct(nitf_TRE ** tre);

/*!
 *  Take in the description and parse the TRE into cooked
 *  hash data
 *  NOTE: The existing hash is emptied first to avoid an unstable TRE
 *  \param  tre      The TRE
 *  \param  desc_ptr The TRE description
 *  \param  bufptr   The buffer
 *  \param  error    The error
 *  \return One on success, zero on failure
 */
NITFAPI(int) nitf_TRE_parse(nitf_TRE * tre,
                            char *bufptr, nitf_Error * error);

/*!
 *  Spit out the TRE for debugging purposes
 *  \param tre The TRE
 *  \param desc_ptr The TRE description
 *  \param error The error to populate on failure
 *  \return One on success, zero on failure
 */
NITFAPI(int) nitf_TRE_print(nitf_TRE * tre, nitf_Error * error);


/*!
 *  Wrapper for the IOHandle.read() function
 *  \param tre The TRE
 *  \param fld The buffer to read the data into
 *  \param length The length of data to read
 *  \param error The error to populate on failure
 *  \return One on success, zero on failure
 */
NITFAPI(NITF_BOOL) nitf_TRE_readField(nitf_IOHandle handle,
                                      char *fld,
                                      int length, nitf_Error * error);

/*!
 * Returns a nitf_TREDescriptionSet* for the specified TRE id.
 * 
 * The resulting set provides TREDescriptions choices for the given TRE tag,
 * which can be used when constructing a TRE from scratch.
 *
 * \param tre_id the id of the tre we want a description for
 * \param error The error to populate on failure
 * \return nitf_TREDescriptionSet* on success, NULL on failure
 */
NITFAPI(nitf_TREDescriptionSet *) nitf_TRE_getDescriptionSet
(
    const char *tre_id,
    nitf_Error *error
);


/*!
 * Returns 1 if the given tag exists in the TRE
 * \param tre the TRE
 * \param tag the tag we want to check for existence
 * \return 1 on success, 0 on failure
 */
NITFAPI(NITF_BOOL) nitf_TRE_exists(nitf_TRE * tre, const char *tag);


/*!
 * Sets the field for the given tag in the tre
 * If the field was never set, a new field is created
 * If the field already existed, the old one is updated
 * \param tre the TRE
 * \param tag the tag we want to check for existence
 * \param data the data to store in the field
 * \param dataLength the length of the data
 * \param error The error to populate on failure
 * \return 1 on success, 0 on failure
 */
NITFAPI(NITF_BOOL) nitf_TRE_setValue(nitf_TRE * tre,
                                     const char *tag,
                                     NITF_DATA * data,
                                     size_t dataLength,
                                     nitf_Error * error);

/*!
 * Returns 1 if the TRE follows the TRE Description
 * and contains fields for each entry that it should,
 * otherwise returns 0
 * \param tre the TRE
 * \return 1 if it is sane, 0 otherwise
 */
NITFAPI(NITF_BOOL) nitf_TRE_isSane(nitf_TRE * tre);


/*!
 * Return a buffer containing the TRE data.
 *
 * This returns the buffer in the format ready for writing. This means that
 * any data that is meant to be in network byte order for writing is converted.
 *
 * The buffer must be freed using NITF_FREE by the caller
 * \param tre The TRE
 * \param length The length of the data, returned
 * \param error The error to populate on failure
 * \return char* buffer on success, NULL on failure
 */
NITFPROT(char *) nitf_TRE_getRawData(nitf_TRE * tre, nitf_Uint32* length, nitf_Error * error);


/*!
 * Returns the length of the TRE
 * This does NOT return the tre->length parameter.
 * This function computes the length based on the tre description
 *
 * CAUTION: If the length was set to NITF_TRE_DEFAULT_LENGTH,
 * then this function could return different values depending on
 * if there are loops and/or conditions in the TRE description.
 * Loops and conditions depend on certain field being present.
 * If the data in those fields changes, this could return a
 * different size at different times. BEWARE!!!!!!!
 *
 * This should only be called when the structure is full
 * and in a "complete" state.
 * \param tre the TRE
 * \return the length of the TRE
 */
NITFPROT(int) nitf_TRE_computeLength(nitf_TRE * tre);


/*!
 * Flushes the contents of the TRE (i.e. flushes the internal hash table)
 * \param tre The TRE
 * \param error The error to populate on failure
 */
NITFPROT(void) nitf_TRE_flush(nitf_TRE * tre, nitf_Error * error);



NITF_CXX_ENDGUARD

#endif
