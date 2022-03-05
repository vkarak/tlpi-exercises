#include "mylib.h"
#include "tlpi_hdr.h"

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    const char *file = "foo.txt";
    int fd1 = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    int fd2 = dup(fd1);
    int fd3 = open(file, O_RDWR);
    write(fd1, "Hello,", 6);    // Contents: Hello,
    write(fd2, " world", 6);    // Contents: Hello, world
    lseek(fd2, 0, SEEK_SET);
    write(fd1, "HELLO,", 6);    // Contents: HELLO, world
    write(fd3, "Gidday", 6);    // Contents: Gidday world

    close(fd1);
    close(fd2);
    close(fd3);

#ifndef KEEP_FILE
    if (unlink(file) < 0)
        errMsg("unlink() failed");
#endif

    return 0;
}
