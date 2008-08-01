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
 * License along with this program; 
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "nitf/List.h"

NITFAPI(nitf_ListNode *) nitf_ListNode_construct(nitf_ListNode * prev,
        nitf_ListNode * next,
        NITF_DATA * data,
        nitf_Error * error)
{
    nitf_ListNode *node =
        (nitf_ListNode *) NITF_MALLOC(sizeof(nitf_ListNode));
    if (node == NULL)
    {
        /*  Init the error with the string value of errno  */
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        /*  Return if we have a problem  */
        return NULL;
    }

    /*  Attention!! Achtung!! This is a data pointer copy,    */
    /*  not copying an object.  That means that YOU the user  */
    /*  of this API MUST allocate it yourself                 */
    /*  And, of course, YOU must delete it as well!!          */
    node->data = data;

    /*  Attach up our nodes  */
    node->next = next;
    node->prev = prev;

    /*  Return the new node  */
    return node;
}


NITFAPI(void) nitf_ListNode_destruct(nitf_ListNode ** this_node)
{
    if (*this_node)
    {
        NITF_FREE(*this_node);
        *this_node = NULL;
    }
}


NITFAPI(NITF_BOOL) nitf_List_isEmpty(nitf_List * this_list)
{
    /*  Be ruthless with our assertions  */
    assert(this_list);

    return !this_list->first;
}


NITFAPI(NITF_BOOL) nitf_List_pushFront(nitf_List * this_list,
                                       NITF_DATA * data,
                                       nitf_Error * error)
{
    /*  Construct a new node, with no surrounding context  */
    nitf_ListNode *node = nitf_ListNode_construct(NULL, NULL, data, error);
    if (!node)
    {
        /*  The node constructor inited the error, so let's go home  */
        return 0;
    }

    /*  Hook the links up manually  */
    if (this_list->first)
    {
        /*  Red wire to the green wire...  */

        /*  Give the first link a previous (the new one)  */
        this_list->first->prev = node;

        /*  Point this one's next at the old first  */
        node->next = this_list->first;

        /*  So now, reassign the meaning of first to our new one  */
        this_list->first = node;
    }
    /*  Otherwise, we are the first and the last  */
    /*  (It was empty before)                     */
    else
        this_list->first = this_list->last = node;
    return 1;
}


NITFAPI(NITF_BOOL) nitf_List_pushBack(nitf_List * this_list,
                                      NITF_DATA * data, nitf_Error * error)
{

    /*  Create a new node with the data, to place on the back  */
    nitf_ListNode *node =
        nitf_ListNode_construct(this_list->last, NULL, data, error);

    if (!node)
    {
        return 0;
    }

    /*  If the last one is set  */
    if (this_list->last)
    {

        /*  Point the (former) last node at our new node */
        /*  And, of course, reassign what last means     */
        this_list->last = this_list->last->next = node;
    }

    /*  Otherwise this is the first node  */
    else
    {

        this_list->first = this_list->last = node;
    }
    return 1;
}


NITFAPI(NITF_DATA *) nitf_List_popFront(nitf_List * this_list)
{

    /*  Make sure to NULL init it  */
    NITF_DATA *data = NULL;

    /*  Get ourselves a pointer to the first link  */
    nitf_ListNode *popped = this_list->first;

    /*  Be ruthless with our assertions  */
    assert(this_list);

    /*  If it exists, reassign the pointers  */
    if (popped)
    {
        /*  If there is more than one node  */
        if (this_list->first != this_list->last)
        {
            /*  Reassign the first to the next  */
            this_list->first = this_list->first->next;
            /*  Reset the first link, thereby eliminating popped  */
            this_list->first->prev = NULL;
        }
        /*  Otherwise reset the first and the last to NULL pointers   */
        else
            this_list->first = this_list->last = NULL;
        data = popped->data;
        nitf_ListNode_destruct(&popped);
    }
    /*  Return the popped item.  Deletion is YOUR problem  */
    return data;
}


NITFAPI(NITF_DATA *) nitf_List_popBack(nitf_List * this_list)
{
    NITF_DATA *data = NULL;

    /*  Get a pointer to the current last  */
    nitf_ListNode *popped = this_list->last;

    /*  Be ruthless with our assertions  */
    assert(this_list);

    /*  If we have an element  */
    if (popped)
    {
        /*  If there is more than one element  */
        if (this_list->last != this_list->first)
        {
            /*  Point the list last node at the previous to last  */
            this_list->last = this_list->last->prev;
            /*  Now eliminate the 'next' and we have shortened our list  */
            this_list->last->next = NULL;
        }
        /*  Otherwise, set our list pointers to NULL  (we are empty)  */
        else
        {
            this_list->first = this_list->last = NULL;
        }
        data = popped->data;
        nitf_ListNode_destruct(&popped);

        /*  Technically, you could put empty data on here,  */
        /*  but why would you???                            */
        assert(data);
    }
    /*  Return the popped node  */
    return data;
}


