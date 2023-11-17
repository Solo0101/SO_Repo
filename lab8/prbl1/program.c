#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>

void childProcess(char *grupa) {
    execl("./studenti", "studenti", grupa, NULL);
    printf("Error! execl() failed!\n");
}

void parentProcess(pid_t pid) {
    char in;
    while(1) {
        usleep(500000); 
        // if(waitpid(pid, NULL, 0) > 0) {
        //     printf("Child process terminated!\n");
        //     break; 
        // }
        printf("Group = ");
        if(scanf("%c", &in)) {
            printf("Input recived: %c\n", in);
        }
    }
}

int main() {
    pid_t pid;
    char *grupa = 0;
    pid = fork();
    if(pid < 0) {
        perror("Error!\n");
    } else if (pid == 0) {
        childProcess(grupa);
    } else {
        parentProcess(pid);
    }

    return 0;
}