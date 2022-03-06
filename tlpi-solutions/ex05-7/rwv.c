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
    return -1;
}


ssize_t xwritev(int fd, const struct iovec *iov, int iovcnt)
{
    return -1;
}


void test_rwv()
{
    // 1. Allocate a large contiguous write buffer
    // 2. Split it virtually in chunks
    // 3. Write through `xritev()`

    char *wbuff = malloc(NUM_CHUNKS*CHUNK_LEN);
    char *rbuff = malloc(NUM_CHUNKS*CHUNK_LEN);
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

    char tempfile[] = "tmp_XXXXXX";
    int tmpfd;
    if ( (tmpfd = mkstemp(tempfile)) < 0) {
        errExit("mkstemp() failed");
    }

    if (unlink(tempfile) < 0) {
        errMsg("unlink() failed");
    }

    xwritev(tmpfd, wchunks, NUM_CHUNKS);
    xreadv(tmpfd, rchunks, NUM_CHUNKS);

    // Assert that rbuff equals to wbuff
    assert(!strncmp(rbuff, wbuff, NUM_CHUNKS*CHUNK_LEN));

    free(wbuff);
    close(tmpfd);
}


int main(int argc, char *argv[])
{
    RUN_TEST(test_rwv);
    return 0;
}
