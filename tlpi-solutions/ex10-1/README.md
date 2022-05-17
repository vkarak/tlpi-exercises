# Question

Assume a system where the value returned by the call `sysconf(_SC_CLK_TCK)` is 100. Assuming that the `clock_t` value returned by `times()` is a signed 32-bit integer, how long will it take before this value cycles so that it restarts at 0? Perform the same calculation for the `CLOCKS_PER_SEC` value returned by `clock()`.

# Answer

- Assuming that the first call to `times()` returns 0, the return value will overflow after (2^31 - 1)/100 = 21474836.47s = 248d 13h 13m 56.47s
- The call to `clock()` will overflow after (2^31 - 1)/1000000 = 2147.483647s = 35m 47.483647s
