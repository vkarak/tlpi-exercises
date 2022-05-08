# Question

Is a process with the following user IDs privileged? Explain your answer.

*real=0 effective=1000 saved=1000 file-system=1000*

# Answer

Currently the process is not executing in privileged mode but it can acquire `root` privileges by calling `setuid(0)` or `seteuid(0)`.
The reason is that these calls allow unprivileged processes to change their effective UID to either the value of their real UID (here 0) or their saved-set UID.
So, effectively, this process can escalate privileges any time.
