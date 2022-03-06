#include "mylib.h"
#include "tlpi_hdr.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>


#define IO_BUFF_SIZE    8192


void usage_error(const char *progname)
{
    usageErr("%s [-a] FILE\n", progname);
}

int main(int argc, char *argv[])
{
    int append_mode = O_TRUNC;
    int opt;
    while ( (opt = getopt(argc, argv, "a")) != -1) {
        switch(opt) {
        case 'a':
            append_mode = O_APPEND;
            break;
        case '?':
            usage_error(argv[0]);
        }
    }

    if (optind == argc) {
        fprintf(stderr, "%s: too few arguments\n", argv[0]);
        usage_error(argv[0]);
    }

    char *filename = argv[optind];
    int flags = O_WRONLY | O_CREAT | append_mode;
    int fd;
    if ( (fd = open(filename, flags, S_IRUSR | S_IWUSR)) < 0) {
        errExit("could not open file: '%s'", filename);
    }

    char buff[IO_BUFF_SIZE];
    ssize_t nr_bytes;
    while ( (nr_bytes = read(STDIN_FILENO, buff, sizeof(buff))) != 0) {
        if (writep(STDOUT_FILENO, buff, nr_bytes) < 0)
            errExit("%s:%d:writep() failed", __FILE__, __LINE__);

        if (writep(fd, buff, nr_bytes) < 0)
            errExit("%s:%d:writep() failed", __FILE__, __LINE__);
    }

    close(fd);
    return 0;
}
