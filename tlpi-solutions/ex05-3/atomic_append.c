#include "mylib.h"
#include "tlpi_hdr.h"

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


void usage_error(const char *progname)
{
    usageErr("%s FILE COUNT [x]\n", progname);
}


int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "%s: too few arguments\n", argv[0]);
        usage_error(argv[0]);
    }

    const char *filename = argv[1];
    size_t count = (size_t) getLong(argv[2], GN_GT_0, NULL);
    int append_mode = O_APPEND;
    if (argc > 3 && strncmp(argv[3], "x", 1) == 0) {
        append_mode = 0;
    }

    int fd, file_exists = 0;
    if ( (fd = open(filename, O_CREAT | O_EXCL | O_WRONLY | append_mode,
                    S_IRUSR | S_IWUSR)) < 0) {
        if (errno == EEXIST) {
            file_exists = 1;
        } else {
            errExit("could not open file '%s'", filename);
        }
    }

    if (file_exists) {
        // Open the file without trying to create it
        if ( (fd = open(filename, O_WRONLY | append_mode)) < 0) {
            errExit("could not open file '%s' for writing", filename);
        }
    }

    char token = 'x';
    for (size_t i = 0; i < count; ++i) {
        if (!append_mode) {
            if (lseek(fd, 0, SEEK_END) < 0) {
                errExit("could not lseek in file '%s'", filename);
            }
        }
        if (writep(fd, &token, sizeof(token)) < 0)
            errExit("writep() failed");
    }

    close(fd);
    return 0;
}
