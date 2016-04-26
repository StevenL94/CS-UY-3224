// init: The initial user-level program

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "bcrypt.h"
#include "rand.h"

char *argv[] = { "sh", 0 };

int
main(void)
{
  int pid, wpid;
    
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
        if (open("pw", O_RDONLY)) {
            printf(1, "No password set. Please choose one.\n");
            open("pw", O_CREATE);
            printf(1, "Enter password: ");
            uchar salt[128];
            *salt = genrand();
            uchar* hash;
            hash = bcrypt(argv[0], salt);
            if (hash < 0) {
                printf(1, "Entered password was too long. Please try again.\n");
            }
            else {
                printf(1, "Re-enter to confirm: ");
                if (bcrypt_checkpass(argv[0], salt, hash) < 0) {
                    printf(1, "Passwords do not match. Try again.\nEnter password: ");
                }
                else {
                    printf(1, "Password successfully set. You may now use it to log in.\n");
                }
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
