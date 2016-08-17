#!/bin/bash

# .o 依赖库a, 库a依赖库b

gcc -c main.c a.c b.c

ar r liba.a a.o
ranlib liba.a

ar r libb.a b.o
ranlib libb.a

rm -f a
gcc -o a main.o -L./ -la -lb
./a; echo $?

# failed
rm -f a
gcc -o a main.o -L./ -lb -la
./a; echo $?

# failed
rm -f a
gcc -o a main.o -L./ -la
./a; echo $?
