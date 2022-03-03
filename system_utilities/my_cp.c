#include <unistd.h>
//#include <sys/wait.h>
//#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
//#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

enum {
    STR_LEN = 1024
};

/*
num is the number of the file whose descriptor we want to return
1 - checking the file for existence (source file)
2 - checking the file for existence and for the possibility of writing,
or for the absence of a file (an object to insert)
*/
int
open_with_checks(char * path, struct stat *file_stat, int num) {
    int fd = -1, st_res = 0;
    char num_text = num + '0';

    if (stat(path, file_stat) == -1) {
        if (num == 1) {
            char buf1[] = "cp: parametrs is not correct path of first file\n";
            write(2, buf1, sizeof(buf1));
            exit (1);
        } else {
            st_res = -1;
        }
    }

    if (st_res == 0 && !S_ISREG(file_stat->st_mode) && !S_ISLNK(file_stat->st_mode)) {
        /*processing a file that is not a symbolic link or a regular file*/
        char buf1[] = "cp: ";
        write(2, buf1, strlen(buf1));
        write(2, &num_text, sizeof(num_text));
        char buf2[] = " parametrs is not file or directory\n";
        write(2, buf2, strlen(buf2));
        exit(1);
    }

    if (num == 1) {
        fd = open(path, O_RDONLY);
        if (fd == -1) {
            exit(1);
        }
    }

    if (num == 2) {
        if (st_res == 0) {
            fd = open(path, O_WRONLY | O_TRUNC);
        } else {
            fd = open(path, O_WRONLY | O_CREAT, 0666);
            stat(path, file_stat);
        }
        if (fd == -1) {
            char buf1[] = "cp: ";
            write(2, buf1, strlen(buf1));
            write(2, &num_text, sizeof(num_text));
            char buf2[] = " parametrs is not file or directory\n";
            write(2, buf2, strlen(buf2));
            exit(1);
            exit(1);
        }
    }
    return fd;
}

int
main(int argc, char ** argv) {
    if (argc < 3) {
        char buf[] = "cp: missing destination file operand after ";
        char c = '\n';
        write(2, buf, strlen(buf));
        write(2, argv[1], strlen(argv[1]));
        write(2, &c, sizeof(c));
        return 1;
    }
    if (argc > 3) {
        char buf[] = "cp: too many argument for cp\n";
        write(2, buf, sizeof(buf));
        return 1;
    }
    int fd1, fd2;
    struct stat file_stat1, file_stat2;

    fd1 = open_with_checks(argv[1], &file_stat1, 1);
    fd2 = open_with_checks(argv[2], &file_stat2, 2);

    if (file_stat1.st_ino == file_stat2.st_ino) {
        close(fd1);
        close(fd2);
        exit(1);
    }

    ssize_t read_res, write_res;
    char str[STR_LEN];
    while (1) {
        read_res = read(fd1, str, STR_LEN);
        if (read_res == -1) {
            close(fd1);
            close(fd2);
            return 1;
        }

        if (read_res == 0) {
            chmod(argv[2], (file_stat1.st_mode & 0777));
            close(fd1);
            close(fd2);
            return 0;
        }

        write_res = write(fd2, str, read_res);
        if (write_res == -1) {
            close(fd1);
            close(fd2);
            return 1;
        }
    }
}