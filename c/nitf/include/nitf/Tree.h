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

#include "nrt/Tree.h"

typedef nrt_TreeNode                nitf_TreeNode;
typedef nrt_Tree                    nitf_Tree;

#define nitf_TreeNode_construct     nrt_TreeNode_construct
#define nitf_TreeNode_destruct      nrt_TreeNode_destruct
#define nitf_TreeNode_addChild      nrt_TreeNode_addChild
#define nitf_TreeNode_hasChildren   nrt_TreeNode_hasChildren
#define nitf_TreeNode_removeChild   nrt_TreeNode_removeChild
#define nitf_TreeNode_clone         nrt_TreeNode_clone
#define nitf_Tree_construct         nrt_Tree_construct
#define nitf_Tree_destruct          nrt_Tree_destruct

typedef nrt_Traversal               nitf_Traversal;
#define NITF_PRE_ORDER              NRT_PRE_ORDER
#define NITF_POST_ORDER             NRT_POST_ORDER
typedef NRT_TREE_TRAVERSER          NITF_TREE_TRAVERSER;
#define nitf_Tree_walk              nrt_Tree_walk
#define nitf_Tree_clone             nrt_Tree_clone

#endif
