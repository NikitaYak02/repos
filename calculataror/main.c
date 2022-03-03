#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

enum LEXEM_TYPE{
    OP_PLUS = 0,
    OP_SUB = 1,
    OP_MUL = 2,
    OP_DIV = 3,
    NUMBER = 4,
    VARIABLE = 5,
    LEFT_BR = 6,
    RIGHT_BR = 7,
    EOI = 8
};

struct node {
    enum LEXEM_TYPE lex;
    double value;
    char str[7];
    struct node * left_node;
    struct node * right_node;
};

struct variable {
    char str[7];
    double value;
    int wr;
};

struct node *parse_expr0(struct node ** node_mas, int * pos);
struct node *parse_expr1(struct node ** node_mas, int * pos);
struct node *parse_expr2(struct node ** node_mas, int * pos);

void
skip_spaces(char ** input_str) {
    while (isspace(**input_str) && **input_str != '\n') {
        (*input_str)++;
    }
}

char *
read_string(void) {
    long mul = 2, num_char = 0;
    char *input_str = malloc(sizeof(char) * mul);
    int c;
    while (((c = getchar()) != EOF )&&(c != '\n')) {
        if (num_char >= mul - 1) {
            mul *= 2;
            input_str = realloc(input_str, mul);
        }
        if (input_str == NULL) {
            fprintf(stderr, "Not enough memory to store expression.\n");
            _Exit(1);
        }
        input_str[num_char] = c;
        num_char++;
    }
    if (num_char == 0 || c == EOF) {
        fprintf(stderr, "String doesn't ended with backslash n.\n");
        free(input_str);
        return NULL;
    }
    input_str[num_char] = '\0';
    return input_str;
}

void
delete_node_mas(struct node ** mas, long cur_mas_len) {
    if (mas == NULL) {
        return;
    }
    long t;
    int val;
    for (t = 0; t <= cur_mas_len; t++) {
        free(mas[t]);
    }
    t = 0;
    if (cur_mas_len == -1) {
        do {
            val = mas[t]->lex;
            free(mas[t]);
            t++;
        } while (val != EOI);
    }
}

