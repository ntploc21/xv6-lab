#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    int p[2];
    char buf[10];
    pipe(p);

    int pid = fork();
    if(pid == 0) { // child
        write(p[1], "ping", 4);
        printf("%d: received ping\n", getpid());
        exit(0);
    } else { // parent
        wait(0);
        read(p[0], buf, 4);
        printf("%d: received pong\n", getpid());
        exit(0);
    }
    return 0;
}