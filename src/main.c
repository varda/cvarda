#include <stdlib.h>     // EXIT_SUCCESS

#include "../include/varda.h"   // VRD_*, vrd_*


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_AVL_Tree* restrict subset = vrd_AVL_tree_init(100);
    vrd_Cov_Table* restrict cov = vrd_Cov_table_init(100, 1000);
    vrd_Seq_Table* restrict seq = vrd_Seq_table_init(10000);
    vrd_SNV_Table* restrict snv = vrd_SNV_table_init(100, 1000);

    vrd_SNV_table_remove(snv, subset);


    vrd_SNV_table_destroy(&snv);
    vrd_Seq_table_destroy(&seq);
    vrd_Cov_table_destroy(&cov);
    vrd_AVL_tree_destroy(&subset);

    return EXIT_SUCCESS;
} // main
