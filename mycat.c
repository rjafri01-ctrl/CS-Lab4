#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

void print_line(char *line, size_t length, int newline,
                int e, int n, int s, int *number) {
    int empty = 1;

    for (size_t i = 0; i < length; i++) {
        if (!isspace((unsigned char)line[i])) {
            empty = 0;
            break;
        }
    }

    if (s && empty)
        return;

    if (n)
        printf("%6d\t", (*number)++);

    fwrite(line, 1, length, stdout);
    if (e)
        putchar('$');
    if (newline)
        putchar('\n');
}

int main(int argc, char *argv[]) {
    int e = 0, n = 0, s = 0, files = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] != '\0') {
            for (int j = 1; argv[i][j] != '\0'; j++) {
                if (argv[i][j] == 'e') e = 1;
                else if (argv[i][j] == 'n') n = 1;
                else if (argv[i][j] == 's') s = 1;
                else {
                    fprintf(stderr, "Unknown option: -%c\n", argv[i][j]);
                    return 1;
                }
            }
        } else {
            files++;
        }
    }

    if (files == 0) {
        fprintf(stderr, "Usage: mycat [-ens] file1 [file2 ...]\n");
        return 1;
    }

    size_t capacity = 256, length = 0;
    char *line = malloc(capacity);
    if (line == NULL)
        return 1;

    int number = 1, error = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] != '\0')
            continue;

        int fd = open(argv[i], O_RDONLY);
        if (fd == -1) {
            perror(argv[i]);
            error = 1;
            continue;
        }

        char ch;
        ssize_t count;
        while ((count = read(fd, &ch, 1)) == 1) {
            if (ch == '\n') {
                print_line(line, length, 1, e, n, s, &number);
                length = 0;
            } else {
                if (length == capacity) {
                    capacity *= 2;
                    char *temp = realloc(line, capacity);
                    if (temp == NULL) {
                        free(line);
                        close(fd);
                        return 1;
                    }
                    line = temp;
                }
                line[length++] = ch;
            }
        }
        if (count == -1) {
            perror(argv[i]);
            error = 1;
        }
        close(fd);
    }

    if (length > 0)
        print_line(line, length, 0, e, n, s, &number);

    free(line);
    return error;
}