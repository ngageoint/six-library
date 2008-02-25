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

#include "nitf/TRE.h"
#include "nitf/IntStack.h"
#include "nitf/IOHandle.h"
#include "nitf/PluginRegistry.h"



NITFPRIV(NITF_BOOL) setDescription(nitf_TRE * tre,
                                   const nitf_TREDescription* desc,
                                   nitf_Error * error);


/* IF desc is null, look for it, if I can't load then fail */
/* Length should be defaultable */
NITFAPI(nitf_TRE *) nitf_TRE_createSkeleton(const char* tag,
        int length,
        nitf_Error * error)
{
    nitf_TRE *tre = (nitf_TRE *) NITF_MALLOC(sizeof(nitf_TRE));
    int toCopy = NITF_ETAG_SZ;
    if (!tre)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    /*  Just in case the first malloc fails  */
    tre->hash = NULL;
    tre->descrip = NULL;
    /*  Set the length to the default if it is not defined */
    length = length < 0 ? NITF_TRE_DEFAULT_LENGTH : length;
    tre->length = length;

    /* This happens with things like "DES" */
    if (strlen(tag) < NITF_ETAG_SZ )
    {
        toCopy = strlen(tag);
    }
    memset(tre->tag, 0, NITF_ETAG_SZ + 1);
    memcpy(tre->tag, tag, toCopy);

    /* create the hashtable for the fields */
    tre->hash = nitf_HashTable_construct(NITF_TRE_HASH_SIZE, error);

    if (!tre->hash)
    {
        nitf_TRE_destruct(&tre);
        return NULL;
    }

    /* ??? change policy? */
    nitf_HashTable_setPolicy(tre->hash, NITF_DATA_ADOPT);
    return tre;
}

NITFAPI(nitf_TRE *) nitf_TRE_construct(const char* tag,
                                       const nitf_TREDescription* descrip,
                                       int length,
                                       nitf_Error * error)
{
    nitf_TRE* tre = nitf_TRE_createSkeleton(tag, length, error);
    if (!tre)
        return NULL;

    /* not associated with a description yet */
    if (! setDescription(tre, descrip, error) )
    {
        nitf_TRE_destruct(&tre);
        return NULL;
    }
    return tre;
}


NITFAPI(nitf_TRE *) nitf_TRE_clone(nitf_TRE * source, nitf_Error * error)
{
    nitf_TRE *tre = NULL;

    nitf_List *lPtr;            /* temporary nitf_List pointer */
    nitf_Field *field;          /* temporary nitf_Field pointer */
    nitf_Pair *pair;            /* temporary nitf_Pair pointer */
    nitf_ListIterator iter;     /* iterator to front of list */
    nitf_ListIterator end;      /* iterator to back of list */
    int i;                      /* used for iterating */

    if (source)
    {
        tre = (nitf_TRE *) NITF_MALLOC(sizeof(nitf_TRE));
        if (!tre)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NULL;
        }
        /*  Set this in case of auto-destruct  */
        tre->hash = NULL;

        /*  Set the easy fields  */
        tre->length = source->length;

        /* share the descrip */
        tre->descrip = source->descrip;
        memcpy(tre->tag, source->tag, sizeof(tre->tag));

        tre->hash = nitf_HashTable_construct(NITF_TRE_HASH_SIZE, error);
        if (!tre->hash)
        {
            nitf_TRE_destruct(&tre);
            return NULL;
        }

        /*  Copy the entire contents of the hash  */
        for (i = 0; i < source->hash->nbuckets; i++)
        {
            /*  Foreach chain in the hash table...  */
            lPtr = source->hash->buckets[i];
            iter = nitf_List_begin(lPtr);
            end = nitf_List_end(lPtr);

            /*  And while they are different...  */
            while (nitf_ListIterator_notEqualTo(&iter, &end))
            {
                /*  Retrieve the field at the iterator...  */
                pair = (nitf_Pair *) nitf_ListIterator_get(&iter);

                /*  Cast it back to a field...  */
                field = (nitf_Field *) pair->data;

                /* clone the field */
                field = nitf_Field_clone(field, error);

                /*  If that failed, we need to destruct  */
                if (!field)
                {
                    nitf_TRE_destruct(&tre);
                    return NULL;
                }

                /*  Yes, now we can insert the new field!  */
                if (!nitf_HashTable_insert(tre->hash,
                                           pair->key, field, error))
                {
                    nitf_TRE_destruct(&tre);
                    return NULL;
                }
                nitf_ListIterator_increment(&iter);
            }
        }

        /* ??? change policy? */
        nitf_HashTable_setPolicy(tre->hash, NITF_DATA_ADOPT);
    }
    else
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Trying to clone NULL pointer");
    }
    return tre;
}


/**
 * Helper function for destructing the HashTable pairs
 */
NITFPRIV(int) destructHashValue(nitf_HashTable * ht,
                                nitf_Pair * pair, NITF_DATA* userData,
                                nitf_Error * error)
{
    if (pair)
        if (pair->data)
            nitf_Field_destruct((nitf_Field **) & pair->data);
    return NITF_SUCCESS;
}


NITFAPI(void) nitf_TRE_destruct(nitf_TRE ** tre)
{
    nitf_Error e;
    if (*tre)
    {
        if ((*tre)->hash)
        {
            /* flush the hash */
            nitf_TRE_flush(*tre, &e);

            /* destruct the hash */
            nitf_HashTable_destruct(&((*tre)->hash));
            (*tre)->hash = NULL;
        }
        NITF_FREE(*tre);
        *tre = NULL;
    }
}


NITFAPI(NITF_BOOL) nitf_TRE_readField(nitf_IOHandle handle,
                                      char *fld,
                                      int length, nitf_Error * error)
{
    NITF_BOOL status;

    /* Make sure the field is nulled out  */
    memset(fld, 0, length);

    /* Read from the IO handle */
    status = nitf_IOHandle_read(handle, fld, length, error);
    if (!status)
    {
        nitf_Error_init(error,
                        "Unable to read from IO object",
                        NITF_CTXT, NITF_ERR_READING_FROM_FILE);
    }
    return status;
}


