#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "mytypes.h"
#include "lexem_analyze.h"
#include "executing.h"

void
exit_with_code(int status) {
    if (WIFEXITED(status)) {
        exit(WEXITSTATUS(status));
    }
    if (WIFSIGNALED(status)) {
        exit(128 + WTERMSIG(status));
    }
}

void
execute(struct node * vertex) {
    int status;
    if (vertex->op_type == ARGV) {
        if (fork() == 0) {
            execvp(vertex->argv[0], vertex->argv);
            exit(127);
        }
        wait(&status);
        exit_with_code(status);
    }

    if (vertex->op_type == MUTE) {
        pid_t pid1, pid2 = -1;
        if ((pid1 = fork()) == 0) {
            execute(vertex->left);
        }
        if (vertex->right != NULL) {
            if ((pid2 = fork()) == 0) {
                execute(vertex->right);
            }
        }
        waitpid(pid1, &status, 0);
        if (pid2 != -1) {
            waitpid(pid2, &status, 0);
        }
        exit_with_code(status);
    }

    if (vertex->op_type == SEMICOLON) {
        pid_t pid1, pid2 = -1;
        if ((pid1 = fork()) == 0) {
            execute(vertex->left);
        }
        waitpid(pid1, &status, 0);
        if (vertex->right != NULL) {
            if ((pid2 = fork()) == 0) {
                execute(vertex->right);
            }
        }
        if (pid2 != -1) {
            waitpid(pid2, &status, 0);
        }
        exit_with_code(status);
    }

    if (vertex->op_type == LOG_OR) {
        pid_t pid1, pid2;
        if ((pid1 = fork()) == 0) {
            execute(vertex->left);
        }
        waitpid(pid1, &status, 0);

        if (!WIFEXITED(status) || WEXITSTATUS(status)) {
            if ((pid2 = fork()) == 0) {
                execute(vertex->right);
            }
            waitpid(pid2, &status, 0);
        }
        exit_with_code(status);
    }
    if (vertex->op_type == LOG_AND) {
        pid_t pid1, pid2 = -1;
        if ((pid1 = fork()) == 0) {
            execute(vertex->left);
        }
        waitpid(pid1, &status, 0);

        if (WIFEXITED(status) && !WEXITSTATUS(status)) {
            if ((pid2 = fork()) == 0) {
                execute(vertex->right);
            }
            waitpid(pid2, &status, 0);
        }
        exit_with_code(status);
    }
    if (vertex->op_type == WR_APP || vertex->op_type == WR || vertex->op_type == RD) {
        struct node * vert_copy = vertex;
        do {
            int fd;
            if (vertex->op_type == WR_APP) {
                fd = open(vertex->path, O_WRONLY | O_APPEND | O_CREAT, 0664);
                if (fd == -1) {
                    exit(1);
                }
                dup2(fd, 1);
                close(fd);
            } else if (vertex->op_type == WR) {
                fd = open(vertex->path, O_WRONLY | O_TRUNC | O_CREAT, 0664);
                if (fd == -1) {
                    exit(1);
                }
                dup2(fd, 1);
                close(fd);
            } else {
                fd = open(vertex->path, O_RDONLY);
                if (fd == -1) {
                    exit(1);
                }
                dup2(fd, 0);
                close(fd);
            }
            vertex = vertex->right;
        } while (vertex != NULL);
        vertex = vert_copy;
        pid_t pid;
        if ((pid = fork()) == 0) {
            execute(vertex->left);
        }
        waitpid(pid, &status, 0);
        exit_with_code(status);
    }
    if (vertex->op_type == CONV) {
        int pipes[2];
        pid_t pid1, pid2;
        if (pipe(pipes) == -1){
            exit(1);
        }
        if ((pid1 = fork()) == 0) {
            dup2(pipes[1], 1);
            close(pipes[0]);
            close(pipes[1]);
            execute(vertex->left); 
        }
        if ((pid2 = fork()) == 0) {
            dup2(pipes[0], 0);
            close(pipes[0]);
            close(pipes[1]);
            execute(vertex->right);
        }
        close(pipes[0]);
        close(pipes[1]);
        waitpid(pid1, NULL, 0);
        waitpid(pid2, &status, 0);
        exit_with_code(status);
    }
}

int
executing_tree (struct node * vertex) {
    int status;

    if (fork() == 0) {
        execute(vertex);
    }
    wait(&status);
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) {
        return 128 + WTERMSIG(status);
    }
    return 0;
}

struct node **
keeper(struct node ** arr) {
    static struct node ** keeps_value;
    if (arr != NULL) {
        keeps_value = arr; 
    }
    return keeps_value;
}

void
safety_exit(void) {
    struct node ** arr = keeper(NULL);
    if (arr != NULL) {
        clean_node_arr(arr);
    }
}