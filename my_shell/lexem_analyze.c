#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include "mytypes.h"

/*
управляющие символы:
>, >>, <, |, ||, &, &&, (, )
*/
int
iscontrlsym(char sym) {
    if (sym == '&' || sym == '|' || sym == '<' || sym == '>' || sym == '(' || sym == ')' || sym == ';') {
        return 1;
    } else {
        return 0;
    }
}

struct node *
get_lexem(char * inputstr, int *pos) {
    char * cur_str = inputstr + *pos;
    //printf("sdytsuiguyiyguiwerguyiwerygiuwfygiuweriuyweiufygweiyguweiygfiuwygeuiygwui%s\n", inputstr);
    //printf("%s\n", cur_str);
    while (isspace(*cur_str)) {
        (*pos)++;
        cur_str++;
    }
    //printf("%s\n", inputstr);
    //printf("cur_str without skipped spaces: %s\n", cur_str);

    struct node * ans = malloc(sizeof(struct node));
    ans->left = NULL;
    ans->right = NULL;
    
    if (*cur_str == '\0') {
        ans->op_type = EOI;
        return ans;
    }
    if (*cur_str == '(') {
        (*pos)++;
        ans->op_type = LEFT_BR;
        return ans;
    }
    if (*cur_str == ')') {
        (*pos)++;
        ans->op_type = RIGHT_BR;
        return ans;
    }
    if (*cur_str == ';') {
        (*pos)++;
        ans->op_type = SEMICOLON;
        return ans;
    }

    //поиск перенапрвлений
    char * pointer = strstr(cur_str, ">>");

    if ((pointer != NULL && pointer == cur_str) || *cur_str == '>' || *cur_str == '<') {
        //printf("cur_str without skipped spaces: %s\n", cur_str);
        if (pointer != NULL && pointer == cur_str) {
            cur_str += 2;
            ans->op_type = WR_APP;
        } else if (*cur_str == '>'){
            cur_str++;
            ans->op_type = WR;
        } else {
            cur_str++;
            ans->op_type = RD;
        }
        while (isspace(*cur_str)) {
            (*pos)++;
            cur_str++;
        }
        int name_len = 0, mult = 2;
        ans->path = malloc(mult * sizeof(char));
        while (!isspace(*cur_str) && *cur_str != '\0' && !iscontrlsym(*cur_str)) {
            if (name_len >= name_len - 1) {
                mult *= 2;
                ans->path = realloc(ans->path, mult * sizeof(char));
                if (ans->path == NULL) {
                    perror("get_lexem: ");
                    free(inputstr);
                    exit(1);
                    //функция очистки дерева созданного
                }
            }
            (ans->path)[name_len] = *cur_str;
            name_len++;
            cur_str++;
        }

        //обновляем позицию
        *pos = cur_str - inputstr;
        (ans->path)[name_len] = '\0';
        if (name_len == 0) {
            free(ans->path);
            free(ans);
            ans = NULL;
        }
        return ans;
    }

    pointer = strstr(cur_str, "||");
    if (pointer != NULL && pointer == cur_str) {
        *pos = *pos + 2;
        ans->op_type = LOG_OR;
        return ans;
    }

    if (*cur_str == '|') {
        (*pos)++;
        ans->op_type = CONV;
        return ans;
    }

    pointer = strstr(cur_str, "&&");
    if (pointer != NULL && pointer == cur_str) {
        *pos = *pos + 2;
        ans->op_type = LOG_AND;
        return ans;
    }

    if (*cur_str == '&') {
        (*pos)++;
        ans->op_type = MUTE;
        return ans;
    }

    int argc = 0, argv_mult = 2;
    char ** argv = malloc(argv_mult * sizeof(struct node *));
    while (*cur_str != '\0' && !iscontrlsym(*cur_str)) {
        if (argc >= argv_mult - 1) {
            argv_mult *= 2;
            argv = realloc(argv, argv_mult * sizeof(struct node *));
            if (argv == NULL) {
                perror("get_lexem: ");
                free(inputstr);
                exit(1);
            }
        }
        while (isspace(*cur_str)) {
            cur_str++;
        }
        int wordl_len = 0, mult = 2;
        argv[argc] = malloc(mult * sizeof(char));
        while (*cur_str != '\0' && !iscontrlsym(*cur_str) && !isspace(*cur_str)) {
            if (wordl_len >= mult - 1) {
                mult *= 2;
                argv[argc] = realloc(argv[argc], mult * sizeof(char));
                if (argv[argc] == NULL) {
                    perror("get_lexem: ");
                    free(inputstr);
                    exit(1);
                    //удаление деерва
                }
            } 
            argv[argc][wordl_len] = *cur_str;
            cur_str++;
            wordl_len++;
        }
        argv[argc][wordl_len] = '\0';
        argc++;
        while (isspace(*cur_str)) {
            cur_str++;
        }
    }
    *pos = cur_str - inputstr;
    argv[argc] = NULL;
    ans->argv = argv;
    ans->argc = argc;
    ans->op_type = ARGV;    
    return ans;
}