NITFAPI(nitf_List *) nitf_List_construct(nitf_Error * error)
{
    /*  New allocate a list */
    nitf_List *l;
    l = (nitf_List *) NITF_MALLOC(sizeof(nitf_List));
    if (!l)
    {
        /*  Initialize the error and return NULL  */
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }
    /*  Null-initialize the link pointers  */
    l->first = l->last = NULL;
    return l;
}


NITFAPI(nitf_List *) nitf_List_clone(nitf_List * source,
                                     NITF_DATA_ITEM_CLONE cloner,
                                     nitf_Error * error)
{
    nitf_List *l = NULL;
    if (source)
    {
        nitf_ListIterator iter = nitf_List_begin(source);
        nitf_ListIterator end = nitf_List_end(source);
        l = nitf_List_construct(error);
        if (!l)
            return NULL;

        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {

            /*  Foreach item in each list... */
            NITF_DATA *data = nitf_ListIterator_get(&iter);

            /*  Use the function pointer to clone the object...  */
            NITF_DATA *newData = (NITF_DATA *) cloner(data, error);
            if (!newData)
                return NULL;

            /*  ... and then insert it with the key into the new table  */
            if (!nitf_List_pushBack(l, newData, error))
            {
                /* destruct the created list.
                 * NOTE - there is no way for us to destroy the NITF_DATA* chunks
                 * that have already been cloned
                 */
                nitf_List_destruct(&l);
                return NULL;
            }

            nitf_ListIterator_increment(&iter);
        }
    }
    else
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Trying to clone NULL pointer");
    }

    return l;
}


NITFAPI(void) nitf_List_destruct(nitf_List ** this_list)
{
    NITF_DATA *data;
    if (*this_list)
    {
        /*  Destroy each node in our list.  */
        while (!nitf_List_isEmpty(*this_list))
        {
            /*  Pop one off the back and delete it  */
            data = nitf_List_popBack(*this_list);
            /* delete it */
            if (data)
                NITF_FREE(data);
        }
        NITF_FREE(*this_list);
        *this_list = NULL;
    }

}


NITFAPI(nitf_ListIterator) nitf_List_begin(nitf_List * list)
{
    nitf_ListIterator list_iterator;

    /*  Be ruthless with our assertions  */
    assert(list);
    list_iterator.current = list->first;

    return list_iterator;
}


NITFAPI(nitf_ListIterator) nitf_List_at(nitf_List * chain, int i)
{
    nitf_ListIterator list_iterator = nitf_List_begin(chain);
    nitf_ListIterator end = nitf_List_end(chain);
    int j;
    for (j = 0; j < i; j++)
    {
        if (nitf_ListIterator_equals(&list_iterator, &end))
        {
            list_iterator.current = NULL;
            break;
        }
        nitf_ListIterator_increment(&list_iterator);
    }

    return list_iterator;
}


NITFAPI(NITF_BOOL) nitf_ListIterator_equals(nitf_ListIterator * it1,
        nitf_ListIterator * it2)
{
    return (it1->current == it2->current);
}


NITFAPI(NITF_BOOL) nitf_ListIterator_notEqualTo(nitf_ListIterator * it1,
        nitf_ListIterator * it2)
{
    return !nitf_ListIterator_equals(it1, it2);
}


NITFAPI(nitf_ListIterator) nitf_List_end(nitf_List * this_list)
{
    nitf_ListIterator list_iterator;
    list_iterator.current = NULL;
    return list_iterator;
}


