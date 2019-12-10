#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdio.h>      // FILE, fclose, fopen, fprintf, stderr
#include <stdlib.h>     // EXIT_*

#include "../include/varda.h"   // vrd_*


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_Trie* trie = vrd_trie_init();
    assert(NULL != trie);

    vrd_Trie_Node* elem = vrd_trie_insert(trie, 7, "romane", (void*) 1);
    assert(NULL != elem);

    elem = vrd_trie_insert(trie, 8, "romanus", (void*) 2);
    assert(NULL != elem);

    elem = vrd_trie_insert(trie, 8, "romulus", (void*) 3);
    assert(NULL != elem);

    elem = vrd_trie_insert(trie, 7, "rubens", (void*) 4);
    assert(NULL != elem);

    elem = vrd_trie_insert(trie, 6, "ruber", (void*) 5);
    assert(NULL != elem);

    elem = vrd_trie_insert(trie, 8, "rubicon", (void*) 6);
    assert(NULL != elem);

    elem = vrd_trie_insert(trie, 11, "rubicundus", (void*) 7);
    assert(NULL != elem);

    elem = vrd_trie_find(trie, 7, "romane");
    assert(NULL != elem && (void*) 1 == elem->data);

    elem = vrd_trie_find(trie, 8, "romanus");
    assert(NULL != elem && (void*) 2 == elem->data);

    elem = vrd_trie_find(trie, 8, "romulus");
    assert(NULL != elem && (void*) 3 == elem->data);

    elem = vrd_trie_find(trie, 7, "rubens");
    assert(NULL != elem && (void*) 4 == elem->data);

    elem = vrd_trie_find(trie, 6, "ruber");
    assert(NULL != elem && (void*) 5 == elem->data);

    elem = vrd_trie_find(trie, 8, "rubicon");
    assert(NULL != elem && (void*) 6 == elem->data);

    elem = vrd_trie_find(trie, 11, "rubicundus");
    assert(NULL != elem && (void*) 7 == elem->data);


    elem = vrd_trie_find(trie, 4, "rom");
    assert(NULL == elem);

    vrd_trie_destroy(&trie);

    return EXIT_SUCCESS;
} // main
