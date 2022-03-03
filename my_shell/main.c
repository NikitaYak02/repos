#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mytypes.h"
#include "lexem_analyze.h"
#include "executing.h"
#include "build_tree.h"

int 
main(void) {
    /*
    1 - only is space
    0 - something else exist
    */
    int only_space_symbol = 1;
    char * instr = read_string(&only_space_symbol);
    if (only_space_symbol == 1) {
        free(instr);
        return 0;
    }
    struct node ** arr = creat_lexem_arr(instr);
    free(instr);
    keeper(arr);
    atexit(safety_exit);
    if (arr == NULL) {
        fprintf(stderr, "Same problem with input.\n");
        return 1;
    }
    struct node * vertex = parse_exp(arr);
    if (vertex == NULL) {
        //clean_node_arr(arr);
        return 1;
    }
    /*
    if (vertex != NULL) {
        print_tree(vertex);
    }
    */
    int exec_res = executing_tree(vertex);
    //printf("%d\n", exec_res);
    return exec_res;
}