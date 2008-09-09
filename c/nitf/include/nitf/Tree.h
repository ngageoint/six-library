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

#ifndef __NITF_TREE_H__
#define __NITF_TREE_H__

#include "nitf/System.h"
#include "nitf/Error.h"
#include "nitf/List.h"

NITF_CXX_GUARD

/*!
 *  \struct nitf_TreeNode
 *  \brief A Tree Node in a nitf_Tree
 *
 *  This object provides the base node for a tree
 *  data structure (not to be confused with a TRE,
 *  or Tagged Record Extension).  Trees may be useful
 *  for certain kinds of parsing, and it may also at
 *  some point become the default storage for TREs 
 *  themselves.  Since a tree isnt really a flat data
 *  structure, traversal functions may be more appropriate
 *  than our typical
 *
 */
typedef struct _nitf_TreeNode
{

    struct _nitf_TreeNode* parent;

    /*! The child nodes List<TreeNode*> */
    nitf_List* children;

    /*! The data */
    NITF_DATA* data;
} nitf_TreeNode;

typedef struct _nitf_Tree
{
    /*! The root node */
    nitf_TreeNode *root;

} nitf_Tree;

/*!
 *  Construct a new (detached) TreeNode.  This function will assign
 *  its user data to the tree node, and will only return error if 
 *  memory cannot be allocated for the TreeNode (or its child list).
 *  The user is assumed to be responsible for the data associated with
 *  the node
 *
 *  \param data The data to encapsulate
 *  \param parent Parent (can be NULL if this is the root)
 *  \param error The error to return
 *
 */
NITFAPI(nitf_TreeNode *) nitf_TreeNode_construct(NITF_DATA* data,
                                                 nitf_Error* error);


/*!
 *  Destroy the current node and NULL set it.  We are not responsible
 *  for deleting the data in the node, it is up to the user to delete.
 *  
 *  \param node The node
 */
NITFAPI(void) nitf_TreeNode_destruct(nitf_TreeNode** node);

/*!
 *  Add a node to our children.  The child will be appended to the NITF
 *  list.  
 *
 *  \param node Our node
 *  \param child The node to add
 *  \param error The error
 *  \return NITF_SUCCESS on success, NITF_FAILURE on failure
 *
 */
NITFAPI(NITF_BOOL) nitf_TreeNode_addChild(nitf_TreeNode* node,
                                          nitf_TreeNode* child, 
                                          nitf_Error* error);

/*!
 *  Return if this tree node has children.  This is slightly easier than 
 *  going directly to the underlying list.
 *
 *  \param node The node
 *  \return 1 if we have children, 0, if not.
 */
NITFAPI(NITF_BOOL) nitf_TreeNode_hasChildren(nitf_TreeNode* node);

/*!
 *  Remove this child from the list.  We will remove our list element
 *  associated with this node from our Tree, if it exists.
 *
 *  Note, if there are multiple of the same node in this tree,
 *  we arent currently accounting for that, so if you knew that
 *  you had it, you would have to call this function repeatedly,
 *  e.g., while (nitf_TreeNode_remove(tree, node));
 */
NITFAPI(NITF_BOOL) nitf_TreeNode_removeChild(nitf_TreeNode* node,
                                             nitf_TreeNode* child);

/*!
 *  Clone our TreeNode into a new one.  We will need a clone function
 *  to tell us how to clone our NITF_DATA
 */
NITFAPI(nitf_TreeNode*) nitf_TreeNode_clone(nitf_TreeNode* source,
                                            NITF_DATA_ITEM_CLONE cloner,
                                            nitf_Error* error);


/*!
 *  Get ourselves a top-level data structure that can be used to control
 *  an integral tree.  It is okay to leave our root element as NULL.
 *  We will simply not initialize the root
 *
 *  \param An error.  
 *
 */
NITFAPI(nitf_Tree*) nitf_Tree_construct(nitf_TreeNode* root,
                                        nitf_Error* error);

/*!
 *  Destroy our tree
 *  \param tree The tree to destruct
 */
NITFAPI(void) nitf_Tree_destruct(nitf_Tree** tree);


typedef enum _nitf_Traversal 
{ 
    NITF_PRE_ORDER = 0,
    NITF_POST_ORDER
    /* We are not a binary tree, so no inorder traversal */
} nitf_Traversal;


/*!
 *  This is a traversal function.  When you call nitf_Tree_walk()
 *  you will give it the user data that is necessary to run this
 *  
 *  The node will be handed to this function by the traversal
 *  The function returns true or false if it succeeded or failed.
 *  If it failed, you should set the nitf_Error*.  On failure,
 *  we will short-circuit the walk method.
 *
 */
typedef NITF_BOOL (*NITF_TREE_TRAVERSER) (nitf_TreeNode *, 
                                          NITF_DATA* userData,
                                          int depth,
                                          nitf_Error *);


/*!
 *  Walk our tree using one of the traversal methods specified.
 *  Our function will stop if any traversal fails.  That means
 *  that the NITF_TREE_TRAVERSER is returning 1 as NITF_SUCCESS
 *  and 0 as NITF_FAILURE.  On NITF_FAILURE, we expect your onNode()
 *  to have returned us an error that we will hand back to the application
 *
 */
NITFAPI(NITF_BOOL) nitf_Tree_walk(nitf_Tree* tree, 
                                  NITF_TREE_TRAVERSER onNode,
                                  int traversalOrder,
                                  NITF_DATA* userData,
                                  nitf_Error* error);

/*!
 *  Only providing this because we have to.  It looks identical to the
 *  List clone function.
 *
 *  \param source The tree to clone
 *  \param cloner the clone function for the NITF_DATA in the element
 *  \param error An error to populate on failure
 *
 *  \return A nitf Tree that is the clone of source
 */
NITFAPI(nitf_Tree*) nitf_Tree_clone(nitf_Tree* source,
                                    NITF_DATA_ITEM_CLONE cloner,
                                    nitf_Error* error);

/*
 *  There are lots of other tree-type functions that I will ignore
 *  for now, but that would be useful, including sub-cloning, etc.
 */
NITF_CXX_ENDGUARD

#endif
