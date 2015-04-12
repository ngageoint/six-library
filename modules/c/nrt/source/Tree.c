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
 * License along with this program; 
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "nrt/Tree.h"

NRTPRIV(NRT_BOOL) preOrder(nrt_TreeNode * node, NRT_TREE_TRAVERSER onNode,
                           NRT_DATA * userData, int depth, nrt_Error * error)
{
    nrt_ListIterator where, end;
    nrt_List *list;
    assert(node);

    list = node->children;
    where = nrt_List_begin(list);
    end = nrt_List_end(list);
    if (!(*onNode) (node, userData, depth, error))
        return NRT_FAILURE;

    while (nrt_ListIterator_notEqualTo(&where, &end))
    {
        nrt_TreeNode *candidate = (nrt_TreeNode *) nrt_ListIterator_get(&where);
        if (!preOrder(candidate, onNode, userData, depth + 1, error))
            return NRT_FAILURE;
        nrt_ListIterator_increment(&where);
    }

    return NRT_SUCCESS;
}

NRTPRIV(NRT_BOOL) postOrder(nrt_TreeNode * node, NRT_TREE_TRAVERSER onNode,
                            NRT_DATA * userData, int depth, nrt_Error * error)
{
    nrt_ListIterator where, end;
    nrt_List *list;
    assert(node);

    list = node->children;
    where = nrt_List_begin(list);
    end = nrt_List_end(list);

    while (nrt_ListIterator_notEqualTo(&where, &end))
    {
        nrt_TreeNode *candidate = (nrt_TreeNode *) nrt_ListIterator_get(&where);

        if (!postOrder(candidate, onNode, userData, depth + 1, error))
            return NRT_FAILURE;

        nrt_ListIterator_increment(&where);
    }

    return (*onNode) (node, userData, depth, error);

}

NRTAPI(nrt_TreeNode *) nrt_TreeNode_construct(NRT_DATA * data,
                                              nrt_Error * error)
{
    nrt_TreeNode *node = (nrt_TreeNode *) NRT_MALLOC(sizeof(nrt_TreeNode));

    node->parent = NULL;

    if (node == NULL)
    {
        /* Init the error with the string value of errno */
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        /* Return if we have a problem */
        return NULL;
    }
    /* printf("Constructed node: %p\n", node); */
    /* This is a reference (see List.c or comments in Tree.h) */
    node->data = data;

    node->children = nrt_List_construct(error);
    if (!node->children)
    {
        /* There goes the show! */
        nrt_TreeNode_destruct(&node);
    }

    /* Return the new node */
    return node;

}

NRTPRIV(void) killNodes(nrt_TreeNode * node)
{
    nrt_ListIterator where, end;
    nrt_List *list;

    list = node->children;
    /* First thing we need to do is walk the list and find the child */
    where = nrt_List_begin(list);
    end = nrt_List_end(list);

    while (nrt_ListIterator_notEqualTo(&where, &end))
    {
        nrt_TreeNode *child = (nrt_TreeNode *) nrt_ListIterator_get(&where);
        if (child != node)
        {
            nrt_TreeNode_destruct(&child);
        }
        nrt_ListIterator_increment(&where);
    }
    NRT_FREE(node);
    /* printf("Destroyed node: %p\n", node); */
}

NRTAPI(void) nrt_TreeNode_destruct(nrt_TreeNode ** node)
{

    if (*node)
    {

        killNodes(*node);
        *node = NULL;
    }

}

NRTAPI(NRT_BOOL) nrt_TreeNode_addChild(nrt_TreeNode * node,
                                       nrt_TreeNode * child, nrt_Error * error)
{
    assert(node);
    assert(child);
    child->parent = node;
    /* printf("Adding %s to %s\n", (char*)child->data, (char*)node->data); */

    return nrt_List_pushBack(node->children, child, error);
}

NRTAPI(NRT_BOOL) nrt_TreeNode_hasChildren(nrt_TreeNode * node)
{
    assert(node);
    return nrt_List_isEmpty(node->children);

}

NRTAPI(NRT_BOOL) nrt_TreeNode_removeChild(nrt_TreeNode * node,
                                          nrt_TreeNode * child)
{
    nrt_ListIterator where, end;
    nrt_List *list;
    int found = 0;
    assert(node);
    assert(child);

    list = node->children;

    /* First thing we need to do is walk the list and find the child */
    where = nrt_List_begin(list);
    end = nrt_List_end(list);

    while (nrt_ListIterator_notEqualTo(&where, &end))
    {
        nrt_TreeNode *candidate = (nrt_TreeNode *) nrt_ListIterator_get(&where);
        if (candidate == node)
        {
            found = 1;
            break;
        }
        nrt_ListIterator_increment(&where);
    }

    /* Note, if there are multiple of the same node in this tree, we arent
     * currently accounting for that, so if you knew that you had it, you would 
     * * * * * * have to call this function repeatedly, e.g., while
     * (nrt_TreeNode_remove(tree, node)); */

    /* Then we simply remove that element */
    if (found)
    {
        child->parent = NULL;
        nrt_List_remove(list, &where);
    }
    return found;

}

NRTAPI(nrt_TreeNode *) nrt_TreeNode_clone(nrt_TreeNode * source,
                                          NRT_DATA_ITEM_CLONE cloner,
                                          nrt_Error * error)
{
    nrt_ListIterator where, end;
    nrt_List *list;

    nrt_TreeNode *clone = nrt_TreeNode_construct(NULL, error);
    assert(clone);
    clone->data = (*(cloner)) (source->data, error);

    list = source->children;

    /* First thing we need to do is walk the list and find the child */
    where = nrt_List_begin(list);
    end = nrt_List_end(list);

    while (nrt_ListIterator_notEqualTo(&where, &end))
    {
        nrt_TreeNode *sourceChild =
            (nrt_TreeNode *) nrt_ListIterator_get(&where);

        nrt_TreeNode *cloneChild =
            nrt_TreeNode_clone(sourceChild, cloner, error);

        nrt_TreeNode_addChild(clone, cloneChild, error);

        nrt_ListIterator_increment(&where);
    }
    return clone;
}

NRTAPI(nrt_Tree *) nrt_Tree_construct(nrt_TreeNode * root, nrt_Error * error)
{
    nrt_Tree *tree;

    tree = (nrt_Tree *) NRT_MALLOC(sizeof(nrt_Tree));
    if (!tree)
    {
        /* Initialize the error and return NULL */
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        return NULL;
    }
    tree->root = root;
    return tree;

}

NRTAPI(void) nrt_Tree_destruct(nrt_Tree ** tree)
{
    if (*tree)
    {
        nrt_TreeNode *root = (*tree)->root;
        nrt_TreeNode_destruct(&root);
        NRT_FREE(*tree);
        *tree = NULL;
    }

}

NRTAPI(NRT_BOOL) nrt_Tree_walk(nrt_Tree * tree, NRT_TREE_TRAVERSER onNode,
                               int traversalOrder, NRT_DATA * userData,
                               nrt_Error * error)
{
    if (traversalOrder == NRT_PRE_ORDER)
        return preOrder(tree->root, onNode, userData, 0, error);
    return postOrder(tree->root, onNode, userData, 0, error);

}

NRTAPI(nrt_Tree *) nrt_Tree_clone(nrt_Tree * source, NRT_DATA_ITEM_CLONE cloner,
                                  nrt_Error * error)
{
    nrt_TreeNode *root;
    assert(source);
    assert(source->root);
    root = nrt_TreeNode_clone(source->root, cloner, error);
    if (!root)
        return NULL;

    return nrt_Tree_construct(root, error);

}
