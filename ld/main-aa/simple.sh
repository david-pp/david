#!/bin/bash

gcc -fPIC -c main.c a1.c a2.c

ar r liba1/liba.a a1.o
ranlib liba1/liba.a

ar r liba2/liba.a a2.o
ranlib liba2/liba.a

gcc -shared -o liba1/liba.so a1.o
gcc -shared -o liba2/liba.so a2.o

echo 'liba1'
gcc -o a main.o -L./liba1 -la
./a; echo $?

echo 'liba2'
gcc -o a main.o -L./liba2 -la
./a; echo $?

echo 'liba1 liba2'
gcc -o a main.o -L./liba1 -L./liba2 -la
./a; echo $?
