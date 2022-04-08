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
    char **ep;
    size_t pos;
    for (ep = environ, pos = 0; *ep != NULL; ep++, pos++) {
        if (**ep == '\0')
            continue;

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

    // Check if requested variable already exists
    char *name_exists = getenv(name);
    if (name_exists && !overwrite) {
        return 0;
    } else if (name_exists) {
        // Remove the environment variable
        xunsetenv(name);
    }

    size_t name_len  = strlen(name);
    size_t value_len = strlen(value);

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
    int rc;

    assert_true(xsetenv(NULL, "foo", 0) < 0);
    assert_int_equal(errno, EINVAL);

    assert_true(xsetenv("", "foo", 0) < 0);
    assert_int_equal(errno, EINVAL);

    assert_true(xsetenv("FOO=", "foo", 0) < 0);
    assert_int_equal(errno, EINVAL);

    xsetenv("FOO", "bar", 0);
    assert_string_equal(getenv("FOO"), "bar");

    // Attempt to reset FOO with overwrite=0
    xsetenv("FOO", "1", 0);
    assert_string_equal(getenv("FOO"), "bar");

    // Attempt to reset FOO with overwrite=1
    xsetenv("FOO", "1", 1);
    assert_string_equal(getenv("FOO"), "1");
}


int main()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_xsetenv),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
    return 0;
}
