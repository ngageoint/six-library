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

#ifndef __NRT_TREE_H__
#define __NRT_TREE_H__

#include "nrt/System.h"
#include "nrt/List.h"

NRT_CXX_GUARD
/*!
 *  \struct nrt_TreeNode
 *  \brief A Tree Node in a nrt_Tree
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
typedef struct _NRT_TreeNode
{

    struct _NRT_TreeNode *parent;

    /* ! The child nodes List<TreeNode*> */
    nrt_List *children;

    /* ! The data */
    NRT_DATA *data;
} nrt_TreeNode;

typedef struct _NRT_Tree
{
    /* ! The root node */
    nrt_TreeNode *root;

} nrt_Tree;

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
NRTAPI(nrt_TreeNode *) nrt_TreeNode_construct(NRT_DATA * data,
                                              nrt_Error * error);

/*!
 *  Destroy the current node and NULL set it.  We are not responsible
 *  for deleting the data in the node, it is up to the user to delete.
 *  
 *  \param node The node
 */
NRTAPI(void) nrt_TreeNode_destruct(nrt_TreeNode ** node);

/*!
 *  Add a node to our children.  The child will be appended to the NITF
 *  list.  
 *
 *  \param node Our node
 *  \param child The node to add
 *  \param error The error
 *  \return NRT_SUCCESS on success, NRT_FAILURE on failure
 *
 */
NRTAPI(NRT_BOOL) nrt_TreeNode_addChild(nrt_TreeNode * node,
                                       nrt_TreeNode * child, nrt_Error * error);

/*!
 *  Return if this tree node has children.  This is slightly easier than 
 *  going directly to the underlying list.
 *
 *  \param node The node
 *  \return 1 if we have children, 0, if not.
 */
NRTAPI(NRT_BOOL) nrt_TreeNode_hasChildren(nrt_TreeNode * node);

/*!
 *  Remove this child from the list.  We will remove our list element
 *  associated with this node from our Tree, if it exists.
 *
 *  Note, if there are multiple of the same node in this tree,
 *  we arent currently accounting for that, so if you knew that
 *  you had it, you would have to call this function repeatedly,
 *  e.g., while (nrt_TreeNode_remove(tree, node));
 */
NRTAPI(NRT_BOOL) nrt_TreeNode_removeChild(nrt_TreeNode * node,
                                          nrt_TreeNode * child);

/*!
 *  Clone our TreeNode into a new one.  We will need a clone function
 *  to tell us how to clone our NRT_DATA
 */
NRTAPI(nrt_TreeNode *) nrt_TreeNode_clone(nrt_TreeNode * source,
                                          NRT_DATA_ITEM_CLONE cloner,
                                          nrt_Error * error);

/*!
 *  Get ourselves a top-level data structure that can be used to control
 *  an integral tree.  It is okay to leave our root element as NULL.
 *  We will simply not initialize the root
 *
 *  \param An error.  
 *
 */
NRTAPI(nrt_Tree *) nrt_Tree_construct(nrt_TreeNode * root, nrt_Error * error);

/*!
 *  Destroy our tree
 *  \param tree The tree to destruct
 */
NRTAPI(void) nrt_Tree_destruct(nrt_Tree ** tree);

typedef enum _NRT_Traversal
{
    NRT_PRE_ORDER = 0,
    NRT_POST_ORDER
        /* We are not a binary tree, so no inorder traversal */
} nrt_Traversal;

/*!
 *  This is a traversal function.  When you call nrt_Tree_walk()
 *  you will give it the user data that is necessary to run this
 *  
 *  The node will be handed to this function by the traversal
 *  The function returns true or false if it succeeded or failed.
 *  If it failed, you should set the nrt_Error*.  On failure,
 *  we will short-circuit the walk method.
 *
 */
typedef NRT_BOOL(*NRT_TREE_TRAVERSER) (nrt_TreeNode *, NRT_DATA * userData,
                                       int depth, nrt_Error *);

/*!
 *  Walk our tree using one of the traversal methods specified.
 *  Our function will stop if any traversal fails.  That means
 *  that the NRT_TREE_TRAVERSER is returning 1 as NRT_SUCCESS
 *  and 0 as NRT_FAILURE.  On NRT_FAILURE, we expect your onNode()
 *  to have returned us an error that we will hand back to the application
 *
 */
NRTAPI(NRT_BOOL) nrt_Tree_walk(nrt_Tree * tree, NRT_TREE_TRAVERSER onNode,
                               int traversalOrder, NRT_DATA * userData,
                               nrt_Error * error);

/*!
 *  Only providing this because we have to.  It looks identical to the
 *  List clone function.
 *
 *  \param source The tree to clone
 *  \param cloner the clone function for the NRT_DATA in the element
 *  \param error An error to populate on failure
 *
 *  \return A Tree that is the clone of source
 */
NRTAPI(nrt_Tree *) nrt_Tree_clone(nrt_Tree * source, NRT_DATA_ITEM_CLONE cloner,
                                  nrt_Error * error);

/*
 *  There are lots of other tree-type functions that I will ignore
 *  for now, but that would be useful, including sub-cloning, etc.
 */
NRT_CXX_ENDGUARD
#endif
