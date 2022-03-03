#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "mytypes.h"
#include "lexem_analyze.h"
#include "executing.h"


struct node * parse_exp0(struct node ** arr, int * pos);

struct node *
parse_exp4(struct node ** arr, int * pos) {
    struct node * ans;
    if (arr[*pos]->op_type == ARGV) {
        (*pos)++;
        return arr[*pos - 1];
    }
    if (arr[*pos]->op_type == LEFT_BR) {
        (*pos)++;
        ans = parse_exp0(arr, pos);
        if (ans == NULL && arr[*pos]->op_type == RIGHT_BR) {
            fprintf(stderr, "Missing argument on %d position.\nTerminated program.\n", *pos);
            fflush(stdout);
            exit(1);
        }
        if (ans == NULL && arr[*pos]->op_type != RIGHT_BR) {
            fprintf(stderr, "Missing argument and right bracket on %d position.\nTerminated program.\n", *pos);
            fflush(stdout);
            exit(1);
        }
        if (arr[*pos]->op_type != RIGHT_BR) {
            fprintf(stderr, "Missing right bracket on %d position.\nTerminated program.\n", *pos);
            fflush(stdout);
            exit(1);
        }
        (*pos)++;
    } else {
        fprintf(stderr, "Incorrect symbol.\nTerminated program.\n");
        exit(1);
    }
    return ans;
}

struct node *
parse_exp3(struct node ** arr, int * pos) {
    struct node * ans, * left, * cur = NULL;
    left = parse_exp4(arr, pos);
    if (left == NULL) {
        return NULL;
    }
    if (arr[*pos]->op_type == WR || arr[*pos]->op_type == WR_APP || arr[*pos]->op_type == RD) {
        ans = arr[*pos];
        ans->left = left;
        (*pos)++;
        cur = ans;
        while (arr[*pos]->op_type == WR || arr[*pos]->op_type == WR_APP || arr[*pos]->op_type == RD) {
            cur->right = arr[*pos];
            cur = cur->right;
            cur->right = NULL;
            (*pos)++; 
        }
    } else {
        ans = left;
    }

    return ans;
}

struct node *
parse_exp2(struct node ** arr, int * pos) {
    struct node * ans, * left, * right;
    left = parse_exp3(arr, pos);
    if (left == NULL) {
        return NULL;
    }
    while (arr[*pos]->op_type == CONV) {
        ans = arr[*pos];
        (*pos)++;
        if (arr[*pos]->op_type == EOI) {
            fprintf(stderr, "Missing command in conveyer.\n");
            fflush(stdout);
            exit(1);
        }
        right = parse_exp3(arr, pos);
        if (right == NULL) {
            return NULL;
        }
        ans->right = right;
        ans->left = left;
        left = ans;
    }
    return left;
}

struct node *
parse_exp1(struct node ** arr, int * pos) {
    struct node * ans, * left, * right;
    left = parse_exp2(arr, pos);
    if (left == NULL) {
        return NULL;
    }
    while (arr[*pos]->op_type == LOG_OR || arr[*pos]->op_type == LOG_AND) {
        ans = arr[*pos];
        (*pos)++;
        if (arr[*pos]->op_type == EOI) {
            fprintf(stderr, "Missing operand after logic operation.\n");
            fflush(stdout);
            exit(1);
        }
        right = parse_exp2(arr, pos);
        if (right == NULL) {
            return NULL;
        }
        ans->right = right;
        ans->left = left;
        left = ans;
    }
    return left;
}

struct node *
parse_exp0(struct node ** arr, int * pos) {
    struct node * ans, * left, * right;
    if (arr[*pos]->op_type == EOI) {
        return NULL;
    }
    left = parse_exp1(arr, pos);
    if (left == NULL) {
        return NULL;
    }
    //оставил while, чтобы нормально функционировал break
    while (arr[*pos]->op_type == MUTE || arr[*pos]->op_type == SEMICOLON) {
        ans = arr[*pos];
        (*pos)++;
        if (arr[*pos]->op_type == EOI) {
            right = NULL;
            ans->right = right;
            ans->left = left;
            left = ans;
            break;
        }
        if (arr[*pos]->op_type == RIGHT_BR) {
            ans->left = left;
            ans->right = NULL;
            left = ans;
            break;
        }
        right = parse_exp0(arr, pos);
        if (right == NULL) {
            return NULL;
        }
        ans->right = right;
        ans->left = left;
        left = ans;
    }
    return left;
}

struct node *
parse_exp(struct node ** arr) {
    if (arr == NULL || arr[0]->op_type == EOI) {
        return NULL;
    }
    int pos = 0;
    struct node * ans;
    ans = parse_exp0(arr, &pos);
    if (arr[pos]->op_type != EOI) {
        ans = NULL;
    }
    return ans;
}