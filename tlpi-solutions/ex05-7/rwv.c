#include "mylib.h"
#include "tlpi_hdr.h"

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>


#define NUM_CHUNKS  4
#define CHUNK_LEN   16

void usage_error(const char *progname)
{
    usageErr("%s\n", progname);
}

ssize_t xreadv(int fd, const struct iovec *iov, int iovcnt)
{
    ssize_t total_read = 0;
    for (int i = 0; i < iovcnt; ++i) {
        void *base = iov[i].iov_base;
        ssize_t count = iov[i].iov_len;
        ssize_t num_read = 0;
        while ( (num_read = read(fd, (char *) base + num_read, count)) > 0) {
            count -= num_read;
            total_read += num_read;
            if (count <= 0) {
                // We filled in the current buffer
                break;
            }
        }
        if (num_read < 0) {
            // Error while reading
            total_read = -1;
            break;
        } else if (num_read == 0) {
            // End of input
            break;
        }
    }
    return total_read;
}


ssize_t xwritev(int fd, const struct iovec *iov, int iovcnt)
{
    ssize_t total_written = 0;
    for (int i = 0; i < iovcnt; ++i) {
        ssize_t num_written;
        if ( (num_written = writep(fd, iov[i].iov_base, iov[i].iov_len)) < 0) {
            return -1;
        }
        total_written += num_written;
    }
    return total_written;
}


void test_rwv()
{
    // 1. Allocate a large contiguous write buffer
    // 2. Split it virtually in chunks
    // 3. Write through `xwritev()`

    char *wbuff = malloc(NUM_CHUNKS*CHUNK_LEN + 1);
    char *rbuff = malloc(NUM_CHUNKS*CHUNK_LEN + 1);
    if (!wbuff || !rbuff) {
        errExit("malloc() failed");
    }

    struct iovec wchunks[NUM_CHUNKS];
    struct iovec rchunks[NUM_CHUNKS];
    for (int i = 0; i < NUM_CHUNKS; ++i) {
        wchunks[i].iov_base = wbuff + i*CHUNK_LEN;
        wchunks[i].iov_len = CHUNK_LEN;

        rchunks[i].iov_base = rbuff + i*CHUNK_LEN;
        rchunks[i].iov_len = CHUNK_LEN;

        // Fill each chunk in the write buffer
        for (int j = 0; j < CHUNK_LEN; ++j) {
            *((char *) wchunks[i].iov_base + j) = 'a' + (char) i;
        }
    }

    wbuff[NUM_CHUNKS*CHUNK_LEN] = 0;

    char tempfile[] = "tmp_XXXXXX";
    int tmpfd;
    if ( (tmpfd = mkstemp(tempfile)) < 0) {
        errExit("mkstemp() failed");
    }

    if (unlink(tempfile) < 0) {
        errMsg("unlink() failed");
    }

    if (xwritev(tmpfd, wchunks, NUM_CHUNKS) < 0) {
        errExit("xwritev() failed");
    }

    if (lseek(tmpfd, 0, SEEK_SET) < 0) {
        errExit("lseek() failed");
    }

    if (xreadv(tmpfd, rchunks, NUM_CHUNKS) < 0) {
        errExit("xreadv() failed");
    }

    rbuff[NUM_CHUNKS*CHUNK_LEN] = 0;

    // Assert that rbuff equals to wbuff
    assert(!strncmp(rbuff, wbuff, NUM_CHUNKS*CHUNK_LEN));
    free(wbuff);
    free(rbuff);
    close(tmpfd);
}


int main(int argc, char *argv[])
{
    RUN_TEST(test_rwv);
    return 0;
}
