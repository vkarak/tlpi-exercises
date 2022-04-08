# tlpi-exercises
My version of the solutions of the exercises of "The Linux Programming Interface" book

## Installing cmocka

Some of the solutions require the [cmocka](https://cmocka.org/) unit testing framework.
This is how you can install it:

```
git clone https://git.cryptomilk.org/projects/cmocka.git
cd cmocka && mkdir build && cd build
cmake ..
make
cd ../../
```

## Build the solutions


```
make -C tlpi-dist
make -C tlpi-solutions
```

Solutions have been tested with GCC 9.3.0 on Ubuntu 20.04.
