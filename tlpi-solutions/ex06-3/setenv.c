#include "mylib.h"
#include "tlpi_hdr.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

// Keep track of the environment variable pointers allocated by us, so as to
// free() them if unset and avoid memory leaks
static char **EnvList = NULL;
static size_t NextEnvVar;


int xunsetenv(const char *name)
{
    if (!name || !strlen(name) || strchr(name, '=')) {
        errno = EINVAL;
        return -1;
    }

    char **ep;
    for (ep = environ; *ep != NULL; ep++) {
        if (!strncmp(*ep, name, strlen(name))) {
            // We found the variable
            **ep = '\0';
        }
    }

    return 0;
}

int xsetenv(const char *name, const char *value, int overwrite)
{
    static size_t EnvListCapacity = 1024;

    if (!name || !strlen(name) || strchr(name, '=')) {
        errno = EINVAL;
        return -1;
    }

    size_t name_len  = strlen(name);
    size_t value_len = strlen(value);

    // Check if requested variable already exists
    char *name_exists = getenv(name);
    if (name_exists && !overwrite) {
        return 0;
    } else if (name_exists) {
        char **ep;
        for (ep = environ; *ep != NULL; ep++) {
            if (!strncmp(*ep, name, name_len)) {
                // Check if we can reuse the available space
                char *existing_value = strchr(*ep, '=') + 1;
                if (strlen(value) < strlen(existing_value)) {
                    strcpy(*ep + name_len + 1, value);
                    return 0;
                } else {
                    // Check if we allocated this value
                    char **np;
                    for (np = EnvList; *np != NULL; np++) {
                        if (*np == *ep) {
                            // We allocated the buffer; realloc() to get enough
                            // space
                            *ep = realloc(*ep, name_len + value_len + 2);
                            if (!*ep) {
                                errno = ENOMEM;
                                return -1;
                            }
                            *np = *ep;  // Save the new pointer
                            strcpy(*ep + name_len + 1, value);
                            putenv(*ep);
                            return 0;
                        }
                    }
                    if (*np == NULL) {
                        // We have not allocated this variable; unset it and
                        // break
                        xunsetenv(name);
                        break;
                    }
                }
            }
        }
    }

    // Prepare NewEnvVars array
    if (!EnvList) {
        EnvList = malloc(EnvListCapacity*sizeof(*EnvList));
        if (!EnvList) {
            errno = ENOMEM;
            return -1;
        }
        NextEnvVar = 0;
    }

    // We need to account for the '=' and the '\0' at the end
    char *envvar = malloc(name_len + value_len + 2);
    if (!envvar) {
        errno = ENOMEM;
        return -1;
    }

    // Store the ennvar pointer
    if (NextEnvVar == EnvListCapacity - 1) {
        // Double the capacity and realloc
        EnvListCapacity <<= 1;
        EnvList = realloc(EnvList, EnvListCapacity*sizeof(*EnvList));
        if (!EnvList) {
            errno = ENOMEM;
            return -1;
        }
    }

    EnvList[NextEnvVar++] = envvar;
    EnvList[NextEnvVar] = NULL;

    envvar[0] = '\0';
    strcat(envvar, name);
    strcat(envvar, "=");
    strcat(envvar, value);
    if (putenv(envvar)) {
        return -1;
    }

    return 0;
}

static void test_xsetenv(void **state)
{
    assert_int_equal(xsetenv(NULL, "foo", 0), -1);
    assert_int_equal(errno, EINVAL);

    assert_int_equal(xsetenv("", "foo", 0), -1);
    assert_int_equal(errno, EINVAL);

    assert_int_equal(xsetenv("FOO=", "foo", 0), -1);
    assert_int_equal(errno, EINVAL);

    xsetenv("FOO", "bar", 0);
    assert_string_equal(getenv("FOO"), "bar");

    // Attempt to reset FOO with overwrite=0
    xsetenv("FOO", "1", 0);
    assert_string_equal(getenv("FOO"), "bar");

    // Attempt to reset FOO with overwrite=1
    xsetenv("FOO", "1", 1);
    assert_ptr_not_equal(getenv("FOO"), NULL);
    assert_string_equal(getenv("FOO"), "1");

    // Attempt to reset FOO with a larger value;
    xsetenv("FOO", "foobar", 1);
    assert_ptr_not_equal(getenv("FOO"), NULL);
    assert_string_equal(getenv("FOO"), "foobar");

    // Reset an existing variable
    char *path_orig = strdup(getenv("PATH"));
    xsetenv("PATH", "/bin", 1);
    assert_ptr_not_equal(getenv("PATH"), NULL);
    assert_string_equal(getenv("PATH"), "/bin");

    // Restore the original PATH and unset FOO
    setenv("PATH", path_orig, 1);
    unsetenv("FOO");
    free(path_orig);
}

static void test_xunsetenv(void **state)
{
    assert_int_equal(xunsetenv(NULL), -1);
    assert_int_equal(errno, EINVAL);

    assert_int_equal(xunsetenv(""), -1);
    assert_int_equal(errno, EINVAL);

    assert_int_equal(xunsetenv("FOO="), -1);
    assert_int_equal(errno, EINVAL);

    setenv("FOO", "bar", 0);
    assert_int_equal(xunsetenv("FOO"), 0);
    assert_ptr_equal(getenv("FOO"), NULL);
}

int main()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_xsetenv),
        cmocka_unit_test(test_xunsetenv),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
    return 0;
}
