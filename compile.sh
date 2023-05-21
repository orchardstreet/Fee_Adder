#! /bin/bash

#sudo apt install libgtk-3-dev

gcc fee_adder.c -o fee_addr `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`
