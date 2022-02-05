#include "mylib.h"
#include "tlpi_hdr.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


void usage_error(const char *progname)
{
    usageErr("%s FILE\n", progname);
}


int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "%s: too few arguments\n", argv[0]);
        usage_error(argv[0]);
    }

    int fd;
    if ( (fd = open(argv[1], O_WRONLY | O_APPEND)) < 0) {
        errExit("could not open file for writing: '%s'", argv[1]);
    }

    if (lseek(fd, 0, SEEK_SET) < 0) {
        errExit("could not seek to the beginning of the file");
    }

    const char *message = "hello\n";
    writep(fd, message, strlen(message));
    close(fd);
    return 0;
}
