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

#include "nitf/Tree.h"


NITFPRIV(NITF_BOOL) preOrder(nitf_TreeNode* node, 
                             NITF_TREE_TRAVERSER onNode,
                             NITF_DATA* userData,
                             int depth,
                             nitf_Error* error)
{
    nitf_ListIterator where, end;
    nitf_List* list;
    assert(node);
    
    list = node->children;
    where = nitf_List_begin(list);
    end = nitf_List_end(list);
    if (!(*onNode)(node, userData, depth, error))
        return NITF_FAILURE;

    
    while ( nitf_ListIterator_notEqualTo(&where, &end) )
    {
        nitf_TreeNode* candidate = 
            (nitf_TreeNode*)nitf_ListIterator_get(&where);
        if (!preOrder(candidate, onNode, userData, depth + 1, error))
            return NITF_FAILURE;
        nitf_ListIterator_increment(&where);
    }

    return NITF_SUCCESS;
}



NITFPRIV(NITF_BOOL) postOrder(nitf_TreeNode* node, 
                              NITF_TREE_TRAVERSER onNode,
                              NITF_DATA* userData,
                              int depth,
                              nitf_Error* error)
{
    nitf_ListIterator where, end;
    nitf_List* list;
    assert(node);
    
    list = node->children;
    where = nitf_List_begin(list);
    end = nitf_List_end(list);

    while ( nitf_ListIterator_notEqualTo(&where, &end) )
    {
        nitf_TreeNode* candidate = 
            (nitf_TreeNode*)nitf_ListIterator_get(&where);

        if (!postOrder( candidate, onNode, userData, depth + 1, error ))
            return NITF_FAILURE;
        
        nitf_ListIterator_increment(&where);
    }
    
    return (*onNode)(node, userData, depth, error);

}


NITFAPI(nitf_TreeNode *) nitf_TreeNode_construct(NITF_DATA* data,
                                                 nitf_Error* error)
{
    nitf_TreeNode *node =
        (nitf_TreeNode *) NITF_MALLOC(sizeof(nitf_TreeNode));

    node->parent = NULL;

    if (node == NULL)
    {
        /*  Init the error with the string value of errno  */
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        /*  Return if we have a problem  */
        return NULL;
    }
    /*printf("Constructed node: %p\n", node);*/
    /* This is a reference (see List.c or comments in Tree.h) */
    node->data = data;

    node->children = nitf_List_construct(error);
    if (!node->children)
    {
        /*  There goes the show! */
        nitf_TreeNode_destruct(&node);
    }


    /*  Return the new node  */
    return node;

}

NITFPRIV(void) killNodes(nitf_TreeNode* node)
{
    nitf_ListIterator where, end;
    nitf_List* list;

    list = node->children;
    /* First thing we need to do is walk the list and find the child */
    where = nitf_List_begin(list);
    end = nitf_List_end(list);
    
    while ( nitf_ListIterator_notEqualTo(&where, &end) )
    {
        nitf_TreeNode* child = 
            (nitf_TreeNode*)nitf_ListIterator_get(&where);
        if (child != node)
        {
            nitf_TreeNode_destruct(&child);
        }
        nitf_ListIterator_increment(&where);
    }
    NITF_FREE(node);
    /*printf("Destroyed node: %p\n", node);*/
}

NITFAPI(void) nitf_TreeNode_destruct(nitf_TreeNode** node)
{


    if (*node)
    {

        killNodes(*node);
        *node = NULL;
    }

}

NITFAPI(NITF_BOOL) nitf_TreeNode_addChild(nitf_TreeNode* node,
                                          nitf_TreeNode* child, 
                                          nitf_Error* error)
{
    assert(node);
    assert(child);
    child->parent = node;
    /*printf("Adding %s to %s\n", (char*)child->data, (char*)node->data);*/

    return nitf_List_pushBack(node->children, child, error);
}

