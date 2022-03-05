#include "mylib.h"
#include "tlpi_hdr.h"

#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    char tempfile[] = "tmp_XXXXXX";
    int tmpfd1, tmpfd2;
    if ( (tmpfd1 = mkstemp(tempfile)) < 0) {
        errExit("mkstemp() failed");
    }

    if (unlink(tempfile) < 0) {
        errMsg("unlink() failed");
    }

    if ( (tmpfd2 = dup(tmpfd1)) < 0) {
        errExit("dup() failed");
    }

    printf("Validating dup() function... ");

    // Assert the flags are the same
    assert(fcntl(tmpfd1, F_GETFL) == fcntl(tmpfd2, F_GETFL));

    // Update the file offset from both descriptors and assert it has the
    // expected value
    if (lseek(tmpfd1, 0, SEEK_SET) < 0) {
        errExit("lseek() on tmpfd1 failed");
    }

    if (lseek(tmpfd2, 10, SEEK_CUR) < 0) {
        errExit("lseek() on tmpfd2 failed");
    }

    // Assert that the offset is the expected one
    off_t off;
    if ( (off = lseek(tmpfd1, 0, SEEK_CUR)) < 0) {
        errExit("could not retrieve offset on tmpfd1");
    }

    assert(off == 10);
    printf("[OK]\n");

    close(tmpfd1);
    close(tmpfd2);
    return 0;
}
