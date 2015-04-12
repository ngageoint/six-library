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

#include "nitf/TRECursor.h"
#include "nitf/TREPrivateData.h"


#define TAG_BUF_LEN 256



NITFPRIV(nitf_Pair *) nitf_TRECursor_getTREPair(nitf_TRE * tre,
                                               char *descTag,
                                               char idx_str[10][10],
                                               int looping,
                                               nitf_Error * error);

NITFPRIV(int) nitf_TRECursor_evalIf(nitf_TRE * tre,
                                   nitf_TREDescription * desc_ptr,
                                   char idx_str[10][10],
                                   int looping,
                                   nitf_Error * error);

/**
 * Helper function for evaluating loops
 * Returns the number of loops that will be processed
 */
NITFPRIV(int) nitf_TRECursor_evalLoops(nitf_TRE * tre,
                                      nitf_TREDescription * desc_ptr,
                                      char idx_str[10][10],
                                      int looping,
                                      nitf_Error * error);


NITFPRIV(int) nitf_TRECursor_evalCondLength(nitf_TRE * tre,
                                           nitf_TREDescription * desc_ptr,
                                           char idx_str[10][10],
                                           int looping,
                                           nitf_Error * error);


/*!
 *  Evaluates the given postfix expression, looking up fields in the TRE, or
 *  using constant integers.
 *
 *  \param tre      The TRE to use
 *  \param idx      The loop index/values
 *  \param looping  The current loop level
 *  \param expression The postfix expression
 *  \param error The error to populate on failure
 *  \return NITF_SUCCESS on sucess or NITF_FAILURE otherwise
 */
NITFPRIV(int) nitf_TRECursor_evaluatePostfix(nitf_TRE *tre,
                                             char idx[10][10],
                                             int looping,
                                             char *expression,
                                             nitf_Error *error);

typedef unsigned int (*NITF_TRE_CURSOR_COUNT_FUNCTION) (nitf_TRE *,
                                                        char idx[10][10],
                                                        int,
                                                        nitf_Error*);



NITFAPI(nitf_TRECursor) nitf_TRECursor_begin(nitf_TRE * tre)
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
    /* init the pointers */
    tre_cursor.end_ptr = NULL;
    tre_cursor.prev_ptr = NULL;
    tre_cursor.desc_ptr = NULL;

    if (tre)
    {
        /* set the start index */
        tre_cursor.index = -1;
        /* count how many descriptions there are */
		dptr = ((nitf_TREPrivateData*)tre->priv)->description;

        while (dptr && (dptr->data_type != NITF_END))
        {
            tre_cursor.numItems++;
            dptr++;
        }
        tre_cursor.end_ptr = dptr;
        memset(tre_cursor.tag_str, 0, TAG_BUF_LEN);
		NITF_SNPRINTF(tre_cursor.tag_str, TAG_BUF_LEN, "%s",
		        ((nitf_TREPrivateData*)tre->priv)->description->tag);
        tre_cursor.tre = tre;
    }

    return tre_cursor;
}

NITFAPI(nitf_TRECursor) nitf_TRECursor_clone(nitf_TRECursor *tre_cursor,
        nitf_Error * error)
{
    nitf_TRECursor cursor;
    cursor.numItems = tre_cursor->numItems;
    cursor.index = tre_cursor->index;
    cursor.looping = tre_cursor->looping;
    cursor.loop = nitf_IntStack_clone(tre_cursor->loop, error);
    cursor.loop_idx = nitf_IntStack_clone(tre_cursor->loop_idx, error);
    cursor.loop_rtn = nitf_IntStack_clone(tre_cursor->loop_rtn, error);
    cursor.tre = tre_cursor->tre;
    cursor.end_ptr = tre_cursor->end_ptr;

    cursor.prev_ptr = tre_cursor->prev_ptr;
    cursor.desc_ptr = tre_cursor->desc_ptr;
    strcpy(cursor.tag_str, tre_cursor->tag_str);
    cursor.length = tre_cursor->length;
    return cursor;
}


/*!
 * Normalizes the tag, which could be in a loop, and returns the nitf_Pair* from
 * the TRE hash that corresponds to the current normalized tag.
 */
