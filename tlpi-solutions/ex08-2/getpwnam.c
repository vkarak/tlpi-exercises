#include "mylib.h"
#include "tlpi_hdr.h"

#include <fcntl.h>
#include <pwd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


struct passwd *xgetpwnam(const char *name)
{
    int errno_save = errno;
    struct passwd *entry;

    setpwent();
    while ( (entry = getpwent()) != NULL) {
        // In case of a successful call, we want to make sure the errno value is
        // unchanged, so we reset it here and after the `strcmp()` call. Any
        // system or library call may change errno arbitrarily if it is
        // successful and in fact `getpwent()` changes it to EINVAL.
        errno = errno_save;
        if (!strcmp(entry->pw_name, name)) {
            errno = errno_save;
            goto exit;
        }
    }

exit:
    endpwent();
    return entry;
}

void usage_error(const char *progname)
{
    usageErr("%s USERNAME\n", progname);
}


int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "%s: too few arguments\n", argv[0]);
        usage_error(argv[0]);
    }

    errno = 0;
    struct passwd *entry = xgetpwnam(argv[1]);
    if (!entry) {
        if (errno) {
            errExit("xgetpwnam() failed");
        }
        fatal("no passwd entry found for username %s", argv[1]);
    }
    printf("UID of user `%s' is %d\n", argv[1], entry->pw_uid);
    return 0;
}
