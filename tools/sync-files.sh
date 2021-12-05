#!/bin/bash

TLPI_PREFIX=$HOME/Repositories/tlpi-exercises
REMOTE_DIR=bkk-linux-testbed:tlpi-exercises

scp -r $TLPI_PREFIX/ $REMOTE_DIR/
