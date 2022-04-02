# Question 06-1

> Although the program contains an array (mbuf) that is around 10 MB in size, the executable file is much smaller than this. Why is this?

The big arrays have uninitialized data, so there is no need to actually allocate that much space in the executable.
All you need is the starting address and the size of the arrays.