NITFAPI(NITF_DATA *) nitf_List_remove(nitf_List * list,
                                      nitf_ListIterator * where)
{
    NITF_DATA *data = NULL;
    nitf_ListNode *old = NULL;
    nitf_ListNode *new_current = NULL;

    /*  Take care of special cases early  */
    if (where->current == list->first)
    {
        data = nitf_List_popFront(list);
        *where = nitf_List_begin(list);
    }
    else if (where->current == list->last)
    {
        data = nitf_List_popBack(list);
        *where = nitf_List_end(list);

    }
    else
    {
        /*  Make a pointer to the data right quick  */
        data = where->current->data;

        /*  Rewire the surrounding elements  */
        old = where->current;
        new_current = old->next;

        new_current->prev = old->prev;
        old->prev->next = new_current;

        /*  Reset the iterator to the NEXT */
        where->current = new_current;

        /* Now, destruct the listNode, but not the data */
        nitf_ListNode_destruct(&old);
    }
    /*  Return what we saved  */

    return data;
}

NITFAPI(NITF_BOOL) nitf_List_move(nitf_List * chain,
                                  nitf_Uint32 oldIndex,
                                  nitf_Uint32 newIndex,
                                  nitf_Error * error)
{
    nitf_Uint32 listSize = nitf_List_size(chain);
    nitf_ListIterator iter;
    NITF_DATA* data = NULL;
    
    /* don't need to check < 0, but in case params change, we'll do it anyway */
    if (oldIndex < 0 || oldIndex == newIndex || oldIndex >= listSize)
    {
        nitf_Error_init(error, "Invalid list index specified",
                            NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }
    newIndex = newIndex > listSize || newIndex < 0 ? listSize : newIndex;
    
    /* first, remove the data from the list */
    iter = nitf_List_at(chain, oldIndex);
    data = nitf_List_remove(chain, &iter);
    /* next, insert it at the new location */
    iter = nitf_List_at(chain, newIndex);
    return nitf_List_insert(chain, iter, data, error);
}


NITFAPI(NITF_BOOL) nitf_List_insert(nitf_List * list,
                                    nitf_ListIterator iter,
                                    NITF_DATA * data, nitf_Error * error)
{
    /*  If our iterator is not set, we will insert the data into the back */
    if (!iter.current)
    {
        /*  Push the data onto the back  */
        if (!nitf_List_pushBack(list, data, error))
        {
            return 0;
        }
        /*  Update the iterator to point at us  */
        iter.current = list->last;
    }
    /*  If the iterator is at the first  */
    else if (iter.current == list->first)
    {
        /*  Push data onto the front  */
        if (!nitf_List_pushFront(list, data, error))
        {
            return 0;
        }
        /*  Update the iterator to point at us  */
        iter.current = list->first;
    }
    else
    {

        /*  Construct a new node and insert it before the current  */
        nitf_ListNode *new_node =
            nitf_ListNode_construct(iter.current->prev,
                                    iter.current,
                                    data,
                                    error);

        /*  If an error occurred, the list node captured it  */
        if (!new_node)
        {
            /*  Error already populated, go home  */
            return 0;
        }

        /*  Point the iterator at our new node  */
        iter.current->prev->next = new_node;
        new_node->next->prev = new_node;
        iter.current = new_node;
    }
    return 1;
}


NITFAPI(void) nitf_ListIterator_increment(nitf_ListIterator * this_iter)
{
    if (this_iter->current)
        this_iter->current = this_iter->current->next;
}


NITFAPI(NITF_DATA *) nitf_ListIterator_get(nitf_ListIterator * this_iter)
{
    /*  Lets make sure we are okay  */
    assert(this_iter->current);
    return this_iter->current->data;
}


NITFAPI(nitf_Uint32) nitf_List_size(nitf_List * list)
{
    nitf_Uint32 size = 0;

    if (list)
    {
        nitf_ListIterator iter = nitf_List_begin(list);
        nitf_ListIterator end = nitf_List_end(list);
        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {
            ++size;
            nitf_ListIterator_increment(&iter);
        }
    }
    return size;
}


NITFAPI(NITF_DATA *) nitf_List_get(nitf_List * list,
                                   int index, nitf_Error * error)
{
    int i = 0;
    if (list)
    {
        nitf_ListIterator iter = nitf_List_begin(list);
        nitf_ListIterator end = nitf_List_end(list);
        for (i = 0; i < index && nitf_ListIterator_notEqualTo(&iter, &end);
                ++i)
            nitf_ListIterator_increment(&iter);
        if (i == index && nitf_ListIterator_notEqualTo(&iter, &end))
        {
            return nitf_ListIterator_get(&iter);
        }
        else
        {
            nitf_Error_init(error, "Object not found at index",
                            NITF_CTXT, NITF_ERR_INVALID_OBJECT);
            return NULL;
        }
    }
    else
    {
        nitf_Error_init(error, "Invalid list -> NULL",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NULL;
    }
}
