# Answer

The data appears always at the end of the file because the `O_APPEND` flag always seeks at the end of the file before each write operation. According to the `open(2)` man pages:

> The file is opened in append mode. Before each `write(2)`, the file offset is positioned at the end of the file, as if with `lseek(2)`.  The modification of the file offset and the write  operation are performed as a single atomic step.

Therefore our `lseek` has no effect.
