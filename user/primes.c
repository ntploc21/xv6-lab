#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void exec_pipe(int fd) {
    int n;
    read(fd, &n, sizeof(int));
    printf("prime %d\n", n);

    int p[2];
    pipe(p);

    int cur = -1;
    while(read(fd, &cur, sizeof(int)) == sizeof(int)) {
        if(cur % n == 0) continue;
        write(p[1], &cur, sizeof(int));
    }

    close(fd);
    if(cur == -1) {
        close(p[1]);
        close(p[0]);
        return;
    }

    int pid = fork();
    if(pid == 0) {
        close(p[1]);
        exec_pipe(p[0]);
        close(p[0]);
    } else {
        close(p[1]);
        close(p[0]);
        wait(0);
    }
}

int main(int argc, char *argv[]) {

    int p[2];
    pipe(p);

    int n;
    for(int i=2;i<=35;i++) {
        n = i;
        write(p[1], &n, sizeof(int));
    }

    close(p[1]);
    exec_pipe(p[0]);
    close(p[0]);
    exit(0);
}