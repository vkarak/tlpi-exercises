/*
 *  Various utility functions.
 */

#ifndef _MYLIB_H
#define _MYLIB_H

#undef _BEGIN_C_DECLS
#undef _END_C_DECLS
#ifdef __cplusplus
#   define _BEGIN_C_DECLS extern "C" {
#   define _END_C_DECLS }
#else
#   define _BEGIN_C_DECLS /* empty */
#   define _END_C_DECLS /* empty */
#endif

#include <stddef.h>
#include <unistd.h>

#define XMIN(a, b) ((a) < (b) ? (a) : (b))
#define XMAX(a, b) ((a) > (b) ? (a) : (b))

#define RUN_TEST(fn) do {printf("%s... ", #fn); (fn)(); printf("[OK]\n");} while (0);


_BEGIN_C_DECLS

/*
 *  Persistent write.
 *
 *  This function will try to write all count bytes persistently.
 */
ssize_t writep(int fd, const void *buf, size_t count);

_END_C_DECLS

#endif  // _MYLIB_H