NITFAPI(NITF_BOOL) nitf_TreeNode_hasChildren(nitf_TreeNode* node)
{
    assert(node);
    return nitf_List_isEmpty(node->children);

}

NITFAPI(NITF_BOOL) nitf_TreeNode_removeChild(nitf_TreeNode* node,
                                             nitf_TreeNode* child)
{
    nitf_ListIterator where, end;
    nitf_List* list;
    int found = 0;
    assert(node);
    assert(child);

    list = node->children;

    /* First thing we need to do is walk the list and find the child */
    where = nitf_List_begin(list);
    end = nitf_List_end(list);
    
    while ( nitf_ListIterator_notEqualTo(&where, &end) )
    {
        nitf_TreeNode* candidate = 
            (nitf_TreeNode*)nitf_ListIterator_get(&where);
        if (candidate == node)
        {
            found = 1;
            break;
        }
        nitf_ListIterator_increment(&where);
    }

    /* Note, if there are multiple of the same node in this tree,
       we arent currently accounting for that, so if you knew that
       you had it, you would have to call this function repeatedly,
       e.g., while (nitf_TreeNode_remove(tree, node));
     */

    /* Then we simply remove that element */
    if (found) 
    {
        child->parent = NULL;
        nitf_List_remove(list, &where);
    }
    return found;

}

NITFAPI(nitf_TreeNode*) nitf_TreeNode_clone(nitf_TreeNode* source,
                                            NITF_DATA_ITEM_CLONE cloner,
                                            nitf_Error* error)
{
    nitf_ListIterator where, end;
    nitf_List* list;

    nitf_TreeNode* clone = nitf_TreeNode_construct(NULL, error);
    assert(clone);
    clone->data = ( *(cloner) )(source->data, error);

    list = source->children;

    /* First thing we need to do is walk the list and find the child */
    where = nitf_List_begin(list);
    end = nitf_List_end(list);
    
    while ( nitf_ListIterator_notEqualTo(&where, &end) )
    {
        nitf_TreeNode* sourceChild = 
            (nitf_TreeNode*)nitf_ListIterator_get(&where);

        nitf_TreeNode* cloneChild = 
            nitf_TreeNode_clone(sourceChild, cloner, error);
        
        nitf_TreeNode_addChild(clone, cloneChild, error);

        nitf_ListIterator_increment(&where);
    }
    return clone;
}

NITFAPI(nitf_Tree*) nitf_Tree_construct(nitf_TreeNode* root,
                                        nitf_Error* error)
{
    nitf_Tree* tree;

    tree = (nitf_Tree*) NITF_MALLOC( sizeof( nitf_Tree ) );
    if (!tree)
    {
       /*  Initialize the error and return NULL  */
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }
    tree->root = root;
    return tree;
    

}

NITFAPI(void) nitf_Tree_destruct(nitf_Tree** tree)
{
    if (*tree)
    {
        nitf_TreeNode* root = (*tree)->root;
        nitf_TreeNode_destruct( &root );
        NITF_FREE( *tree );
        *tree = NULL;
    }

}



NITFAPI(NITF_BOOL) nitf_Tree_walk(nitf_Tree* tree, 
                                  NITF_TREE_TRAVERSER onNode,
                                  int traversalOrder,
                                  NITF_DATA* userData,
                                  nitf_Error* error)
{
    if (traversalOrder == NITF_PRE_ORDER)
        return preOrder(tree->root, onNode, userData, 0, error);
    return postOrder(tree->root, onNode, userData, 0, error);
            
}

NITFAPI(nitf_Tree*) nitf_Tree_clone(nitf_Tree* source,
                                    NITF_DATA_ITEM_CLONE cloner,
                                    nitf_Error* error)
{
    nitf_TreeNode* root;
    assert(source);
    assert(source->root);
    root = nitf_TreeNode_clone(source->root, cloner, error);
    if (!root)
        return NULL;

    return nitf_Tree_construct(root, error);

}
