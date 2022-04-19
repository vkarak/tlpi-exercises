# Explanation

`getpwuid()` stores its result in a static structure.
So the result of one call gets overwritten from the result of the other before `printf()` is actually called.
As a result, the same user name is always printed.
Also, since the order of function argument evaluation is unspecified, the printed user name can either be the first or the second one, but it will always be the same.
In my case, the user name corresponding to `uid2` was always printed.
