/*
 *  Various utility functions.
 */

#ifndef _MYLIB_H
#define _MYLIB_H

#include <stddef.h>
#include <unistd.h>

#define RUN_TEST(fn) do {printf("%s... ", #fn); (fn)(); printf("[OK]\n");} while (0);

/*
 *  Persistent write.
 *
 *  This function will try to write all count bytes persistently.
 */
ssize_t writep(int fd, const void *buf, size_t count);

#endif  // _MYLIB_H
