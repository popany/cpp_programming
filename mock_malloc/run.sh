#!/usr/bin/bash

export LD_PRELOAD=./lib/libmy_malloc.so
./src/demo_app
