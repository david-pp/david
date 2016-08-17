#!/bin/bash

# main.o 依赖库a, 库a和库b循环依赖

gcc -c main.c a.c b.c

ar r liba.a a.o
ranlib liba.a

ar r libb.a b.o
ranlib libb.a

rm -f a
gcc -o a main.o -L./ -la -lb
./a; echo $?

echo '-l./ -lb -la'
# failed
rm -f a
gcc -o a main.o -L./ -lb -la
./a; echo $?

echo '-l./ -la'
# failed
rm -f a
gcc -o a main.o -L./ -la
./a; echo $?
