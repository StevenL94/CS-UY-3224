#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main() {
    int filedes;
    filedes = open("myfile.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    int rv;
    rv = fork();
    if (rv == 0) {
        char msg[] = "Process 1\n";
        printf("Hello, I'm in the child, my process ID is %d\n", getpid());
        write(filedes, msg, sizeof(msg));
    }
    else {
        char msg[] = "Process 2\n";
        printf("This is the parent process, my process ID is %d and my child is %d\n", getpid(), rv);
        write(filedes, msg, sizeof(msg));
    }
    close(filedes);
}