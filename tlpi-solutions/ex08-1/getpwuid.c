#include "mylib.h"
#include "tlpi_hdr.h"

#include <fcntl.h>
#include <pwd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

void usage_error(const char *progname)
{
    usageErr("%s UID1 UID2\n", progname);
}


int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "%s: too few arguments\n", argv[0]);
        usage_error(argv[0]);
    }

    uid_t uid1 = atoi(argv[1]);
    uid_t uid2 = atoi(argv[2]);
    printf("%s %s\n", getpwuid(uid1)->pw_name, getpwuid(uid2)->pw_name);
    return 0;
}
