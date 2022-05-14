# Exercise

If a process whose user IDs all have the value *X* executes a set-user-ID program whose user ID, *Y*, is nonzero, then the process credentials are set as follows:

```
real=X effective=Y saved=Y
```

*(We ignore the file-system user ID, since it tracks the effective user ID.) Show the `setuid()`, `seteuid()`, `setreuid()`, and `setresuid()` calls, respectively, that would be used to perform the following operations:*

1. Suspend and resume the set-user-ID identity (i.e., switch the effective user ID to the value of the real user ID and then back to the saved set-user-ID).

```c
uid_t ruid, euid, suid;
getresuid(&ruid, &euid, &suid);

// Switch the euid to the ruid
seteuid(ruid);  // or setuid(ruid), since euid is non-zero;
                // the other calls can be made equivalent to `seteuid()` so they are omitted

// Switch the euid back to the suid
seteuid(suid);  // or setuid(suid), since euid is non-zero; similarly
                // the other calls can be made equivalent to `seteuid()` so they are omitted
```


2. Permanently drop the set-user-ID identity (i.e., ensure that the effective user ID and the saved set-user-ID are set to the value of the real user ID).

```c

uid_t ruid, euid, suid;
getresuid(&ruid, &euid, &suid);

setresuid(-1, ruid, ruid);
```