NITFPRIV(nitf_Pair *) nitf_TRECursor_getTREPair(nitf_TRE * tre,
                                                char *descTag,
                                                char idx_str[10][10],
                                                int looping, nitf_Error * error)
{
    /* temp buf used for storing the qualified tag */
    char tag_str[TAG_BUF_LEN];         

    /* pointer for brace position */
    char *bracePtr;             

    /* used as in iterator */
    int index = 0;

    /* temp used for looping */
    int i;                      

    /* the pair to return */
    nitf_Pair *pair = NULL;

    strncpy(tag_str, descTag, sizeof(tag_str));

    /* deal with braces */
    if (strchr(descTag, '['))
    {
        index = 0;
        bracePtr = descTag;
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
         * since tags are unique, we are ok checking like this 
         */
        pair = nitf_HashTable_find(
                ((nitf_TREPrivateData*)tre->priv)->hash, tag_str);
        for (i = 0; i < looping && !pair; ++i)
        {
            strcat(tag_str, idx_str[i]);
            pair = nitf_HashTable_find(
                    ((nitf_TREPrivateData*)tre->priv)->hash, tag_str);
        }
    }

    /* pull the data from the hash about the dependent loop value */
    pair = nitf_HashTable_find(((nitf_TREPrivateData*)tre->priv)->hash,
                               tag_str);
    return pair;
}



NITFAPI(void) nitf_TRECursor_cleanup(nitf_TRECursor * tre_cursor)
{
    nitf_IntStack_destruct(&tre_cursor->loop);
    nitf_IntStack_destruct(&tre_cursor->loop_idx);
    nitf_IntStack_destruct(&tre_cursor->loop_rtn);
}


NITFAPI(NITF_BOOL) nitf_TRECursor_isDone(nitf_TRECursor * tre_cursor)
{
    nitf_Error error;
    int isDone = (tre_cursor->desc_ptr == tre_cursor->end_ptr);

    /* check if the passed in cursor is not at the beginning */
    if (!isDone && tre_cursor->index >= 0)
    {
        nitf_TRECursor dolly = nitf_TRECursor_clone(tre_cursor, &error);
        /* if iterate returns 0, we are done */
        isDone = !nitf_TRECursor_iterate(&dolly, &error);
        isDone = isDone || (dolly.desc_ptr == dolly.end_ptr);
        nitf_TRECursor_cleanup(&dolly);
    }
    return isDone;
}

