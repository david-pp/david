#!/bin/bash

# main.o 依赖库a, 库a和库b循环依赖

gcc -c main.c a.c a2.c b.c

ar r liba.a a.o a2.o
ranlib liba.a

ar r libb.a b.o
ranlib libb.a

rm -f a
echo 'FAILED: -l./ -la -lb'
gcc -o a main.o -L./ -la -lb
./a; echo $?

rm -f a
echo 'FAILED: -l./ -lb -la'
gcc -o a main.o -L./ -lb -la
./a; echo $?

rm -f a
echo 'FAILED: -l./ -lb -la -lb'
gcc -o a main.o -L./ -lb -la -lb
./a; echo $?

rm -f a
echo 'PASS: -l./ -la -lb -la'
gcc -o a main.o -L./ -la -lb -la
./a; echo $?

rm -f a
echo 'PASS: start-group'
gcc -o a main.o -L./ -Wl,--start-group -la -lb -Wl,--end-group
./a; echo $?

rm -f a
echo 'PASS: start-group'
gcc -o a main.o -L./ -Wl,--start-group -lb -la -Wl,--end-group
./a; echo $?

