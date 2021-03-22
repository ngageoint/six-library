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
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "nrt/List.h"

NRTAPI(nrt_ListNode *) nrt_ListNode_construct(nrt_ListNode * prev,
                                              nrt_ListNode * next,
                                              NRT_DATA * data,
                                              nrt_Error * error)
{
    nrt_ListNode *node = (nrt_ListNode *) NRT_MALLOC(sizeof(nrt_ListNode));
    if (node == NULL)
    {
        /* Init the error with the string value of errno */
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        /* Return if we have a problem */
        return NULL;
    }

    /* Attention!! Achtung!! This is a data pointer copy, */
    /* not copying an object.  That means that YOU the user */
    /* of this API MUST allocate it yourself */
    /* And, of course, YOU must delete it as well!! */
    node->data = data;

    /* Attach up our nodes */
    node->next = next;
    node->prev = prev;

    /* Return the new node */
    return node;
}

NRTAPI(void) nrt_ListNode_destruct(nrt_ListNode ** this_node)
{
    if (*this_node)
    {
        NRT_FREE(*this_node);
        *this_node = NULL;
    }
}

NRTAPI(NRT_BOOL) nrt_List_isEmpty(nrt_List * this_list)
{
    return !this_list || !this_list->first;
}

NRTAPI(NRT_BOOL) nrt_List_pushFront(nrt_List * this_list, NRT_DATA * data,
                                    nrt_Error * error)
{
    /* Construct a new node, with no surrounding context */
    nrt_ListNode *node = nrt_ListNode_construct(NULL, NULL, data, error);
    if (!node)
    {
        /* The node constructor inited the error, so let's go home */
        return 0;
    }

    /* Hook the links up manually */
    if (this_list->first)
    {
        /* Red wire to the green wire...  */

        /* Give the first link a previous (the new one) */
        this_list->first->prev = node;

        /* Point this one's next at the old first */
        node->next = this_list->first;

        /* So now, reassign the meaning of first to our new one */
        this_list->first = node;
    }
    /* Otherwise, we are the first and the last */
    /* (It was empty before) */
    else
        this_list->first = this_list->last = node;
    return 1;
}

NRTAPI(NRT_BOOL) nrt_List_pushBack(nrt_List * this_list, NRT_DATA * data,
                                   nrt_Error * error)
{

    /* Create a new node with the data, to place on the back */
    nrt_ListNode *node =
        nrt_ListNode_construct(this_list->last, NULL, data, error);

    if (!node)
    {
        return 0;
    }

    /* If the last one is set */
    if (this_list->last)
    {

        /* Point the (former) last node at our new node */
        /* And, of course, reassign what last means */
        this_list->last = this_list->last->next = node;
    }

    /* Otherwise this is the first node */
    else
    {

        this_list->first = this_list->last = node;
    }
    return 1;
}

NRTAPI(NRT_DATA *) nrt_List_popFront(nrt_List * this_list)
{

    /* Make sure to NULL init it */
    NRT_DATA *data = NULL;

    /* Get ourselves a pointer to the first link */
    nrt_ListNode *popped = this_list ? this_list->first : NULL;

    /* If it exists, reassign the pointers */
    if (popped)
    {
        /* If there is more than one node */
        if (this_list->first != this_list->last)
        {
            /* Reassign the first to the next */
            this_list->first = this_list->first->next;
            /* Reset the first link, thereby eliminating popped */
            this_list->first->prev = NULL;
        }
        /* Otherwise reset the first and the last to NULL pointers */
        else
            this_list->first = this_list->last = NULL;
        data = popped->data;
        nrt_ListNode_destruct(&popped);
    }
    /* Return the popped item.  Deletion is YOUR problem */
    return data;
}

NRTAPI(NRT_DATA *) nrt_List_popBack(nrt_List * this_list)
{
    NRT_DATA *data = NULL;

    /* Get a pointer to the current last */
    nrt_ListNode *popped = this_list ? this_list->last : NULL;

    /* If we have an element */
    if (popped)
    {
        /* If there is more than one element */
        if (this_list->last != this_list->first)
        {
            /* Point the list last node at the previous to last */
            this_list->last = this_list->last->prev;
            /* Now eliminate the 'next' and we have shortened our list */
            this_list->last->next = NULL;
        }
        /* Otherwise, set our list pointers to NULL (we are empty) */
        else
        {
            this_list->first = this_list->last = NULL;
        }
        data = popped->data;
        nrt_ListNode_destruct(&popped);
    }
    /* Return the popped node */
    return data;
}