NITFAPI(int) nitf_TRECursor_iterate(nitf_TRECursor * tre_cursor,
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

	dptr = ((nitf_TREPrivateData*)tre_cursor->tre->priv)->description;

    while (!done)
    {
        done = 1;               /* set the flag */

        /* iterate the index */
        tre_cursor->index++;

        if (tre_cursor->index < tre_cursor->numItems)
        {
            memset(tre_cursor->tag_str, 0, TAG_BUF_LEN);

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
                    NITF_SNPRINTF(idx_str[index], TAG_BUF_LEN,
                                  "[%d]", stack[index]);
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
                NITF_SNPRINTF(tre_cursor->tag_str, TAG_BUF_LEN, "%s",
                        tre_cursor->desc_ptr->tag);
                /* check if data is part of an array */
                if (tre_cursor->looping)
                {
                    stack = tre_cursor->loop_idx->st;
                    for (index = 0; index < tre_cursor->looping; index++)
                    {
                        char entry[64];
                        NITF_SNPRINTF(entry, 64, "[%d]", stack[index]);
                        strcat(tre_cursor->tag_str, entry);
                    }
                }

                /* check to see if we don't know the length */
                if (tre_cursor->desc_ptr->data_count ==
                        NITF_TRE_CONDITIONAL_LENGTH)
                {
                    /* compute it from the function given */
                    if (tre_cursor->desc_ptr->special)
                    {
                        /* evaluate the special string as a postfix expression */
                        tre_cursor->length = 
                            nitf_TRECursor_evaluatePostfix(
                                tre_cursor->tre, 
                                idx_str, 
                                tre_cursor->looping,
                                tre_cursor->desc_ptr->special, 
                                error);

                        if (tre_cursor->length < 0)
                        {
                            /* error! */
                            nitf_Error_print(error, stderr, "TRE expression error:");
                            return NITF_FAILURE;
                        }
                    }
                    else
                    {
                        /* should we return failure here? */
                        /* for now, just set the length to 0, which forces an
                           iteration... */
                        tre_cursor->length = 0;
                    }

                    if (tre_cursor->length == 0)
                    {
                        return nitf_TRECursor_iterate(tre_cursor, error);
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
                        nitf_TRECursor_evalLoops(tre_cursor->tre,
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
                    if (!nitf_TRECursor_evalIf
                            (tre_cursor->tre, 
                             tre_cursor->desc_ptr, 
                             idx_str,
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


/**
 * Helper function for evaluating loops
 * Returns the number of loops that will be processed
 */
NITFPRIV(int) nitf_TRECursor_evalLoops(nitf_TRE* tre,
                                      nitf_TREDescription* desc_ptr,
                                      char idx_str[10][10],
                                      int looping, nitf_Error* error)
{
    int loops;

    /* temp buf used for manipulating the loop label */
    char str[TAG_BUF_LEN];   

    nitf_Pair *pair;
    nitf_Field *field;

    char *op;
    char *valPtr;
    
    /* used for the possible data in the loop label */
    int loopVal;

    /* if the user wants a constant value */
    if (desc_ptr->label && strcmp(desc_ptr->label, NITF_CONST_N) == 0)
    {
        loops = NITF_ATO32(desc_ptr->tag);
    }

    else if (desc_ptr->label && strcmp(desc_ptr->label, NITF_FUNCTION) == 0)
    {
        NITF_TRE_CURSOR_COUNT_FUNCTION fn =
            (NITF_TRE_CURSOR_COUNT_FUNCTION)desc_ptr->tag;


        loops = (*fn)(tre, idx_str, looping, error);

        if (loops == -1)
            return NITF_FAILURE;
    }

    else
    {
        pair = nitf_TRECursor_getTREPair(tre, desc_ptr->tag,
                                         idx_str, looping, error);
        if (!pair)
        {
            nitf_Error_init(error,
                            "nitf_TRECursor_evalLoops: invalid TRE loop counter",
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
                                            "nitf_TRECursor_evalLoops: attempt to divide by zero",
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
                nitf_Error_init(error, "nitf_TRECursor_evalLoops: invalid operator",
                                NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                return NITF_FAILURE;
            }
        }
    }
    return loops < 0 ? 0 : loops;
}


NITFPRIV(int) nitf_TRECursor_evalIf(nitf_TRE* tre,
                                   nitf_TREDescription* desc_ptr,
                                   char idx_str[10][10],
                                   int looping, 
                                   nitf_Error* error)
{
    nitf_Field *field;
    nitf_Pair *pair;

    /* temp buf for the label */
    char str[TAG_BUF_LEN];      
    char *emptyPtr;
    char *op;
    char *valPtr;

    /* the return status */
    int status = 0;

    /* used as the value for comparing */
    int fieldData;
    
    /* the value defined int the TRE descrip */
    int treData;

    /* the bit-field for comparing */
    unsigned int bitFieldData;

    /* the bit-field defined in the TRE descrip */
    unsigned int treBitField;

    /* get the data out of the hashtable */
    pair = nitf_TRECursor_getTREPair(tre, desc_ptr->tag, idx_str, 
                                     looping, error);
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
    /* check if it is a bit-wise operator */
    else if (strcmp(op, "&") == 0)
    {
        /* make sure it is a binary field */
        if (field->type != NITF_BINARY)
        {
            nitf_Error_init(error,
                            "evaluate: must use binary data for bit-wise expressions",
                            NITF_CTXT,
                            NITF_ERR_INVALID_PARAMETER);
            return NITF_FAILURE;
        }

        treBitField = NITF_ATOU32_BASE(valPtr, 0);
        if (!nitf_Field_get(field,
                            (char *)&bitFieldData,
                            NITF_CONV_UINT,
                            sizeof(bitFieldData),
                            error))
        {
            return NITF_FAILURE;
        }

        /* check this bit field */
        status = ((treBitField & bitFieldData) != 0);
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
NITFPRIV(int) nitf_TRECursor_evalCondLength(nitf_TRE* tre,
                                           nitf_TREDescription* desc_ptr,
                                           char idx_str[10][10],
                                           int looping, 
                                           nitf_Error* error)
{
    int computedLength;
    /* temp buf used for manipulating the loop label */
    char str[TAG_BUF_LEN];      
    nitf_Pair *pair;
    nitf_Field *field;

    char *op;
    char *valPtr;

    /* used for the possible data in the description label */
    int funcVal;

    pair = nitf_TRECursor_getTREPair(tre, desc_ptr->tag, 
                                     idx_str, looping, error);
    if (!pair)
    {
        nitf_Error_init(error,
                        "nitf_TRECursor_evalCondLength: invalid TRE reference",
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
                                        "nitf_TRECursor_evalCondLength: attempt to divide by zero",
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
                            "nitf_TRECursor_evalCondLength: invalid operator",
                            NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
            return NITF_FAILURE;
        }
    }
    return computedLength < 0 ? 0 : computedLength;
}

NITFPRIV(int) nitf_TRECursor_evaluatePostfix(nitf_TRE *tre,
                                             char idx[10][10],
                                             int looping,
                                             char *expression,
                                             nitf_Error *error)
{
    nitf_List *parts = NULL;
    nitf_IntStack *stack = NULL;
    int expressionValue;

    /* create the postfix stack */
    stack = nitf_IntStack_construct(error);
    if (!stack)
        goto CATCH_ERROR;

    /* split the expression by spaces */
    parts = nitf_Utils_splitString(expression, 0, error);
    if (!parts)
        goto CATCH_ERROR;

    while(!nitf_List_isEmpty(parts))
    {
        char* expr = (char*) nitf_List_popFront(parts);
        if (strlen(expr) == 1 &&
                (expr[0] == '+' || expr[0] == '-' || expr[0] == '*' ||
                        expr[0] == '/' || expr[0] == '%'))
        {
            int op1, op2, stackSize;
            stackSize = nitf_IntStack_depth(stack, error) + 1;

            if (stackSize == 0)
            {
                /* error for postfix... */
                nitf_Error_init(error,
                        "nitf_TRECursor_evaluatePostfix: invalid expression",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                goto CATCH_ERROR;
            }

            op2 = nitf_IntStack_pop(stack, error);
            if (stackSize == 1)
                op1 = 0; /* assume 0 for the first operand of a unary op */
            else
                op1 = nitf_IntStack_pop(stack, error);

            switch(expr[0])
            {
            case '+':
                nitf_IntStack_push(stack, (op1 + op2), error);
                break;
            case '-':
                nitf_IntStack_push(stack, (op1 - op2), error);
                break;
            case '*':
                nitf_IntStack_push(stack, (op1 * op2), error);
                break;
            case '/':
                /* check for divide by zero */
                if (op2 == 0)
                {
                    nitf_Error_init(error,
                            "nitf_TRECursor_evaluatePostfix: attempt to divide by zero",
                            NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                    goto CATCH_ERROR;
                }
                nitf_IntStack_push(stack, (op1 / op2), error);
                break;
            case '%':
                nitf_IntStack_push(stack, (op1 % op2), error);
                break;
            }
        }
        else
        {
            /* evaluate as an integer and push onto the stack */
            if (nitf_Utils_isNumeric(expr))
            {
                nitf_IntStack_push(stack, NITF_ATO32(expr), error);
            }
            else
            {
                /* must be a dependent field */
                int intVal;
                nitf_Field *field = NULL;
                nitf_Pair *pair = nitf_TRECursor_getTREPair(tre, expr, idx,
                        looping, error);

                if (!pair)
                {
                    nitf_Error_init(error,
                            "nitf_TRECursor_evaluatePostfix: invalid TRE field reference",
                            NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                    goto CATCH_ERROR;
                }
                field = (nitf_Field *) pair->data;

                /* get the int value */
                if (!nitf_Field_get(field, (char*) &intVal, NITF_CONV_INT,
                         sizeof(intVal), error))
                {
                    goto CATCH_ERROR;
                }
                nitf_IntStack_push(stack, intVal, error);
            }
        }

        /* must cleanup after ourselves */
        if (expr)
            NITF_FREE(expr);
    }

    /* if all is well, the postfix stack should have one value */
    if (nitf_IntStack_depth(stack, error) != 0)
    {
        nitf_Error_init(error, "Invalid postfix expression",
                NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        goto CATCH_ERROR;
    }

    expressionValue = nitf_IntStack_pop(stack, error);

    nitf_IntStack_destruct(&stack);
    nitf_List_destruct(&parts);

    return expressionValue;

  CATCH_ERROR:
    if (stack) nitf_IntStack_destruct(&stack);
    if (parts) nitf_List_destruct(&parts);
    return -1;
}