struct node **
lexem_analize (char * input_str) {
    //array of lexems
    if (input_str == NULL) {
        return NULL;
    }
    struct node ** node_mas;
    /*
    1) current allocated number of record for node_mas
    2) current node_mas size
    4) current variable number
    5) current allocated number of record for var_mas
    */
    int mul = 2, cur_mas_len = 0, bracket_balance = 0;
    node_mas = malloc(mul * sizeof(struct node *));
    skip_spaces(&input_str);
    while (*input_str != '\0') {
        if (bracket_balance < 0) {
            break;
        }
        if (cur_mas_len  + 1 >= mul) {
            mul *= 2;
            node_mas = realloc(node_mas, mul * sizeof(struct node *));
        }
        node_mas[cur_mas_len] = malloc(sizeof(struct node));
        node_mas[cur_mas_len]->left_node = NULL;
        node_mas[cur_mas_len]->right_node = NULL;
        switch ( *input_str) {
            case '(':
                bracket_balance++;
                node_mas[cur_mas_len]->lex = LEFT_BR;
                break;
            case ')':
                bracket_balance--;
                node_mas[cur_mas_len]->lex = RIGHT_BR;
                break;
            case '+':
                node_mas[cur_mas_len]->lex = OP_PLUS;
                break;
            case '-':
                node_mas[cur_mas_len]->lex = OP_SUB;
                break;
            case '*':
                node_mas[cur_mas_len]->lex = OP_MUL;
                break;
            case '/':
                node_mas[cur_mas_len]->lex = OP_DIV;
                break;
            default:
                if (isalpha(*input_str) || *input_str == '_') {
                    node_mas[cur_mas_len]->lex = VARIABLE;
                    int str_len = 0;
                    //считывание имени переменной
                    while ((isalpha(*input_str) || *input_str == '_' || isdigit(*input_str)) && str_len < 7) {
                        (node_mas[cur_mas_len]->str)[str_len] = *input_str;
                        str_len++;
                        input_str++;
                    }
                    //выход, если длина имени больше 6
                    if (str_len == 7) {
                        fprintf(stderr, "%s\n", "Too long variable name. Terminated program.\n");
                        delete_node_mas(node_mas, cur_mas_len);
                        free(node_mas);
                        return NULL;
                    } else {
                        (node_mas[cur_mas_len]->str)[str_len] = '\0';
                    }
                } else if (isdigit(*input_str)|| (*input_str == '.' && isdigit(*(input_str + 1)))) {
                    char buf[14], *temp;
                    int len1 = 0, len2 = 0, is_dots = 0;
                    while (isdigit(*input_str) && *input_str != '.' && len1 < 7) {
                        buf[len1] = *input_str;
                        input_str++;
                        len1++;
                    }

                    if (*input_str == '.' && len1 < 7) {
                        buf[len1] = *input_str;
                        is_dots = 1;
                        input_str++;
                    }

                    while (isdigit(*input_str)&& len2 < 7) {
                        buf[len1 + is_dots + len2] = *input_str;
                        input_str++;
                        len2++;
                    }

                    if (len1 == 7 || !is_dots || len2 == 7) {
                        fprintf(stderr, "Incorrect number to store in double variable\n");
                        delete_node_mas(node_mas, cur_mas_len);
                        free(node_mas);
                        return NULL;
                    }
                    buf[len1 + is_dots + len2] = '\0';
                    node_mas[cur_mas_len]->lex = NUMBER;
                    node_mas[cur_mas_len]->value = strtod(buf, &temp);
                } else if (*input_str != '\0') {
                    fprintf(stderr, "Incorrect symbol combination from %s", input_str);
                    delete_node_mas(node_mas, cur_mas_len);
                    free(node_mas);
                    return NULL;
                }
                input_str--;
        }
        cur_mas_len++;
        input_str++;
        skip_spaces(&input_str);
    }
    if (cur_mas_len  + 1 >= mul) {
        mul *= 2;
        node_mas = realloc(node_mas, mul * sizeof(struct node *));
    }
    node_mas[cur_mas_len] = malloc(sizeof(struct node));
    node_mas[cur_mas_len]->left_node = NULL;
    node_mas[cur_mas_len]->right_node = NULL;
    node_mas[cur_mas_len]->lex = EOI;
    if (bracket_balance < 0) {
        fprintf(stderr, "Right bracket without left bracket.\n");
        delete_node_mas(node_mas, -1);
        free(node_mas);
        node_mas = NULL;
        return NULL;
    }
    if (bracket_balance > 0) {
        fprintf(stderr, "Left bracket without right bracket.\n");
        delete_node_mas(node_mas, -1);
        free(node_mas);
        node_mas = NULL;
        return NULL;
    }
    return node_mas;
}

struct variable **
create_var_mas(struct node ** node_mas, int *rez_len) {
    if (node_mas == NULL) {
        return NULL;
    }
    int num = 0, mul = 2, t;
    struct variable ** ans = malloc(mul * sizeof(struct variable *));
    while ((*node_mas)->lex != EOI) {
        if ((*node_mas)->lex == VARIABLE) {
            for (t = 0; t < num; t++) {
                if (strcmp((*node_mas)->str, ans[t]->str) == 0) {
                    break;
                }
            }
            if (t < num) {
                node_mas++;
                continue;
            }
            if (num >= mul) {
                mul *= 2;
                ans = realloc(ans, mul * sizeof(struct variable *));
            }
            ans[num] = malloc(sizeof(struct variable));
            for (t = 0; t < 7; t++){
                (ans[num]->str)[t] = ((*node_mas)->str)[t];
                if (((*node_mas)->str)[t] == '\0') {
                    break;
                }
            }
            ans[num]->wr = 0;
            num++;
        }
        node_mas++;
    }
    if (num == 0) {
        free(ans);
        ans = NULL;
    }
    *rez_len = num;
    return ans;
}