NRTAPI(nrt_List *) nrt_List_construct(nrt_Error * error)
{
    /* New allocate a list */
    nrt_List *l;
    l = (nrt_List *) NRT_MALLOC(sizeof(nrt_List));
    if (!l)
    {
        /* Initialize the error and return NULL */
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        return NULL;
    }
    /* Null-initialize the link pointers */
    l->first = l->last = NULL;
    return l;
}

NRTAPI(nrt_List *) nrt_List_clone(nrt_List * source, NRT_DATA_ITEM_CLONE cloner,
                                  nrt_Error * error)
{
    nrt_List *l = NULL;
    if (source)
    {
        nrt_ListIterator iter = nrt_List_begin(source);
        nrt_ListIterator end = nrt_List_end(source);
        l = nrt_List_construct(error);
        if (!l)
            return NULL;

        while (nrt_ListIterator_notEqualTo(&iter, &end))
        {

            /* Foreach item in each list... */
            NRT_DATA *data = nrt_ListIterator_get(&iter);

            /* Use the function pointer to clone the object...  */
            NRT_DATA *newData = (NRT_DATA *) cloner(data, error);
            if (!newData)
                return NULL;

            /* ... and then insert it with the key into the new table */
            if (!nrt_List_pushBack(l, newData, error))
            {
                /* destruct the created list. NOTE - there is no way for us to
                 * destroy the NRT_DATA* chunks that have already been cloned */
                nrt_List_destruct(&l);
                return NULL;
            }

            nrt_ListIterator_increment(&iter);
        }
    }
    else
    {
        nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_OBJECT,
                        "Trying to clone NULL pointer");
    }

    return l;
}

NRTAPI(void) nrt_List_destruct(nrt_List ** this_list)
{
    NRT_DATA *data;
    if (*this_list)
    {
        /* Destroy each node in our list.  */
        while (!nrt_List_isEmpty(*this_list))
        {
            /* Pop one off the back and delete it */
            data = nrt_List_popBack(*this_list);
            /* delete it */
            if (data)
                NRT_FREE(data);
        }
        NRT_FREE(*this_list);
        *this_list = NULL;
    }

}

NRTAPI(nrt_ListIterator) nrt_List_begin(const nrt_List * list)
{
    nrt_ListIterator list_iterator;

    /* Be ruthless with our assertions */
    assert(list);
    list_iterator.current = list->first;

    return list_iterator;
}

NRTAPI(nrt_ListIterator) nrt_List_at(const nrt_List * chain, int i)
{
    nrt_ListIterator list_iterator = nrt_List_begin(chain);
    nrt_ListIterator end = nrt_List_end(chain);
    int j;
    for (j = 0; j < i; j++)
    {
        if (nrt_ListIterator_equals(&list_iterator, &end))
        {
            list_iterator.current = NULL;
            break;
        }
        nrt_ListIterator_increment(&list_iterator);
    }

    return list_iterator;
}

NRTAPI(NRT_BOOL) nrt_ListIterator_equals(const nrt_ListIterator * it1,
                                         const nrt_ListIterator * it2)
{
    return (it1->current == it2->current);
}

NRTAPI(NRT_BOOL) nrt_ListIterator_notEqualTo(const nrt_ListIterator * it1,
                                             const nrt_ListIterator * it2)
{
    return !nrt_ListIterator_equals(it1, it2);
}

NRTAPI(nrt_ListIterator) nrt_List_end(const nrt_List * this_list)
{
    nrt_ListIterator list_iterator;

    /* Silence compiler warnings about unused variables */
    (void)this_list;

    list_iterator.current = NULL;
    return list_iterator;
}

NRTAPI(NRT_DATA *) nrt_List_remove(nrt_List * list, nrt_ListIterator * where)
{
    NRT_DATA *data = NULL;
    nrt_ListNode *old = NULL;
    nrt_ListNode *new_current = NULL;

    /* Take care of special cases early */
    if (where->current == list->first)
    {
        data = nrt_List_popFront(list);
        *where = nrt_List_begin(list);
    }
    else if (where->current == list->last)
    {
        data = nrt_List_popBack(list);
        *where = nrt_List_end(list);

    }
    else
    {
        /* Make a pointer to the data right quick */
        data = where->current->data;

        /* Rewire the surrounding elements */
        old = where->current;
        new_current = old->next;

        new_current->prev = old->prev;
        old->prev->next = new_current;

        /* Reset the iterator to the NEXT */
        where->current = new_current;

        /* Now, destruct the listNode, but not the data */
        nrt_ListNode_destruct(&old);
    }
    /* Return what we saved */

    return data;
}

