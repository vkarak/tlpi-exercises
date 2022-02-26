#include "mylib.h"
#include "tlpi_hdr.h"

#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define RUN_TEST(fn) {printf("%s... ", #fn); fn(); printf("[OK]\n");}

int xdup(int oldfd)
{
    if (oldfd < 0) {
        errno = EBADF;
        return -1;
    }

    int newfd = fcntl(oldfd, F_DUPFD, oldfd);
    if (newfd < 0 && errno == EMFILE) {
        // dup() returns EBADF in case the process limit for open file
        // descriptors is reached
        errno = EBADF;
    }

    return newfd;
}


int xdup2(int oldfd, int newfd)
{
    if (oldfd < 0 || newfd < 0) {
        errno = EBADF;
        return -1;
    }

    // Check if oldfd is valid
    if (fcntl(oldfd, F_GETFL) < 0) {
        // If oldfd is invalid, fcntl() would again set EBADF, but we set it
        // here explicitly to always match the dup2() spec
        errno = EBADF;
        return -1;
    }

    if (oldfd == newfd)
        return newfd;

    // Close newfd regardless; if newfd is not open, close will fail but we
    // ignore the error
    close(newfd);
    newfd = fcntl(oldfd, F_DUPFD, newfd);
    if (newfd < 0 && errno == EMFILE) {
        // dup2() returns EBADF in case the process limit for open file
        // descriptors is reached
        errno = EBADF;
    }

    // The returned newfd will be greater or equal to the passed newfd. The
    // calling program must explicitly check this and take action.
    return newfd;
}


void test_xdup()
{
    // 1. Create a temporary file
    // 2. Duplicate the descriptor
    // 3. Update the offset of the file through both descriptors
    // 4. Verify that the offset is modified accordingly

    char tempfile[] = "tmp_XXXXXX";
    int tmpfd1, tmpfd2, tmpfd3;
    if ( (tmpfd1 = mkstemp(tempfile)) < 0) {
        errExit("mkstemp() failed");
    }

    if (unlink(tempfile) < 0) {
        errMsg("unlink() failed");
    }

    if ( (tmpfd2 = dup(tmpfd1)) < 0) {
        errExit("dup() failed");
    }

    if ( (tmpfd3 = xdup(tmpfd1)) < 0) {
        errExit("xdup() failed");
    }

    if (lseek(tmpfd1, 0, SEEK_SET) < 0) {
        errExit("lseek() on tmpfd1 failed");
    }

    if (lseek(tmpfd2, 10, SEEK_CUR) < 0) {
        errExit("lseek() on tmpfd2 failed");
    }

    if (lseek(tmpfd3, 10, SEEK_CUR) < 0) {
        errExit("lseek() on tmpfd3 failed");
    }

    // Assert that the offset is the expected one
    off_t off;
    if ( (off = lseek(tmpfd1, 0, SEEK_CUR)) < 0) {
        errExit("could not retrieve offset on tmpfd1");
    }

    assert(off == 20);
    close(tmpfd1);
    close(tmpfd2);
    close(tmpfd3);
}

void test_xdup2()
{
    char tempfile[] = "tmp_XXXXXX";
    int tmpfd, newfd = 100;
    if ( (tmpfd = mkstemp(tempfile)) < 0) {
        errExit("mkstemp() failed");
    }

    if (unlink(tempfile) < 0) {
        errMsg("unlink() failed");
    }

    if ( (newfd = xdup2(tmpfd, newfd)) < 0) {
        errExit("xdup2() failed");
    }

    assert(newfd == 100);

    if (lseek(tmpfd, 10, SEEK_SET) < 0) {
        errExit("lseek() on tmpfd failed");
    }

    if (lseek(newfd, 10, SEEK_CUR) < 0) {
        errExit("lseek() on tmpfd failed");
    }

    off_t off;
    if ( (off = lseek(tmpfd, 0, SEEK_CUR)) < 0) {
        errExit("could not retrieve offset on tmpfd");
    }

    assert(off == 20);
    close(tmpfd);
    close(newfd);
}

int main(int argc, char *argv[])
{
    RUN_TEST(test_xdup);
    RUN_TEST(test_xdup2);
    return 0;
}
