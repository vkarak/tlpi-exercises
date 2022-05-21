#include "mylib.h"
#include "tlpi_hdr.h"

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


void print_usage(const char *progname)
{
    fprintf(stderr, "Usage: %s [USER]\n", progname);
}


int main(int argc, char *argv[])
{
    int opt;
    while ( (opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
        case 'h':
            print_usage(argv[0]);
            exit(0);
        case '?':
            print_usage(argv[0]);
            exit(1);
        }
    }

    uid_t ruid;
    if (optind == argc) {
        // No additional argument was given; assume the current user
        ruid = getuid();
    } else {
        // Retrieve the user id of the specified user
        char *username = argv[optind];
        struct passwd *user_info = getpwnam(username);
        if (!user_info) {
            fatal("unknown user: %s", username);
        }

        ruid = user_info->pw_uid;
    }

    DIR *dp = opendir("/proc");
    if (!dp) {
        errExit("could not open `/proc' directory");
    }

    struct dirent *dir;
    while ( (dir = readdir(dp)) != NULL) {
        if (!isdigit(dir->d_name[0])) {
            // Not a PID
            continue;
        }

        char status_file[PATH_MAX];
        snprintf(status_file, PATH_MAX, "/proc/%d/status", atoi(dir->d_name));
        FILE *fp = fopen(status_file, "r");
        if (!fp) {
            // The process has probably exited
            continue;
        }

        char proc_name[LINE_MAX];
        char *line = NULL;
        size_t line_size = 0;
        while (getline(&line, &line_size, fp) != -1) {
            if (!strncmp(line, "Name:", 5)) {
                sscanf(line, "Name: %s", proc_name);
            } else if (!strncmp(line, "Uid:", 4)) {
                uid_t proc_ruid;
                sscanf(line, "Uid: %u", &proc_ruid);
                if (ruid == proc_ruid) {
                    printf("%s\n", proc_name);
                }
            }
        }
        fclose(fp);
        free(line);
    }
    closedir(dp);
    return 0;
}