NRTAPI(NRT_BOOL) nrt_List_move(nrt_List * chain, uint32_t oldIndex,
                               uint32_t newIndex, nrt_Error * error)
{
    uint32_t listSize = nrt_List_size(chain);
    nrt_ListIterator iter;
    NRT_DATA *data = NULL;

    if (oldIndex == newIndex || oldIndex >= listSize)
    {
        nrt_Error_init(error, "Invalid list index specified", NRT_CTXT,
                       NRT_ERR_INVALID_PARAMETER);
        return NRT_FAILURE;
    }
    newIndex = newIndex > listSize ? listSize : newIndex;

    /* first, remove the data from the list */
    iter = nrt_List_at(chain, oldIndex);
    data = nrt_List_remove(chain, &iter);
    /* next, insert it at the new location */
    iter = nrt_List_at(chain, newIndex);
    return nrt_List_insert(chain, iter, data, error);
}

NRTAPI(NRT_BOOL) nrt_List_insert(nrt_List * list, nrt_ListIterator iter,
                                 NRT_DATA * data, nrt_Error * error)
{
    /* If our iterator is not set, we will insert the data into the back */
    if (!iter.current)
    {
        /* Push the data onto the back */
        if (!nrt_List_pushBack(list, data, error))
        {
            return 0;
        }
        /* Update the iterator to point at us */
        iter.current = list->last;
    }
    /* If the iterator is at the first */
    else if (iter.current == list->first)
    {
        /* Push data onto the front */
        if (!nrt_List_pushFront(list, data, error))
        {
            return 0;
        }
        /* Update the iterator to point at us */
        iter.current = list->first;
    }
    else
    {

        /* Construct a new node and insert it before the current */
        nrt_ListNode *new_node = nrt_ListNode_construct(iter.current->prev,
                                                        iter.current,
                                                        data,
                                                        error);

        /* If an error occurred, the list node captured it */
        if (!new_node)
        {
            /* Error already populated, go home */
            return 0;
        }

        /* Point the iterator at our new node */
        iter.current->prev->next = new_node;
        new_node->next->prev = new_node;
        iter.current = new_node;
    }
    return 1;
}

NRTAPI(void) nrt_ListIterator_increment(nrt_ListIterator * this_iter)
{
    if (this_iter->current)
        this_iter->current = this_iter->current->next;
}

NRTAPI(NRT_DATA *) nrt_ListIterator_get(nrt_ListIterator * this_iter)
{
    /* Lets make sure we are okay */
    assert(this_iter->current);
    return this_iter->current->data;
}

NRTAPI(uint32_t) nrt_List_size(nrt_List * list)
{
    uint32_t size = 0;

    if (list)
    {
        nrt_ListIterator iter = nrt_List_begin(list);
        nrt_ListIterator end = nrt_List_end(list);
        while (nrt_ListIterator_notEqualTo(&iter, &end))
        {
            ++size;
            nrt_ListIterator_increment(&iter);
        }
    }
    return size;
}
NRTAPI(uint16_t) nrt_List_size16(nrt_List* list)
{
    uint16_t size = 0;

    if (list)
    {
        nrt_ListIterator iter = nrt_List_begin(list);
        nrt_ListIterator end = nrt_List_end(list);
        while (nrt_ListIterator_notEqualTo(&iter, &end))
        {
            ++size;
            nrt_ListIterator_increment(&iter);
        }
    }
    return size;
}

NRTAPI(NRT_DATA *) nrt_List_get(nrt_List * list, int index, nrt_Error * error)
{
    int i = 0;
    if (list)
    {
        nrt_ListIterator iter = nrt_List_begin(list);
        nrt_ListIterator end = nrt_List_end(list);
        for (i = 0; i < index && nrt_ListIterator_notEqualTo(&iter, &end); ++i)
            nrt_ListIterator_increment(&iter);
        if (i == index && nrt_ListIterator_notEqualTo(&iter, &end))
        {
            return nrt_ListIterator_get(&iter);
        }
        else
        {
            nrt_Error_init(error, "Object not found at index", NRT_CTXT,
                           NRT_ERR_INVALID_OBJECT);
            return NULL;
        }
    }
    else
    {
        nrt_Error_init(error, "Invalid list -> NULL", NRT_CTXT,
                       NRT_ERR_INVALID_OBJECT);
        return NULL;
    }
}
