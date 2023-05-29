#! /bin/bash

gcc src/fee_adder.c -o fee_adder `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`
