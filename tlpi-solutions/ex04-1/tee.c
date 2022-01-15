#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "lib/tlpi_hdr.h"


#define IO_BUFF_SIZE    8192


void usage_error(const char *progname)
{
    usageErr("%s [-a] FILE\n", progname);
}

void writep(int fd, const void *buf, size_t count)
{
    size_t total_written = 0;
    while (total_written < count) {
        ssize_t nr_bytes;
        if ( (nr_bytes = write(fd, buf, count - total_written)) < 0) {
            errExit("failed to write %ld byte(s)", count);
        }
        total_written += nr_bytes;
    }
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
        writep(STDOUT_FILENO, buff, nr_bytes);
        writep(fd, buff, nr_bytes);
    }

    close(fd);
    return 0;
}
