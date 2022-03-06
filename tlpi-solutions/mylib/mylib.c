#include "mylib.h"
#include "tlpi_hdr.h"


ssize_t writep(int fd, const void *buf, size_t count)
{
    size_t total_written = 0;
    while (total_written < count) {
        ssize_t nr_bytes;
        if ( (nr_bytes = write(fd, buf, count - total_written)) < 0) {
            return -1;
        }
        total_written += nr_bytes;
    }
}