void
delete_node(struct node * vertex) {
    if (vertex == NULL || vertex->op_type == EOI) {
        return;
    }
    /*
    if (vertex->left != NULL) {
        delete_node(vertex->left);
        free(vertex->left);
    }
    if (vertex->right != NULL) {
        delete_node(vertex->right);
        free(vertex->right);
    }
    */
    if (vertex->op_type == WR_APP) {
        free(vertex->path);
        return;
    }
    if (vertex->op_type == WR) {
        free(vertex->path);
        return;
    }
    if (vertex->op_type == RD) {
        free(vertex->path);
        return;
    }
    if (vertex->op_type == ARGV) {
        int i = 0;
        while (vertex->argv[i] != NULL) {
            free(vertex->argv[i]);
            i++;
        }
        free(vertex->argv);
        return;
    }
}

void
print_node (struct node * vertex) {
    /*
    || - LOG_OR
    && - LOG_AND
    | - CONV
    & - MUTE
    ; - SEMICOLON
    >> - WR_APP
    > - WR
    < - RD
    argv - ARGV
    */
    //printf("PRINT: %d\n", vertex->op_type);
    if (vertex->op_type == LOG_OR) {
        printf(" || ");
        return;
    }
    if (vertex->op_type == LOG_AND) {
        printf(" && ");
        return;
    }
    if (vertex->op_type == CONV) {
        printf(" | ");
        return;
    }
    if (vertex->op_type == MUTE) {
        printf(" & ");
        return;
    }
    if (vertex->op_type == SEMICOLON) {
        printf(" ; ");
        return;
    }
    if (vertex->op_type == WR_APP) {
        printf(" >> ");
        printf("%s ", vertex->path);
        return;
    }
    if (vertex->op_type == WR) {
        printf(" > ");
        printf("%s ", vertex->path);
        return;
    }
    if (vertex->op_type == RD) {
        printf(" < ");
        printf("%s ", vertex->path);
        return;
    }
    if (vertex->op_type == ARGV) {
        int i = 0;
        while (vertex->argv[i] != NULL) {
            printf("%s ", vertex->argv[i]);
            i++;
        }
        return;
    }
    //printf("PRINT.\n");
}

void
print_tree(struct node * vertex) {
    print_node(vertex);
    fflush(stdout);
    if (vertex->left != NULL) {
        print_tree(vertex->left);
    }
    if (vertex->right != NULL) {
        print_tree(vertex->right);
    }
}

char *
read_string(int * only_space_symbol) {
    char str[MAXLEN + 1];
    int mult = MAXLEN + 1, inlen = 0, curstrlen = 0;
    char *inputstr = malloc(mult * sizeof(char));

    inputstr[inlen] = '\0';
    while (fgets(str, MAXLEN + 1, stdin) != NULL) {
        curstrlen = strlen(str);
        if (inlen + curstrlen >= mult) {
            mult *= 2;
            inputstr = realloc(inputstr, mult * sizeof(char));
        }
        strcat(inputstr, str);
        inlen += curstrlen; 
    }
    *only_space_symbol = 1;
    for (int i = 0; i < strlen(inputstr); i++) {
        if (isspace(inputstr[i]) == 0) {
            *only_space_symbol = 0;
            break;
        }
    }
    //printf("string is read: %s\n", inputstr);
    fflush(stdout);
    return inputstr;
}

struct node **
creat_lexem_arr(char * inputstr) {
    int arr_len = 0, mult = 2, pos = 0;
    struct node * vertex;
    struct node ** ans = malloc(mult * sizeof(struct node *));
    while ((vertex = get_lexem(inputstr, &pos)) != NULL) {
        //printf("\n cycle %d\n", arr_len);
        if (mult <= arr_len) {
            mult *= 2;
            ans = realloc(ans, mult * sizeof(struct node *));
        }
        //print_node(vertex);
        ans[arr_len] = vertex;
        arr_len++;
        if (vertex->op_type == EOI) {
            break;
        }
    }
    if (vertex == NULL) {
        for (int i = 0; i < arr_len; i++) {
            delete_node(ans[i]);
            free(ans[i]);
            ans[i] = NULL;
        }
        free(ans);
        ans = NULL;
    }
    return ans;
}

void
clean_node_arr(struct node ** arr) {
    int i = 0;
    while (arr[i]->op_type != EOI) {
        delete_node(arr[i]);
        free(arr[i]);
        arr[i] = NULL;
        i++;
    }
    delete_node(arr[i]);
    free(arr[i]);
    free(arr);
}