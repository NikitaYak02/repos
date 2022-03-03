#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

enum {
    BUF_SIZE = 1024
};

void
itostr(int num) {
    //printf("%d\n", num);
    char b;
    if (num < 10) {
        b = num + '0';
        write(1, &b, sizeof(char));
    } else {
        itostr(num / 10);
        b = num % 10 + '0';
        write(1, &b, sizeof(char));
    }
    //printf("\n");
}

char buf[BUF_SIZE];
int buf_len = 0;
int str_num = 1, is_new_string_before = 1;


void
read_file(int fd) {
    ssize_t read_res;
    while ((read_res = read(fd, buf, sizeof(buf))) != 0) {
        if (read_res == -1) {
            write(2, "Incorrect file descr\n", strlen("Incorrect file descr\n"));
            exit(1);
        }
        if (write(1, buf, read_res) == -1) {
            write(2, "Same problem with write\n", strlen("Same problem with write\n"));
            exit(1);
        }
    }
}

void
read_file_numerate(int fd) {
    ssize_t read_res;
    size_t chr_in_str;
    char *start, *end;
    while ((read_res = read(fd, buf, sizeof(buf) - 1)) != 0) {
        
        if (read_res == -1) {
            write(2, "Incorrect file descr\n", strlen("Incorrect file descr\n"));
            exit(1);
        }
        
        buf[read_res] = '\0';
        start = buf;
        
        while ((end = strchr(start, '\n')) != NULL) {
            //end - pointer to first byte after string ending
            end++;
            chr_in_str = end - start;
            if (is_new_string_before) {
                itostr(str_num);
                write(1, " ", 1);
                str_num++;
            } else {
                is_new_string_before = 1;
            }

            if (write(1, start, chr_in_str) == -1) {
                exit(1);
            }
            
            start = end;
            if (start >= buf + read_res) {
                break;
            }
        }

        if (start < buf + read_res) {
            chr_in_str = start - buf;
            chr_in_str = read_res - chr_in_str;
            if (is_new_string_before) {
                itostr(str_num);
                write(1, " ", 1);
                str_num++;
            } else {
                is_new_string_before = 1;
            } 
            is_new_string_before = 0;
            write(1, start, chr_in_str);
        }
    }
}

int
main(int argc, char ** argv) {
    if (argc < 2) {
        read_file(0);
        return 0;
    }
    int fd;
    if (strcmp(argv[1], "-n") == 0) {
        if (argc == 2) {
            read_file_numerate(0);
            return 0;
        }
        for (int i = 2; i < argc; i++) {
            fd = open(argv[i], O_RDONLY);
            if (fd == -1) {
                continue;
            } else {
                read_file_numerate(fd);
            }
            close(fd);
        }
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        fd = open(argv[i], O_RDONLY);
        if (fd == -1) {
            continue;
        } else {
            read_file(fd);
        }
        close(fd);
    }
    return 0;
}