/**
 * Normalizes the tag, which could be in a loop, and returns the nitf_Pair* from
 * the TRE hash that corresponds to the current normalized tag.
 */
NITFPRIV(nitf_Pair *) getTREPair(nitf_TRE * tre,
                                 nitf_TREDescription * desc_ptr,
                                 char idx_str[10][10],
                                 int looping, nitf_Error * error)
{
    char tag_str[256];          /* temp buf used for storing the qualified tag */
    char *bracePtr;             /* pointer for brace position */
    int index = 0;              /* used as in iterator */
    int i;                      /* temp used for looping */
    nitf_Pair *pair = NULL;     /* the pair to return */

    strncpy(tag_str, desc_ptr->tag, sizeof(tag_str));
    /* deal with braces */
    if (strchr(desc_ptr->tag, '['))
    {
        index = 0;
        bracePtr = desc_ptr->tag;
        /* split the string */
        *(strchr(tag_str, '[')) = 0;
        bracePtr--;
        while ((bracePtr = strchr(bracePtr + 1, '[')) != NULL)
        {
            /* tack on the depth */
            strcat(tag_str, idx_str[index++]);
        }
    }
    else
    {
        /* it is dependent on something in another loop,
         * so, we need to figure out what level.
         * since tags are unique, we are ok checking like this */
        pair = nitf_HashTable_find(tre->hash, tag_str);
        for (i = 0; i < looping && !pair; ++i)
        {
            strcat(tag_str, idx_str[i]);
            pair = nitf_HashTable_find(tre->hash, tag_str);
        }
    }
    /* pull the data from the hash about the dependent loop value */
    pair = nitf_HashTable_find(tre->hash, tag_str);
    return pair;
}


