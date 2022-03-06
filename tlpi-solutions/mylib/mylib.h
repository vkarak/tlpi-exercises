/*
 *  Various utility functions.
 */

#ifndef _MYLIB_H
#define _MYLIB_H

#include <stddef.h>

#define RUN_TEST(fn) {printf("%s... ", #fn); (fn)(); printf("[OK]\n");}

/*
 *  Persistent write.
 *
 *  This function will try to write all count bytes persistently or fail and
 *  exit if it does not succeed.
 */
void writep(int fd, const void *buf, size_t count);


#endif  // _MYLIB_H