struct node *
parse_expr2(struct node ** node_mas, int * pos) {
    struct node * ans;
    if (node_mas[*pos]->lex == NUMBER || node_mas[*pos]->lex == VARIABLE) {
        ans = node_mas[*pos];
        (*pos)++;
    } else if (node_mas[*pos]->lex == LEFT_BR) {
        (*pos)++;
        ans = parse_expr0(node_mas, pos);
        if (ans == NULL && node_mas[*pos]->lex == RIGHT_BR) {
            fprintf(stderr, "Missing argument on %d position.\n. Terminated program.\n", *pos);
            return NULL;
        }
        if (ans == NULL && node_mas[*pos]->lex != RIGHT_BR) {
            fprintf(stderr, "Missing argument and right bracket on %d position.\n. Terminated program.\n", *pos);
            return NULL;
        }
        if (node_mas[*pos]->lex != RIGHT_BR) {
            fprintf(stderr, "Missing right bracket on %d position.\n. Terminated program.\n", *pos);
            return NULL;
        }
        (*pos)++;
    } else {
        return NULL;
    }
    return ans;
}

struct node *
parse_expr1 (struct node ** node_mas, int * pos) {
    if (node_mas[*pos]->lex == EOI) {
        return NULL;
    }

    struct node *left, *right, *ans;
    left = parse_expr2(node_mas, pos);
    if (left == NULL) {
        return NULL;
    }
    while (node_mas[*pos]->lex == OP_MUL || node_mas[*pos]->lex == OP_DIV) {
        ans = node_mas[*pos];
        (*pos)++;
        right = parse_expr2(node_mas, pos);
        if (right == NULL) {
            return NULL;
        }
        ans->left_node = left;
        ans->right_node = right;
        left = ans;
    }
    return left;
}

struct node *
parse_expr0(struct node ** node_mas, int * pos) {
    if (node_mas[*pos]->lex == EOI) {
        return NULL;
    }

    struct node *left, *right, *ans;
    left = parse_expr1(node_mas, pos);
    if (left == NULL) {
        return NULL;
    }
    while (node_mas[*pos]->lex == OP_PLUS || node_mas[*pos]->lex == OP_SUB) {
        ans = node_mas[*pos];
        (*pos)++;
        right = parse_expr1(node_mas, pos);
        if (right == NULL) {
            return NULL;
        }
        ans->left_node = left;
        ans->right_node = right;
        left = ans;
    }
    return left;
}

struct node *
parse_expr(struct node ** node_mas) {
    if (node_mas == NULL ||node_mas[0]->lex == EOI) {
        return NULL;
    }

    int pos = 0;
    struct node *ans;
    ans = parse_expr0(node_mas, &pos);
    if (node_mas[pos]->lex != EOI) {
        if (node_mas[pos]->lex == NUMBER || node_mas[pos]->lex == LEFT_BR) {
            fprintf(stderr, "Missing operators on %d position\n", pos);
        }
        ans = NULL;
    }
    return ans;
}

void
delete_var_mas(struct variable **var_mas, int var_num) {
    for (int t = 0; t < var_num; t++) {
        free(*var_mas);
        var_mas++;
    }
}

int
compare(struct variable ** v1, struct variable ** v2) {
    return strcmp((*v1)->str, (*v2)->str);
}

