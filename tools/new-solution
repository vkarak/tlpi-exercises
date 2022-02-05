#!/usr/bin/env python3
#
# Generate basic structure of a new solution
#

import functools
import os
import shutil
import sys

PROGNAME = sys.argv[0]
PREFIX = os.path.dirname(__file__)

print_err = functools.partial(print, file=sys.stderr)


def print_usage():
    print_err(f'Usage: {PROGNAME} EXERCISE EXE_NAME')


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print_err(f'{PROGNAME}: too few arguments')
        print_usage()
        sys.exit(1)

    try:
        chap, exer_no = [int(x) for x in sys.argv[1].split('-')]
    except ValueError:
        print_errr(f'{PROGNAME}: invalid exercise number format: '
                   f'please use <number>-<number>')
        sys.exit(1)

    exdir = os.path.join('tlpi-solutions', f'ex{chap:02}-{exer_no}')
    exec_name = sys.argv[2]

    os.makedirs(exdir)

    # Generate Makefile from the template
    with open(os.path.join(PREFIX, 'Makefile.in')) as fp:
        makefile = fp.read()

    with open(os.path.join(exdir, 'Makefile'), 'w') as fp:
        fp.write(makefile.format(exec_name=exec_name))

    # Generate the base source file from the template
    shutil.copy(os.path.join(PREFIX, 'source.c.in'),
                os.path.join(exdir, f'{exec_name}.c'))
