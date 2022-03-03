#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

enum {
    BUF_SIZE = 1024
};


void
itostr(int num) {
    char b;
    if (num < 10) {
        b = num + '0';
        write(1, &b, sizeof(char));
    } else {
        itostr(num / 10);
        b = num % 10 + '0';
        write(1, &b, sizeof(char));
    }
}

int
main(int argc, char ** argv) {
    if (argc == 1) {
        write(2, "Few parameters to wc\n", 21);
    }
    char buf[BUF_SIZE];
    int fd, bytes = 0, strings = 0, words = 0;
    int is_space_before = 1;
    ssize_t num = 0;
    for (int i = 1; i < argc; i++) {
        fd = open(argv[i], O_RDONLY);
        if (fd == -1) {
            write(2, "Problem with file ", 18);
            write(2, argv[i], strlen(argv[i]));
            write(2, "\n", 1);
            continue;
        }
        
        while ((num = read(fd, buf, BUF_SIZE)) != 0) {
            if (num < 0 && bytes == 0) {
                write(2, "Same problem with read byte\n", 28);
                exit(1);
            }
            for (int j = 0; j < num; j++) {
                if (isspace(buf[j])) {
                    if (!is_space_before) {
                        words++;
                    }
                    is_space_before = 1;
                } else {
                    is_space_before = 0;
                }
                if (buf[j] == '\n') {
                    strings++;
                }
            }
            bytes += num;
        }
        if (!is_space_before) {
            words++;
        }
        write(1, argv[i], strlen(argv[i]) * sizeof(*(argv[i])));
        write(1, " ", 1);
        itostr(strings);
        write(1, " ", 1);
        itostr(words);
        write(1, " ", 1);
        itostr(bytes);
        write(1, "\n", 1);
        bytes = 0;
        words = 0;
        strings = 0;
        close(fd);
    }
    return 0;
}