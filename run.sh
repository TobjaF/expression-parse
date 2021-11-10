#!/bin/bash
gcc -Wall -g  -c main.c -o obj/main.o
gcc  -o bin/expr_parse.exe obj/main.o
./bin/expr_parse.exe