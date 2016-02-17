#include "types.h"
#include "stat.h"
#include "user.h"

char buf[512];
char string[512];

void head(int fd, char *lines) {
//    initialize variables
    int i, j, n, l;
    l = j = 0;
//    verify that file has not reached NUL
    while((n = read(fd, buf, sizeof(buf))) > 0) {
        for (i = 0; i < n; i++) {
//            find instance of EOL and increment counter
            if (buf[i] == '\n') {
                l++;
//                verify EOL counter has not exceeded parameters
                if (l <= atoi(&lines[1])) {
//                    append EOL to line
                    string[j] = '\n';
                }
//                print line
                printf(1, "%s", string);
//                clear array for next line
                for (j = 0; j < i; j++) {
                    string[j] = '\0';
                }
                j = 0;
            }
            else {
//                verify EOL counter has not exceeded parameters
                if (l < atoi(&lines[1])) {
//                    store line
                    string[j] = buf[i];
                }
                j++;
            }
        }
    }
}

int main(int argc, char *argv[]) {
//    initialize variables
    int fd, i;
//    read from standard input
    if (argc <= 1) {
        head(0, "-10");
        exit();
    }
    for (i = 1; i < argc; i++) {
//        attempt to open file
        
        if ((fd = open(argv[i], 0)) < 0) {
//            if first attempt failed check the command argument
            if ((fd = open(argv[i+1], 0)) < 0) {
//                output error
                if (*argv[1] == '-') {
                    printf(1, "head: cannot open %s\n", argv[i+1]);
                    exit();
                }
                printf(1, "head: cannot open %s\n", argv[i]);
                exit();
            }
//            user defined argument
            head(fd, argv[i]);
            close(fd);
            exit();
        }
//        default value for number of lines is 10
        head(fd, "-10");
        close(fd);
    }
    exit();
}