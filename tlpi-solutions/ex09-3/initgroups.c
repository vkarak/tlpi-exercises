#include "mylib.h"
#include "tlpi_hdr.h"

#include <grp.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


int xinitgroups(const char *user, gid_t group)
{
    gid_t *groups = malloc((NGROUPS_MAX+1)*sizeof(*groups));
    if (!groups) {
        errno = ENOMEM;
        return -1;
    }

    setgrent();
    size_t nr_groups = 0;
    struct group *grpent;
    while ( (grpent = getgrent()) != NULL) {
        char **pmemb = grpent->gr_mem;
        while (*pmemb) {
            if (!strcmp(*pmemb, user)) {
                groups[nr_groups++] = grpent->gr_gid;
            }
            ++pmemb;
        }
    }
    endgrent();

    // Add `group` to the list
    groups[nr_groups++] = group;

    // Try to elevate privileges and call setgroups(); we will then restore the
    // privileges to their original state
    int ret = 0;
    uid_t euid = geteuid();
    if (euid && seteuid(0) < 0) {
        ret = -1;
        goto cleanup;
    }

    if (setgroups(nr_groups, groups) < 0) {
        ret = -1;
        goto cleanup;
    }

    // Restore privileges
    seteuid(euid);

cleanup:
    free(groups);
    return ret;
}



void usage_error(const char *progname)
{
    usageErr("%s USER\n", progname);
}


int main(int argc, char *argv[])
{
    // De-escalate our privileges before doing anything else
    uid_t ruid, euid, suid;
    getresuid(&ruid, &euid, &suid);
    if (euid == 0) {
        if (ruid) {
            seteuid(ruid);
        } else if (suid) {
            seteuid(suid);
        }
    }

    if (argc < 2) {
        fprintf(stderr, "%s: too few arguments\n", argv[0]);
        usage_error(argv[0]);
    }

    const char *user = argv[1];

    errno = 0;
    struct passwd *info;
    if ( (info = getpwnam(user)) == NULL) {
        if (errno) {
            errExit("getpwnam() failed");
        }

        fatal("no such user: %s", user);
    }

    if (xinitgroups(user, info->pw_gid) < 0) {
        errExit("xinitgroups() failed");
    }
    return 0;
}
