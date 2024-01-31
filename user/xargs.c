#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"



int main(int argc, char *argv[]) {
    char buf[512];
    char *bufEnd = buf;

    char *args[MAXARG];
    int nargs = argc-1;
    char c[1]; // buffer for reading char in stdin
    
    for(int i=1;i<argc;i++) {
        args[i-1] = (char *)malloc(strlen(argv[i])+1);
        strcpy(args[i-1], argv[i]);
    }

    while(read(0, c, 1) > 0) {
        if(*c == '\n') {
            *bufEnd = '\0';
            args[nargs] = (char *)malloc(strlen(buf)+1);
            strcpy(args[nargs], buf);
            nargs++;

            int pid = fork();
            if(pid == 0) { // child
                if(exec(argv[1], args) < 0) {
                    printf("xargs: exec %s failed\n", argv[1]);
                    // exit(1);
                }
            } else {
                wait(0);

                for(int i=argc-1;i<nargs;i++) {
                    free(args[i]);
                }
                nargs = argc-1;
                bufEnd = buf; // reset buffer
            }
        } else if(*c == ' ') {
            *bufEnd = '\0';
            args[nargs] = (char *)malloc(strlen(buf)+1);
            strcpy(args[nargs], buf);
            nargs++;
            bufEnd = buf; // reset buffer
        } else {
            *bufEnd = *c;
            bufEnd++;
        }
    }

    for(int i=1;i<argc;i++) {
        free(args[i-1]);
    }

    return 0;
}