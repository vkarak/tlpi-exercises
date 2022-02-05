#include "mylib.h"
#include "tlpi_hdr.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>


#define IO_BUFF_SIZE    8192


void usage_error(const char *progname)
{
    usageErr("%s SRC DST\n", progname);
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "%s: too few arguments\n", argv[0]);
        usage_error(argv[0]);
    }

    int src_fd, dst_fd;
    if ( (src_fd = open(argv[1], O_RDONLY)) < 0) {
        errExit("could not open file for reading: '%s'", argv[1]);
    }

    if ( (dst_fd = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
        errExit("could not open file for writing: '%s'", argv[2]);
    }

    char buff[IO_BUFF_SIZE];
    ssize_t nr_bytes;
    while ( (nr_bytes = read(src_fd, buff, sizeof(buff))) != 0) {
        writep(dst_fd, buff, nr_bytes);
    }

    close(src_fd);
    close(dst_fd);
    return 0;
}
