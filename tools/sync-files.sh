#!/bin/bash

TLPI_PREFIX=$HOME/Repositories/tlpi-exercises
REMOTE_DIR=bkk-linux-testbed:tlpi-exercises

rsync -avz $* $TLPI_PREFIX/ $REMOTE_DIR/