NITFPRIV(int) evaluate_if(nitf_TRE * tre,
                          nitf_TREDescription * desc_ptr,
                          char idx_str[10][10],
                          int looping, nitf_Error * error)
{
    nitf_Field *field;
    nitf_Pair *pair;

    char str[256];              /* temp buf for the label */
    char *emptyPtr;
    char *op;
    char *valPtr;

    int status = 0;             /* the return status */
    int fieldData;              /* used as the value for comparing */
    int treData;                /* the value defined int the TRE descrip */

    /* get the data out of the hashtable */
    pair = getTREPair(tre, desc_ptr, idx_str, looping, error);
    if (!pair)
    {
        nitf_Error_init(error, "Unable to find tag in TRE hash",
                        NITF_CTXT, NITF_ERR_UNK);
        return NITF_FAILURE;
    }
    field = (nitf_Field *) pair->data;
    assert(strlen(desc_ptr->label) < sizeof(str));

    strcpy(str, desc_ptr->label);
    op = str;
    while (isspace(*op))
        op++;

    /* split the string at the space */
    emptyPtr = strchr(op, ' ');
    *emptyPtr = 0;

    /* remember where the operand is */
    valPtr = emptyPtr + 1;

    /* check if it is a string comparison of either 'eq' or 'ne' */
    if ((strcmp(op, "eq") == 0) || (strcmp(op, "ne") == 0))
    {
        /* must be a string */
        if (field->type == NITF_BCS_N)
        {
            nitf_Error_init(error,
                            "evaluate: can't use eq/ne to compare a number",
                            NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
            return NITF_FAILURE;
        }
        status = strncmp(field->raw, valPtr, field->length);
        status = strcmp(op, "eq") == 0 ? !status : status;
    }
    /* check if it is a logical operator for ints */
    else if ((strcmp(op, "<") == 0) ||
             (strcmp(op, ">") == 0) ||
             (strcmp(op, ">=") == 0) ||
             (strcmp(op, "<=") == 0) ||
             (strcmp(op, "==") == 0) || (strcmp(op, "!=") == 0))
    {
        /* make sure it is a number */
        if (field->type != NITF_BCS_N)
        {
            nitf_Error_init(error,
                            "evaluate: can't use strings for logical expressions",
                            NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
            return NITF_FAILURE;
        }

        treData = NITF_ATO32(valPtr);
        if (!nitf_Field_get
                (field, (char *) &fieldData, NITF_CONV_INT, sizeof(fieldData),
                 error))
        {
            return NITF_FAILURE;
        }

        /* 0 -> equal, <0 -> less true, >0 greater true */
        status = fieldData - treData;

        if (strcmp(op, ">") == 0)
            status = (status > 0);
        else if (strcmp(op, "<") == 0)
            status = (status < 0);
        else if (strcmp(op, ">=") == 0)
            status = (status >= 0);
        else if (strcmp(op, "<=") == 0)
            status = (status <= 0);
        else if (strcmp(op, "==") == 0)
            status = (status == 0);
        else if (strcmp(op, "!=") == 0)
            status = (status != 0);
    }
    /* otherwise, they used a bad operator */
    else
    {
        nitf_Error_init(error, "evaluate: invalid comparison operator",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }
    return status;
}


/**
 * Helper function for evaluating loops
 * Returns the number of loops that will be processed
 */
NITFPRIV(int) evaluate_loops(nitf_TRE * tre,
                             nitf_TREDescription * desc_ptr,
                             char idx_str[10][10],
                             int looping, nitf_Error * error)
{
    int loops;
    char str[256];              /* temp buf used for manipulating the loop label */
    nitf_Pair *pair;            /* temp nitf_Pair */
    nitf_Field *field;          /* temp nitf_Field */

    char *op;
    char *valPtr;
    int loopVal;                /* used for the possible data in the loop label */

    /* if the user wants a constant value */
    if (desc_ptr->label && strcmp(desc_ptr->label, NITF_CONST_N) == 0)
    {
        loops = NITF_ATO32(desc_ptr->tag);
    }
    else
    {
        pair = getTREPair(tre, desc_ptr, idx_str, looping, error);
        if (!pair)
        {
            nitf_Error_init(error,
                            "evaluate_loops: invalid TRE loop counter",
                            NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
            return NITF_FAILURE;
        }
        field = (nitf_Field *) pair->data;

        /* get the int value */
        if (!nitf_Field_get
                (field, (char *) &loops, NITF_CONV_INT, sizeof(loops), error))
        {
            return NITF_FAILURE;
        }

        /* if the label is not empty, then apply some functionality */
        if (desc_ptr->label && strlen(desc_ptr->label) != 0)
        {
            assert(strlen(desc_ptr->label) < sizeof(str));

            strcpy(str, desc_ptr->label);
            op = str;
            while (isspace(*op))
                op++;
            if ((*op == '+') ||
                    (*op == '-') ||
                    (*op == '*') || (*op == '/') || (*op == '%'))
            {
                valPtr = op + 1;
                while (isspace(*valPtr))
                    valPtr++;

                loopVal = NITF_ATO32(valPtr);

                switch (*op)
                {
                    case '+':
                        loops += loopVal;
                        break;
                    case '-':
                        loops -= loopVal;
                        break;
                    case '*':
                        loops *= loopVal;
                        break;
                    case '/':
                        /* check for divide by zero */
                        if (loopVal == 0)
                        {
                            nitf_Error_init(error,
                                            "evaluate_loops: attempt to divide by zero",
                                            NITF_CTXT,
                                            NITF_ERR_INVALID_PARAMETER);
                            return NITF_FAILURE;
                        }
                        loops /= loopVal;
                        break;
                    case '%':
                        loops %= loopVal;
                        break;
                    default:
                        break;
                }
            }
            else
            {
                nitf_Error_init(error, "evaluate_loops: invalid operator",
                                NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                return NITF_FAILURE;
            }
        }
    }
    return loops < 0 ? 0 : loops;
}


/**
 * Helper function for evaluating loops
 * Returns the number of loops that will be processed
 */
NITFPRIV(int) evaluate_conditionalLength(nitf_TRE * tre,
        nitf_TREDescription * desc_ptr,
        char idx_str[10][10],
        int looping, nitf_Error * error)
{
    int computedLength;
    char str[256];              /* temp buf used for manipulating the loop label */
    nitf_Pair *pair;            /* temp nitf_Pair */
    nitf_Field *field;          /* temp nitf_Field */

    char *op;
    char *valPtr;
    int funcVal;                /* used for the possible data in the description label */

    pair = getTREPair(tre, desc_ptr, idx_str, looping, error);
    if (!pair)
    {
        nitf_Error_init(error,
                        "evaluate_conditionalLength: invalid TRE reference",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }
    field = (nitf_Field *) pair->data;

    /* get the int value */
    if (!nitf_Field_get
            (field, (char *) &computedLength, NITF_CONV_INT,
             sizeof(computedLength), error))
    {
        return NITF_FAILURE;
    }

    /* if the label is not empty, then apply some functionality */
    if (desc_ptr->label && strlen(desc_ptr->label) != 0)
    {
        assert(strlen(desc_ptr->label) < sizeof(str));

        strcpy(str, desc_ptr->label);
        op = str;
        while (isspace(*op))
            op++;
        if ((*op == '+') ||
                (*op == '-') || (*op == '*') || (*op == '/') || (*op == '%'))
        {
            valPtr = op + 1;
            while (isspace(*valPtr))
                valPtr++;

            funcVal = NITF_ATO32(valPtr);

            switch (*op)
            {
                case '+':
                    computedLength += funcVal;
                    break;
                case '-':
                    computedLength -= funcVal;
                    break;
                case '*':
                    computedLength *= funcVal;
                    break;
                case '/':
                    /* check for divide by zero */
                    if (funcVal == 0)
                    {
                        nitf_Error_init(error,
                                        "evaluate_conditionalLength: attempt to divide by zero",
                                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                        return NITF_FAILURE;
                    }
                    computedLength /= funcVal;
                    break;
                case '%':
                    computedLength %= funcVal;
                    break;
                default:
                    break;
            }
        }
        else
        {
            nitf_Error_init(error,
                            "evaluate_conditionalLength: invalid operator",
                            NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
            return NITF_FAILURE;
        }
    }
    return computedLength < 0 ? 0 : computedLength;
}


NITFAPI(nitf_TRECursor) nitf_TRE_begin(nitf_TRE * tre)
{
    nitf_Error error;
    nitf_TRECursor tre_cursor;
    nitf_TREDescription *dptr;

    tre_cursor.loop = nitf_IntStack_construct(&error);
    tre_cursor.loop_idx = nitf_IntStack_construct(&error);
    tre_cursor.loop_rtn = nitf_IntStack_construct(&error);
    tre_cursor.numItems = 0;
    tre_cursor.index = 0;
    tre_cursor.looping = 0;

    if (tre)
    {
        /* set the start index */
        tre_cursor.index = -1;
        /* count how many descriptions there are */
        dptr = tre->descrip;
        while (dptr && (dptr->data_type != NITF_END))
        {
            tre_cursor.numItems++;
            dptr++;
        }
        memset(tre_cursor.tag_str, 0, 256);
        sprintf(tre_cursor.tag_str, "%s", tre->descrip->tag);
        tre_cursor.tre = tre;
    }

    return tre_cursor;
}


NITFAPI(void) nitf_TRE_cleanup(nitf_TRECursor * tre_cursor)
{
    nitf_IntStack_destruct(&tre_cursor->loop);
    nitf_IntStack_destruct(&tre_cursor->loop_idx);
    nitf_IntStack_destruct(&tre_cursor->loop_rtn);
}


NITFAPI(NITF_BOOL) nitf_TRE_isDone(nitf_TRECursor * tre_cursor)
{
    int status = 1;
    int i = 0;
    int gotField = 0;
    nitf_Error error;
    NITF_BOOL iterStatus = NITF_SUCCESS;
    nitf_TRECursor cursor = nitf_TRE_begin(tre_cursor->tre);

    /* check if the passed in cursor just began */
    if (tre_cursor->index < 0)
        status = 0;

    /* first check all loops to see if we are in the middle of one */
    for (i = 0; status && (i < tre_cursor->looping); ++i)
    {
        if (tre_cursor->loop_idx->st[i] < tre_cursor->loop->st[i])
        {
            status = 0;
        }
    }

    /* try iterating and see if we make it to the end */
    while (status && (cursor.index < cursor.numItems) && iterStatus == NITF_SUCCESS)
    {
        iterStatus = nitf_TRE_iterate(&cursor, &error);

        if (gotField)
        {
            /* we got to the next field... so return not done */
            status = 0;
        }
        else if (strcmp(tre_cursor->tag_str, cursor.tag_str) == 0)
        {
            gotField = 1;
        }
    }

    /* and the current status with a check to see if we reached the end */
    status = (status & (cursor.index >= cursor.numItems)) | !iterStatus;
    nitf_TRE_cleanup(&cursor);
    return status;
}


NITFAPI(int) nitf_TRE_iterate(nitf_TRECursor * tre_cursor,
                              nitf_Error * error)
{
    nitf_TREDescription *dptr;

    int *stack;                 /* used for in conjuction with the stacks */
    int index;                  /* used for in conjuction with the stacks */

    int loopCount = 0;          /* tells how many times to loop */
    int loop_rtni = 0;          /* used for temp storage */
    int loop_idxi = 0;          /* used for temp storage */

    int numIfs = 0;             /* used to keep track of nested Ifs */
    int numLoops = 0;           /* used to keep track of nested Loops */
    int done = 0;               /* flag used for special cases */

    char idx_str[10][10];       /* used for keeping track of indexes */

    if (!tre_cursor->loop || !tre_cursor->loop_idx
            || !tre_cursor->loop_rtn)
    {
        /* not initialized */
        nitf_Error_init(error, "Unhandled TRE Value data type",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }

    /* count how many descriptions there are */
    dptr = tre_cursor->tre->descrip;

    while (!done)
    {
        done = 1;               /* set the flag */

        /* iterate the index */
        tre_cursor->index++;

        if (tre_cursor->index < tre_cursor->numItems)
        {
            memset(tre_cursor->tag_str, 0, 256);

            tre_cursor->prev_ptr = tre_cursor->desc_ptr;
            tre_cursor->desc_ptr = &dptr[tre_cursor->index];

            /* if already in a loop, prepare the array of values */
            if (tre_cursor->looping)
            {
                stack = tre_cursor->loop_idx->st;
                /* assert, because we only prepare for 10 */
                assert(tre_cursor->looping <= 10);

                for (index = 0; index < tre_cursor->looping; index++)
                {
                    sprintf(idx_str[index], "[%d]", stack[index]);
                }
            }

            /* check if it is an actual item now */
            /* ASCII string */
            if ((tre_cursor->desc_ptr->data_type == NITF_BCS_A) ||
                    /* ASCII number */
                    (tre_cursor->desc_ptr->data_type == NITF_BCS_N) ||
                    /* raw bytes */
                    (tre_cursor->desc_ptr->data_type == NITF_BINARY))
            {
                sprintf(tre_cursor->tag_str, "%s",
                        tre_cursor->desc_ptr->tag);
                /* check if data is part of an array */
                if (tre_cursor->looping)
                {
                    stack = tre_cursor->loop_idx->st;
                    for (index = 0; index < tre_cursor->looping; index++)
                    {
                        char entry[64];
                        sprintf(entry, "[%d]", stack[index]);
                        strcat(tre_cursor->tag_str, entry);
                    }
                }
                /* check to see if we don't know the length */
                if (tre_cursor->desc_ptr->data_count ==
                        NITF_TRE_CONDITIONAL_LENGTH)
                {
                    /* compute the length given by the previous field */
                    tre_cursor->length =
                        evaluate_conditionalLength(tre_cursor->tre,
                                                   tre_cursor->prev_ptr,
                                                   idx_str,
                                                   tre_cursor->looping,
                                                   error);
                    /* This check was added because of this scenario:
                     * What if the value that the conditional length is based upon
                     * happens to be zero??? That means that technically the field
                     * should not exist. Since we enforce fields to be of size > 0,
                     * then we will just take care of this scenario now, and just
                     * iterate the cursor.
                     */
                    if (tre_cursor->length == 0)
                    {
                        return nitf_TRE_iterate(tre_cursor, error);
                    }
                }
                else
                {
                    /* just set the length that was in the TREDescription */
                    tre_cursor->length = tre_cursor->desc_ptr->data_count;
                }
            }
            /* NITF_LOOP, NITF_IF, etc. */
            else if ((tre_cursor->desc_ptr->data_type >=
                      NITF_LOOP)
                     && (tre_cursor->desc_ptr->data_type < NITF_END))
            {
                done = 0;       /* set the flag */

                /* start of a loop */
                if (tre_cursor->desc_ptr->data_type == NITF_LOOP)
                {
                    loopCount =
                        evaluate_loops(tre_cursor->tre,
                                       tre_cursor->desc_ptr, idx_str,
                                       tre_cursor->looping, error);
                    if (loopCount > 0)
                    {
                        tre_cursor->looping++;
                        /* record loopcount in @loop stack */
                        nitf_IntStack_push(tre_cursor->loop, loopCount,
                                           error);
                        /* record i in @loop_rtn stack */
                        nitf_IntStack_push(tre_cursor->loop_rtn,
                                           tre_cursor->index, error);
                        /* record a 0 in @loop_idx stack */
                        nitf_IntStack_push(tre_cursor->loop_idx, 0, error);
                    }
                    else
                    {
                        numLoops = 1;
                        /* skip until we see the matching ENDLOOP */
                        while (numLoops
                                && (++tre_cursor->index <
                                    tre_cursor->numItems))
                        {
                            tre_cursor->desc_ptr =
                                &dptr[tre_cursor->index];
                            if (tre_cursor->desc_ptr->data_type ==
                                    NITF_LOOP)
                                numLoops++;
                            else if (tre_cursor->desc_ptr->data_type ==
                                     NITF_ENDLOOP)
                                numLoops--;
                        }
                    }
                }
                /* end of a loop */
                else if (tre_cursor->desc_ptr->data_type == NITF_ENDLOOP)
                {
                    /* retrieve loopcount from @loop stack */
                    loopCount = nitf_IntStack_pop(tre_cursor->loop, error);
                    /* retrieve loop_rtn from @loop_rtn stack */
                    loop_rtni =
                        nitf_IntStack_pop(tre_cursor->loop_rtn, error);
                    /* retrieve loop_idx from @loop_idx stack */
                    loop_idxi =
                        nitf_IntStack_pop(tre_cursor->loop_idx, error);

                    if (--loopCount > 0)
                    {
                        /* record loopcount in @loop stack */
                        nitf_IntStack_push(tre_cursor->loop, loopCount,
                                           error);
                        /* record i in @loop_rtn stack */
                        nitf_IntStack_push(tre_cursor->loop_rtn, loop_rtni,
                                           error);
                        /* record idx in @loop_idx stack */
                        nitf_IntStack_push(tre_cursor->loop_idx,
                                           ++loop_idxi, error);
                        /* jump to the start of the loop */
                        tre_cursor->index = loop_rtni;
                    }
                    else
                    {
                        --tre_cursor->looping;
                    }
                }
                /* an if clause */
                else if (tre_cursor->desc_ptr->data_type == NITF_IF)
                {
                    if (!evaluate_if
                            (tre_cursor->tre, tre_cursor->desc_ptr, idx_str,
                             tre_cursor->looping, error))
                    {
                        numIfs = 1;
                        /* skip until we see the matching ENDIF */
                        while (numIfs
                                && (++tre_cursor->index <
                                    tre_cursor->numItems))
                        {
                            tre_cursor->desc_ptr =
                                &dptr[tre_cursor->index];
                            if (tre_cursor->desc_ptr->data_type == NITF_IF)
                                numIfs++;
                            else if (tre_cursor->desc_ptr->data_type ==
                                     NITF_ENDIF)
                                numIfs--;
                        }
                    }
                }
                /* compute the length */
                else if (tre_cursor->desc_ptr->data_type == NITF_COMP_LEN)
                {
                    /*do nothing */
                }
            }
            else
            {
                nitf_Error_init(error, "Unhandled TRE Value data type",
                                NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                return NITF_FAILURE;
            }
        }
        else
        {
            /* should return FALSE instead. signifies we are DONE iterating! */
            return NITF_FAILURE;
        }
    }
    return NITF_SUCCESS;
}


NITFPRIV(NITF_BOOL) setDescription(nitf_TRE * tre,
                                   const nitf_TREDescription* descrip,
                                   nitf_Error * error)
{
    int bad = 0;
    NITF_PLUGIN_TRE_SET_DESCRIPTION_FUNCTION function =
        (NITF_PLUGIN_TRE_SET_DESCRIPTION_FUNCTION) NULL;

    nitf_PluginRegistry *reg;
    nitf_Pair *pair;            /* temp nitf_Pair */
    nitf_Field *field;          /* temp nitf_Field */
    nitf_TRECursor cursor;      /* the cursor for looping the TRE */
    char *tempBuf;              /* temp buffer */

    /* get out if TRE is null */
    if (!tre)
    {
        nitf_Error_init(error, "setDescription -> invalid tre object",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        goto CATCH_ERROR;
    }

    /* see if the description needs to be looked up in the registry */
    if (!descrip)
    {
        reg = nitf_PluginRegistry_getInstance(error);
        if (reg)
        {
            function = nitf_PluginRegistry_retrieveSetTREDescription(reg,
                tre->tag, &bad, error);
            if (bad)
                goto CATCH_ERROR;
        }

        if (function == (NITF_PLUGIN_TRE_SET_DESCRIPTION_FUNCTION) NULL)
        {
            /* try the default TRE handler */
            /*handler = (NITF_PLUGIN_TRE_DESCRIPTION_FUNCTION) nitf_DefaultTRE_handler; */
            /* for now, just error out */
            nitf_Error_init(error,
                            "TRE Set Description handle is NULL",
                            NITF_CTXT, NITF_ERR_INVALID_OBJECT);
            goto CATCH_ERROR;
        }
        
        (*function)(tre, error);
        if (!tre->descrip)
        {
            /*nitf_Error_init(error, "TRE Description is NULL",
                            NITF_CTXT, NITF_ERR_INVALID_OBJECT);*/
            goto CATCH_ERROR;
        }
    }
    else
    {
        tre->descrip = (nitf_TREDescription *) descrip;
    }

    /* loop over the description, and add blank fields for the
     * "normal" fields... any special case fields (loops, conditions)
     * won't be added here
     */
    cursor = nitf_TRE_begin(tre);
    while (!nitf_TRE_isDone(&cursor))
    {
        if (nitf_TRE_iterate(&cursor, error))
        {
            pair = nitf_HashTable_find(tre->hash, cursor.tag_str);
            if (!pair || !pair->data)
            {
                field = nitf_Field_construct(cursor.length,
                                             cursor.desc_ptr->data_type,
                                             error);

                /* special case if BINARY... must set Raw Data */
                if (cursor.desc_ptr->data_type == NITF_BINARY)
                {
                    tempBuf = (char *) NITF_MALLOC(cursor.length);
                    if (!tempBuf)
                    {
                        nitf_Field_destruct(&field);
                        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                                        NITF_CTXT, NITF_ERR_MEMORY);
                        goto CATCH_ERROR;
                    }

                    memset(tempBuf, 0, cursor.length);
                    nitf_Field_setRawData(field, (NITF_DATA *) tempBuf,
                                          cursor.length, error);
                }
                else
                {
                    /* this will get zero/blank filled by the function */
                    nitf_Field_setString(field, "", error);
                }

                /* add to hash if there wasn't an entry yet */
                if (!pair)
                {
                    nitf_HashTable_insert(tre->hash,
                                          cursor.tag_str, field, error);
                }
                /* otherwise, just set the data pointer */
                else
                {
                    pair->data = (NITF_DATA *) field;
                }
            }
        }
    }
    nitf_TRE_cleanup(&cursor);

    /* no problems */
    /*    return tre->descrip; */
    return NITF_SUCCESS;
CATCH_ERROR:
    return NITF_FAILURE;
}


NITFAPI(int) nitf_TRE_print(nitf_TRE * tre, nitf_Error * error)
{
    nitf_Pair *pair;            /* temp pair */
    int status = NITF_SUCCESS;
    nitf_TRECursor cursor;

    /* get out if TRE is null */
    if (!tre)
    {
        nitf_Error_init(error, "print -> invalid tre object",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }

    cursor = nitf_TRE_begin(tre);
    while (!nitf_TRE_isDone(&cursor) && (status == NITF_SUCCESS))
    {
        if ((status = nitf_TRE_iterate(&cursor, error)) == NITF_SUCCESS)
        {
            pair = nitf_HashTable_find(tre->hash, cursor.tag_str);
            if (!pair || !pair->data)
            {
                nitf_Error_initf(error, NITF_CTXT, NITF_ERR_UNK,
                                 "Unable to find tag, '%s', in TRE hash for TRE '%s'", cursor.tag_str, tre->tag);
                status = NITF_FAILURE;
            }
            else
            {
                printf("%s (%s) = [",
                       cursor.desc_ptr->label == NULL ?
                       "null" : cursor.desc_ptr->label, cursor.tag_str);
                nitf_Field_print((nitf_Field *) pair->data);
                printf("]\n");
            }
        }
    }
    nitf_TRE_cleanup(&cursor);
    return status;
}


NITFAPI(NITF_BOOL) nitf_TRE_exists(nitf_TRE * tre, const char *tag)
{
    return (!tre) ? NITF_FAILURE : nitf_HashTable_exists(tre->hash, tag);
}


NITFAPI(NITF_BOOL) nitf_TRE_setValue(nitf_TRE * tre,
                                     const char *tag,
                                     NITF_DATA * data,
                                     size_t dataLength, nitf_Error * error)
{
    nitf_Pair *pair;
    nitf_Field *field = NULL;
    nitf_TRECursor cursor;
    NITF_BOOL done = 0;
    NITF_BOOL status = 1;
    nitf_FieldType type = NITF_BCS_A;
    int length;                 /* used temporarily for storing the length */

    /* get out if TRE is null */
    if (!tre)
    {
        nitf_Error_init(error, "setValue -> invalid tre object",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }

    /* If the field already exists, get it and modify it */
    if (nitf_HashTable_exists(tre->hash, tag))
    {
        pair = nitf_HashTable_find(tre->hash, tag);
        field = (nitf_Field *) pair->data;

        if (!field)
        {
            nitf_Error_init(error, "setValue -> invalid field object",
                            NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
            return NITF_FAILURE;
        }

        /* check to see if the data passed in is too large or too small */
        if (dataLength > field->length || dataLength < 1)
        {
            nitf_Error_init(error, "setValue -> invalid dataLength",
                            NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
            return NITF_FAILURE;
        }

        if (!nitf_Field_setRawData
                (field, (NITF_DATA *) data, dataLength, error))
        {
            return NITF_FAILURE;
        }
    }
    /* it doesn't exist in the hash yet, so we need to find it */
    else
    {
        cursor = nitf_TRE_begin(tre);
        while (!nitf_TRE_isDone(&cursor) && !done && status)
        {
            if (nitf_TRE_iterate(&cursor, error) == NITF_SUCCESS)
            {
                /* we found it */
                if (strcmp(tag, cursor.tag_str) == 0)
                {
                    if (cursor.desc_ptr->data_type == NITF_BCS_A)
                    {
                        type = NITF_BCS_A;
                    }
                    else if (cursor.desc_ptr->data_type == NITF_BCS_N)
                    {
                        type = NITF_BCS_N;
                    }
                    else if (cursor.desc_ptr->data_type == NITF_BINARY)
                    {
                        type = NITF_BINARY;
                    }
                    else
                    {
                        /* bad type */
                        nitf_Error_init(error,
                                        "setValue -> invalid data type",
                                        NITF_CTXT,
                                        NITF_ERR_INVALID_PARAMETER);
                        return NITF_FAILURE;
                    }

                    length = cursor.length;
                    /* check to see if we should gobble the rest */
                    if (length == NITF_TRE_GOBBLE)
                    {
                        length = dataLength;
                    }

                    /* construct the field */
                    field = nitf_Field_construct(length, type, error);

                    /* now, set the data */
                    nitf_Field_setRawData(field, (NITF_DATA *) data,
                                          dataLength, error);

#ifdef NITF_DEBUG
                    fprintf(stdout, "Adding Field [%s] to TRE [%s]\n",
                            cursor.tag_str, tre->tag);
#endif

                    /* add to the hash */
                    nitf_HashTable_insert(tre->hash, cursor.tag_str, field,
                                          error);
                    done = 1;   /* set, so we break out of loop */
                }
            }
        }
        /* did we find it? */
        if (!done)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_UNK,
                             "Unable to find tag, '%s', in TRE hash for TRE '%s'",
                             tag, tre->tag);
            status = 0;
        }
        nitf_TRE_cleanup(&cursor);
    }
    return status;
}


NITFAPI(NITF_BOOL) nitf_TRE_isSane(nitf_TRE * tre)
{
    int status = 1;
    nitf_Error error;
    nitf_TRECursor cursor;

    /* get out if TRE is null */
    if (!tre)
        return NITF_FAILURE;

    cursor = nitf_TRE_begin(tre);
    while (!nitf_TRE_isDone(&cursor) && status)
    {
        if (nitf_TRE_iterate(&cursor, &error) == NITF_SUCCESS)
            if (!nitf_TRE_exists(tre, cursor.tag_str))
                status = !status;
    }
    nitf_TRE_cleanup(&cursor);
    return status;
}


NITFPROT(int) nitf_TRE_computeLength(nitf_TRE * tre)
{
    int length = 0;
    int tempLength;
    nitf_Error error;
    nitf_Pair *pair;            /* temp nitf_Pair */
    nitf_Field *field;          /* temp nitf_Field */
    nitf_TRECursor cursor;

    /* get out if TRE is null */
    if (!tre)
        return -1;

    cursor = nitf_TRE_begin(tre);
    while (!nitf_TRE_isDone(&cursor))
    {
        if (nitf_TRE_iterate(&cursor, &error) == NITF_SUCCESS)
        {
            tempLength = cursor.length;
            if (tempLength == NITF_TRE_GOBBLE)
            {
                /* we don't have any other way to know the length of this
                 * field, other than to see if the field is in the hash
                 * and use the length defined when it was created.
                 * Otherwise, we don't add any length.
                 */
                tempLength = 0;
                pair = nitf_HashTable_find(tre->hash, cursor.tag_str);
                if (pair)
                {
                    field = (nitf_Field *) pair->data;
                    if (field)
                        tempLength = field->length;
                }
            }
            length += tempLength;
        }
    }
    nitf_TRE_cleanup(&cursor);
    return length;
}


NITFAPI(int) nitf_TRE_parse(nitf_TRE * tre,
                            char *bufptr, nitf_Error * error)
{
    int status = 1;
    int iterStatus = NITF_SUCCESS;
    int offset = 0;
    int length;
    nitf_TRECursor cursor;
    nitf_Field *field;

    /* get out if TRE is null */
    if (!tre)
    {
        nitf_Error_init(error, "parse -> invalid tre object",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }

    /* flush the TRE hash first, to protect from duplicate entries */
    nitf_TRE_flush(tre, error);

    cursor = nitf_TRE_begin(tre);
    while (offset < tre->length && status)
    {
        if ((iterStatus = nitf_TRE_iterate(&cursor, error)) == NITF_SUCCESS)
        {
            length = cursor.length;
            if (length == NITF_TRE_GOBBLE)
            {
                length = tre->length - offset;
            }

            /* no need to call setValue, because we already know
             * it is OK for this one to be in the hash
             */
            /* construct the field */
            field =
                nitf_Field_construct(length, cursor.desc_ptr->data_type,
                                     error);
            if (!field)
                goto CATCH_ERROR;

            /* first, check to see if we need to swap bytes */
            if (field->type == NITF_BINARY
                    && (length == NITF_INT16_SZ || length == NITF_INT32_SZ))
            {
                if (length == NITF_INT16_SZ)
                {
                    nitf_Int16 int16 =
                        (nitf_Int16)NITF_NTOHS(*((nitf_Int16 *) (bufptr + offset)));
                    status = nitf_Field_setRawData(field,
                                                   (NITF_DATA *) & int16, length, error);
                }
                else if (length == NITF_INT32_SZ)
                {
                    nitf_Int32 int32 =
                        (nitf_Int32)NITF_NTOHL(*((nitf_Int32 *) (bufptr + offset)));
                    status = nitf_Field_setRawData(field,
                                                   (NITF_DATA *) & int32, length, error);
                }
            }
            else
            {
                /* check for the other binary lengths ... */
                if (field->type == NITF_BINARY)
                {
                    /* TODO what to do??? 8 bit is ok, but what about 64? */
                    /* for now, just let it go through... */
                }

                /* now, set the data */
                status = nitf_Field_setRawData(field, (NITF_DATA *) (bufptr + offset),
                                               length, error);
            }

#ifdef NITF_DEBUG
            {
                fprintf(stdout, "Adding Field [%s] to TRE [%s]\n",
                        cursor.tag_str, tre->tag);
            }
#endif

            /* add to the hash */
            nitf_HashTable_insert(tre->hash, cursor.tag_str, field, error);

            offset += length;
        }
        /* otherwise, the iterate function thinks we are done */
        else
        {
            break;
        }
    }
    nitf_TRE_cleanup(&cursor);

    /* check if we still have more to parse, and throw an error if so */
    if (offset < tre->length)
    {
        nitf_Error_init(error, "TRE data is longer than it should be",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        status = NITF_FAILURE;
    }
    return status;

    /* deal with errors here */
CATCH_ERROR:
    return NITF_FAILURE;
}


NITFPROT(char *) nitf_TRE_getRawData(nitf_TRE * tre, nitf_Uint32* treLength, nitf_Error * error)
{
    int status = 1;
    int offset = 0;
    nitf_Uint32 length;
    int tempLength;
    char *data = NULL;          /* data buffer - Caller must free this */
    char *tempBuf = NULL;       /* temp data buffer */
    nitf_Pair *pair;            /* temp nitf_Pair */
    nitf_Field *field;          /* temp nitf_Field */
    nitf_TRECursor cursor;      /* the cursor */

    /* get acutal length of TRE */
    if (tre->length <= NITF_TRE_DEFAULT_LENGTH)
        length = nitf_TRE_computeLength(tre);
    else
        length = tre->length;
    *treLength = length;

    if (length <= 0)
    {
        nitf_Error_init(error, "TRE has invalid length",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NULL;
    }

    /* allocate the memory - this does not get freed in this function */
    data = (char *) NITF_MALLOC(length + 1);
    if (!data)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(data, 0, length + 1);

    cursor = nitf_TRE_begin(tre);
    while (!nitf_TRE_isDone(&cursor) && status && offset < length)
    {
        if (nitf_TRE_iterate(&cursor, error) == NITF_SUCCESS)
        {
            pair = nitf_HashTable_find(tre->hash, cursor.tag_str);
            if (pair && pair->data)
            {
                tempLength = cursor.length;
                if (tempLength == NITF_TRE_GOBBLE)
                {
                    tempLength = length - offset;
                }
                field = (nitf_Field *) pair->data;

                /* get the raw data */
                tempBuf = NITF_MALLOC(tempLength);
                if (!tempBuf)
                {
                    nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                                    NITF_CTXT, NITF_ERR_MEMORY);
                    goto CATCH_ERROR;
                }
                /* get the data as raw buf */
                nitf_Field_get(field, (NITF_DATA *) tempBuf,
                               NITF_CONV_RAW, tempLength, error);

                /* first, check to see if we need to swap bytes */
                if (field->type == NITF_BINARY)
                {
                    if (tempLength == NITF_INT16_SZ)
                    {
                        nitf_Int16 int16 =
                            (nitf_Int16)NITF_HTONS(*((nitf_Int16 *) tempBuf));
                        memcpy(tempBuf, (char*)&int16, tempLength);
                    }
                    else if (tempLength == NITF_INT32_SZ)
                    {
                        nitf_Int32 int32 =
                            (nitf_Int32)NITF_HTONL(*((nitf_Int32 *) tempBuf));
                        memcpy(tempBuf, (char*)&int32, tempLength);
                    }
                    else
                    {
                        /* TODO what to do??? 8 bit is ok, but what about 64? */
                        /* for now, just let it go through... */
                    }
                }

                /* now, memcpy the data */
                memcpy(data + offset, tempBuf, tempLength);
                offset += tempLength;

                /* free the buf */
                NITF_FREE(tempBuf);
            }
            else
            {
                nitf_Error_init(error,
                                "Failed due to missing TRE field(s)",
                                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
                goto CATCH_ERROR;
            }
        }
    }
    nitf_TRE_cleanup(&cursor);
    return data;

    /* deal with errors here */
CATCH_ERROR:
    if (data)
        NITF_FREE(data);
    return NULL;
}


NITFAPI(NITF_BOOL) nitf_TRE_foreach(nitf_TRE * tre,
                                    NITF_TRE_FUNCTOR fn,
                                    nitf_List * warningList,
                                    NITF_DATA* userData,
                                    nitf_Error * error)
{
    int status = 1;
    nitf_Pair *pair;            /* temp nitf_Pair */
    nitf_Field *field;          /* temp nitf_Field */
    nitf_TRECursor cursor;

    /* get out if TRE is null */
    if (!tre)
    {
        nitf_Error_init(error, "foreach -> invalid tre object",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }

    cursor = nitf_TRE_begin(tre);
    while (!nitf_TRE_isDone(&cursor) && status)
    {
        if (nitf_TRE_iterate(&cursor, error) == NITF_SUCCESS)
        {
            pair = nitf_HashTable_find(tre->hash, cursor.tag_str);
            if (!pair || !pair->data)
            {
                nitf_Error_initf(error, NITF_CTXT, NITF_ERR_UNK,
                                 "Unable to find tag, '%s', in TRE hash for TRE '%s'", cursor.tag_str, tre->tag);
                status = NITF_FAILURE;
            }
            else
            {
                field = (nitf_Field *) pair->data;
                status =
                    (*fn) (tre, cursor.desc_ptr, cursor.tag_str, field,
                           warningList, userData, error);
            }
        }
    }
    nitf_TRE_cleanup(&cursor);
    return status;
}


NITFPROT(void) nitf_TRE_flush(nitf_TRE * tre, nitf_Error * error)
{
    if (tre->hash)
    {
        /* destruct each field in the hash */
        nitf_HashTable_foreach(tre->hash,
                               (NITF_HASH_FUNCTOR) destructHashValue,
                               NULL, error);
    }
}


NITFAPI(nitf_TREDescriptionSet *) nitf_TRE_getDescriptionSet
(
    const char *tre_id,
    nitf_Error *error
)
{
    int bad = 0;
    NITF_PLUGIN_TRE_GET_DESCRIPTIONS_FUNCTION function =
        (NITF_PLUGIN_TRE_GET_DESCRIPTIONS_FUNCTION) NULL;
    nitf_TREDescriptionSet *descriptions = NULL;\
    nitf_PluginRegistry *reg = NULL;
    
    /* get out if TRE is null */
    if (!tre_id)
    {
        nitf_Error_init(error, "getDescriptionSet -> invalid TRE ID",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        goto CATCH_ERROR;
    }

    reg = nitf_PluginRegistry_getInstance(error);
    if (reg)
    {
        function = nitf_PluginRegistry_retrieveTREDescription(
            reg, tre_id, &bad,error);
        if (bad)
            goto CATCH_ERROR;
    }

    if (function == (NITF_PLUGIN_TRE_GET_DESCRIPTIONS_FUNCTION) NULL)
    {
        nitf_Error_init(error,
                        "TRE GetDescriptions handle is NULL",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }
    
    descriptions = (*function)();
    if (!descriptions)
    {
        nitf_Error_init(error, "TRE Description Set is NULL",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }
    return descriptions;
CATCH_ERROR:
    return NULL;
}
