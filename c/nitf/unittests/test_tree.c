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

#include <import/nitf.h>
#include "Test.h"

char* C(const char* p)
{
    char* x = malloc(strlen(p) + 1);
    strcpy(x, p);
    return x;
}

NITF_BOOL deleteData(nitf_TreeNode *source, NITF_DATA* userData, int depth,
        nitf_Error *error)
{
    char* p = (char*) source->data;
    printf("Deleting %s\n", p);
    free(p);
    return 1;
}

void makeTree(nitf_Tree* t, const char* testName)
{
    nitf_Error e;
    nitf_TreeNode* an, *ancho, *abso;
    t->root = nitf_TreeNode_construct(C("a"), &e);
    TEST_ASSERT(t->root);

    an = nitf_TreeNode_construct(C("an"), &e);
    TEST_ASSERT(an);

    nitf_TreeNode_addChild(t->root, an, &e);

    ancho = nitf_TreeNode_construct(C("ancho"), &e);

    nitf_TreeNode_addChild(an, ancho, &e);

    nitf_TreeNode_addChild(an, nitf_TreeNode_construct(C("anomaly"), &e), &e);

    nitf_TreeNode_addChild(ancho, nitf_TreeNode_construct(C("anchovy"), &e), &e);
    nitf_TreeNode_addChild(ancho, nitf_TreeNode_construct(C("anchor"), &e), &e);

    abso = nitf_TreeNode_construct(C("absolut"), &e);
    TEST_ASSERT(abso);

    nitf_TreeNode_addChild(t->root, abso, &e);

    nitf_TreeNode_addChild(abso, nitf_TreeNode_construct(C("absolute"), &e), &e);
    nitf_TreeNode_addChild(abso, nitf_TreeNode_construct(C("absolution"), &e),
                           &e);

}

NITF_BOOL printElement(nitf_TreeNode* t, NITF_DATA* ud, int depth,
        nitf_Error* e)
{
    const char* p = (const char*) t->data;
    int i;

    for (i = 0; i < depth; i++)
        printf("   ");
    printf("%s depth=\"%d\"\n", p, depth);
    return 1;
}

TEST_CASE(testTree)
{
    nitf_Error e;

    /*  Create a tree - root can be passed during or after*/
    nitf_Tree *t = nitf_Tree_construct(NULL, &e);
    nitf_Tree *tc = NULL;
    TEST_ASSERT(t);

    makeTree(t, testName);
    printf("Pre-order traversal:\n");
    printf("=======================================================\n");
    TEST_ASSERT(nitf_Tree_walk(t, &printElement, NITF_PRE_ORDER, NULL, &e));

    printf("\n\n");

    tc = nitf_Tree_clone(t, (NITF_DATA_ITEM_CLONE) & C, &e);
    TEST_ASSERT(tc);

    printf("Post-order traversal (cloned):\n");
    printf("=======================================================\n");
    TEST_ASSERT(nitf_Tree_walk(tc, &printElement, NITF_POST_ORDER, NULL, &e));

    TEST_ASSERT(nitf_Tree_walk(t, &deleteData, NITF_POST_ORDER, NULL, &e));
    TEST_ASSERT(nitf_Tree_walk(tc, &deleteData, NITF_POST_ORDER, NULL, &e));

    nitf_Tree_destruct(&t);
    nitf_Tree_destruct(&tc);

    TEST_ASSERT_NULL(t);
    TEST_ASSERT_NULL(tc);
}

int main(int argc, char **argv)
{
    CHECK(testTree);
    return 0;
}
