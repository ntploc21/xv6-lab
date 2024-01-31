#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char* fmtname(char *path) {
    static char buf[DIRSIZ+1];
    char *p;

    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--);
    p++;

    // Return blank-padded name.
    if(strlen(p) >= DIRSIZ) return p;
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), '\0', DIRSIZ-strlen(p));
    return buf;
}

void find_recurse(char* pattern, char* path, char* buf, int fd, struct stat st) {
    char *p;
    struct dirent de;
    struct stat st_next;

    if(strlen(path) + 1 + DIRSIZ + 1 > 512) {
        printf("find: path too long\n");
        return;
    }

    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)) {
        if(de.inum == 0) continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0) {
            printf("find: cannot stat %s\n", buf);
            continue;
        }

        if(st.type == T_FILE) {
            if(strcmp(fmtname(buf), pattern) == 0) {
                printf("%s\n", buf);
            }
            continue;
        } else if(st.type == T_DIR) {
            if(strcmp(fmtname(buf), ".") != 0 && strcmp(fmtname(buf), "..") != 0) {
                int fd_next = open(buf, O_RDONLY);
                if(fstat(fd, &st_next) < 0) {
                    printf("find: cannot stat %s\n", path);
                    close(fd);
                    return;
                }

                find_recurse(pattern, buf, buf, fd_next, st_next);
                close(fd_next);
            }
        }
    }
}

void find(char *pattern, char *path) {
    char buf[512];
    int fd;
    struct stat st;

    if((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if(st.type == T_DIR) {
        find_recurse(pattern, path, buf, fd, st);
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: find <pattern> <path>\n");
    } else if(argc == 2) {
        find(argv[1], ".");
    } else {
        for(int i=2;i<=argc;i++) {
            find(argv[i], argv[1]);
        }
    }
    exit(0);
}