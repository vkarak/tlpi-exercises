# Question

Assume in each of the following cases that the initial set of process user IDs is *real=1000 effective=0 saved=0 file-system=0*. What would be the state of the user IDs after the following calls?

1. `setuid(2000);`
2. `setreuid(–1, 2000);`
3. `seteuid(2000);`
4. `setfsuid(2000);`
5. `setresuid(–1, 2000, 3000);`


# Answer

>  `setuid(2000);`

*real=2000 effective=2000 saved=2000 file-system=2000*

>  `setreuid(–1, 2000);`

*real=1000 effective=2000 saved=2000 file-system=2000*

>  `seteuid(2000);`

*real=1000 effective=2000 saved=0 file-system=2000*

>  `setfsuid(2000);`

*real=1000 effective=0 saved=0 file-system=2000*

>  `setresuid(–1, 2000, 3000);`

*real=1000 effective=2000 saved=3000 file-system=2000*
