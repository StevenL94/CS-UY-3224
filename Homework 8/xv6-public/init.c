// init: The initial user-level program

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "bcrypt.h"
#include "rand.c"

char *argv[] = { "sh", 0 };

int
main(void)
{
  int pid, wpid;
  static char buf[100];
  int buffread = 0;
  char readbuf[512];
  int n;
  uchar salt[128];
  uchar* hash;
  int h_full;
    
  if(open("console", O_RDWR) < 0){
    mknod("console", 1, 1);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  for(;;){
    printf(1, "init: starting sh\n");
    pid = fork();
    if(pid < 0){
      printf(1, "init: fork failed\n");
      exit();
    }
    if(pid == 0){
        int fd;
        if ((fd = open("pw", O_RDONLY)) < 0) {
            printf(1, "No password set. Please choose one.\n");
            fd = open("pw", O_CREATE);
            printf(1, "Enter password: ");
            sgenrand(1024);
            *salt = genrand();
            while (buffread >= 0) {
                memset(buf, 0, sizeof(buf));
                gets(buf, sizeof(buf));
                if(buf[0] == 0) // EOF
                    buffread = -1;
                buf[strlen(buf)-1] = 0;
                hash = bcrypt(buf, salt);
                break;
            }
            while (hash < 0) {
                printf(1, "Entered password was too long. Please try again.\n");
                while (buffread >= 0) {
                    memset(buf, 0, sizeof(buf));
                    gets(buf, sizeof(buf));
                    if(buf[0] == 0) // EOF
                        buffread = -1;
                    buf[strlen(buf)-1] = 0;
                    hash = bcrypt(buf, salt);
                    break;
                }
            }
            printf(1, "Re-enter to confirm: ");
            while (buffread >= 0) {
                memset(buf, 0, sizeof(buf));
                gets(buf, sizeof(buf));
                if(buf[0] == 0) // EOF
                    buffread = -1;
                buf[strlen(buf)-1] = 0;
                break;
            }
            while (bcrypt_checkpass(buf, salt, hash) < 0) {
                printf(1, "Passwords do not match. Try again.\nEnter password: ");
                while (buffread >= 0) {
                    memset(buf, 0, sizeof(buf));
                    gets(buf, sizeof(buf));
                    if(buf[0] == 0) // EOF
                        buffread = -1;
                    buf[strlen(buf)-1] = 0;
                    break;
                }
                write(fd, hash, O_WRONLY);
                printf(1, "Password successfully set. You may now use it to log in.\n");
            }
            write(fd, hash, O_WRONLY);
            printf(1, "Password successfully set. You may now use it to log in.\n");
        }
        while((n = read(fd, readbuf, sizeof(readbuf))) > 0) {
            write(1, readbuf, n);
        }
        int i;
        h_full = 0;
        for(i = 0; i < sizeof(readbuf); ++i){
            if (h_full == 0) {
                if (readbuf[i] != '\n') {
                    hash[i] = readbuf[i];
                }
                else {
                    h_full = 1;
                }
            }
            ++i;
            if (readbuf[i] != '\n') {
                salt[i] = readbuf[i];
            }
        }
        printf(1, "Enter password: ");
        while (buffread >= 0) {
            memset(buf, 0, sizeof(buf));
            gets(buf, sizeof(buf));
            if(buf[0] == 0) // EOF
                buffread = -1;
            buf[strlen(buf)-1] = 0;
            break;
        }
        while (bcrypt_checkpass(buf, salt, hash) < 0) {
            printf(1, "Passwords do not match. Try again.\nEnter password: ");
            while (buffread >= 0) {
                memset(buf, 0, sizeof(buf));
                gets(buf, sizeof(buf));
                if(buf[0] == 0) // EOF
                    buffread = -1;
                buf[strlen(buf)-1] = 0;
                break;
            }
        }
        
      exec("sh", argv);
      printf(1, "init: exec sh failed\n");
      exit();
    }
    while((wpid=wait()) >= 0 && wpid != pid)
      printf(1, "zombie!\n");
  }
}
