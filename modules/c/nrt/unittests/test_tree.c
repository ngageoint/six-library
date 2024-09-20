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

#include <import/nrt.h>
#include "Test.h"

static char *C(const char *p)
{
    const size_t p_sz = strlen(p) + 1;
    void* x_ = malloc(p_sz);
    char* x =
#if defined(__cplusplus)
     static_cast<char*>(x_);
#else
    x_;
#endif
    nrt_strcpy_s(x, p_sz, p);
    return x;
}

static NRT_BOOL deleteData(nrt_TreeNode * source, NRT_DATA * userData, int depth,
                    nrt_Error * error)
{
    (void)userData;
    (void)depth;
    (void)error;

    char *p = (char *) source->data;
    printf("Deleting %s\n", p);
    free(p);
    return 1;
}

void makeTree(nrt_Tree * t, const char* testName_)
{
    (void)testName_;
#define testName testName_ // "testName" can be a global which causes a compiler warning

    nrt_Error e;
    nrt_TreeNode *an, *ancho, *abso;
    t->root = nrt_TreeNode_construct(C("a"), &e);
    TEST_ASSERT(t->root != NULL);

    an = nrt_TreeNode_construct(C("an"), &e);
    TEST_ASSERT(an != NULL);

    nrt_TreeNode_addChild(t->root, an, &e);

    ancho = nrt_TreeNode_construct(C("ancho"), &e);

    nrt_TreeNode_addChild(an, ancho, &e);

    nrt_TreeNode_addChild(an, nrt_TreeNode_construct(C("anomaly"), &e), &e);

    nrt_TreeNode_addChild(ancho, nrt_TreeNode_construct(C("anchovy"), &e), &e);
    nrt_TreeNode_addChild(ancho, nrt_TreeNode_construct(C("anchor"), &e), &e);

    abso = nrt_TreeNode_construct(C("absolut"), &e);
    TEST_ASSERT(abso != NULL);

    nrt_TreeNode_addChild(t->root, abso, &e);

    nrt_TreeNode_addChild(abso, nrt_TreeNode_construct(C("absolute"), &e), &e);
    nrt_TreeNode_addChild(abso, nrt_TreeNode_construct(C("absolution"), &e),
                          &e);

}
#undef testName

static NRT_BOOL printElement(nrt_TreeNode * t, NRT_DATA * ud, int depth, nrt_Error * e)
{
    (void)ud;
    (void)e;

    const char *p = (const char *) t->data;
    int i;

    for (i = 0; i < depth; i++)
        printf("   ");
    printf("%s depth=\"%d\"\n", p, depth);
    return 1;
}

TEST_CASE(testTree)
{
    nrt_Error e;

    /* Create a tree - root can be passed during or after */
    nrt_Tree *t = nrt_Tree_construct(NULL, &e);
    nrt_Tree *tc = NULL;
    TEST_ASSERT(t != NULL);

    const char* testName_ = "testTree";
    makeTree(t, testName_);
    printf("Pre-order traversal:\n");
    printf("=======================================================\n");
    TEST_ASSERT(nrt_Tree_walk(t, &printElement, NRT_PRE_ORDER, NULL, &e));

    printf("\n\n");

    tc = nrt_Tree_clone(t, (NRT_DATA_ITEM_CLONE) & C, &e);
    TEST_ASSERT(tc != NULL);

    printf("Post-order traversal (cloned):\n");
    printf("=======================================================\n");
    TEST_ASSERT(nrt_Tree_walk(tc, &printElement, NRT_POST_ORDER, NULL, &e));

    TEST_ASSERT(nrt_Tree_walk(t, &deleteData, NRT_POST_ORDER, NULL, &e));
    TEST_ASSERT(nrt_Tree_walk(tc, &deleteData, NRT_POST_ORDER, NULL, &e));

    nrt_Tree_destruct(&t);
    nrt_Tree_destruct(&tc);

    TEST_ASSERT_NULL(t);
    TEST_ASSERT_NULL(tc);
}

TEST_MAIN(
    (void)argc;
    (void)argv;
    CHECK(testTree);
)
