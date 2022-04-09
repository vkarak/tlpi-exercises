# Interesting conclusions on my test system

1. At the beginning of the program the end of the heap is way beyond the end of the static uninitialized data (`&end`).
   I don't know if there are any sort of heap allocations happening before `main()`.
2. `printf()` does a bunch of heap allocations, that's why is important to move the `sbrk(0)` print at the beginning of the program in order to show that the first `malloc()` indeed pushes the program break.
3. The program break is pushed 33 pages every time.
4. `malloc()` allocates 16 bytes more for each one of the `4KB` allocations requested.