int
read_var(struct variable ** var_mas, int num) {
    //если не надо считывать переменные, то это тоже успех
    if (var_mas == NULL) {
        return 1;
    }
    char * input_str, *str;
    for (int t = 0; t < num; t++){
        //слоджная и нудная проверка на соответствие 
        input_str = read_string();
        if (input_str == NULL) {
            return 0;
        }
        str = input_str;
        skip_spaces(&input_str);
        int is_sub = 0, len1 = 0, is_dots = 0, len2 = 0;
        if (*input_str == '-') {
            is_sub = 1;
            input_str++;
        }
        char buf[14], *temp;
        if (!(isdigit(*input_str) || (*input_str == '.' && isdigit(*(input_str+1)) ))){
            free(str);
            return 0;
        }
        while (isdigit(*input_str) && len1 < 7) {
            buf[len1] = *input_str;
            input_str++;
            len1++;
        }
        if (*input_str == '.') {
            input_str++;
            is_dots = 1;
            buf[len1] = '.';
        }
        while (isdigit(*input_str) && len2 < 7) {
            buf[len1 + is_dots + len2] = *input_str;
            input_str++;
            len2++;
        }
        if (len1 == 7 || !is_dots || len2 == 7) {
            fprintf(stderr, "Incorrect number to store in double variable\n");
            free(str);
            return 0;
        }
        buf[len1 + is_dots + len2] = '\0';
        var_mas[t]->value = strtod(buf, &temp);
        if (is_sub) {
            var_mas[t]->value *= -1;
        }
        var_mas[t]->wr = 1;
        skip_spaces(&input_str);
        if (*input_str != '\0') {
            free(str);
            fprintf(stderr, "Incorrect number to store in double variable\n");
            return 0;
        }
        free(str);
    }
    return 1;
}

double
calculate(struct node * main_node, int *res_exist, struct variable ** var_mas, int var_num) {
    if (main_node == NULL) {
        *res_exist = 0;
        return 1;
    }
    switch (main_node->lex) {
        case OP_PLUS:
            return calculate(main_node->left_node, res_exist, var_mas, var_num)\
             + calculate(main_node->right_node, res_exist, var_mas, var_num);
        case OP_SUB:
            return calculate(main_node->left_node, res_exist, var_mas, var_num)\
             - calculate(main_node->right_node, res_exist, var_mas, var_num);
        case OP_MUL:
            return calculate(main_node->left_node, res_exist, var_mas, var_num)\
             * calculate(main_node->right_node, res_exist, var_mas, var_num);
        case OP_DIV:
            double right = calculate(main_node->right_node, res_exist, var_mas, var_num);
            if (right < 0.00000001 && right > -0.00000001) {
                if (*res_exist != 0) {
                    fprintf(stderr, "Division by zero. %lf\n", right);
                }
                *res_exist = 0;
                return 1;
            }
            return calculate(main_node->left_node, res_exist, var_mas, var_num) / right;
        case NUMBER:
            return main_node->value;
        case VARIABLE:
            for (int t = 0; t < var_num; t++) {
                if (strcmp(var_mas[t]->str, main_node->str) == 0) {
                    return var_mas[t]->value;
                }
            }
            *res_exist = 0;
            break;
        default:
            *res_exist = 0;
            return 1;
    }
    *res_exist = 0;
    return 1;
}

int
main (void) {
    int ret_value;
    char *input_str = read_string();
    if (input_str == NULL) {
        fprintf(stderr, "Free input.\n");
        return 1;
    }
    struct node ** node_mas = lexem_analize(input_str);
    free(input_str);
    int var_num = 0;
    struct variable ** var_mas;
    if (node_mas != NULL) {
        var_mas = create_var_mas(node_mas, &var_num);
    } else {
        var_mas = NULL;
    }
    if (var_mas != NULL && var_num != 0) {
        qsort(var_mas, var_num, sizeof(struct variable *),  (int (*)(const void *, const void *)) compare);
    }
    int read_res = read_var(var_mas, var_num);
    struct node * main_node = parse_expr(node_mas);
    if (main_node == NULL) {
        ret_value = 1;
    } else {
        if (read_res) {
            int res_exist = 1;
            double res = calculate(main_node, &res_exist, var_mas, var_num);
            if (res_exist){
                printf("%f\n", res);
                ret_value = 0;
            } else {
                ret_value = 3;
            }
        } else {
            ret_value = 2;
        }
    }
    delete_var_mas(var_mas, var_num);
    free(var_mas);
    delete_node_mas(node_mas, -1);
    free(node_mas);
    return ret_value;
}