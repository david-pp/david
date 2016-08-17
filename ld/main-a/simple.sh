#!/bin/bash

gcc -c main.c a.c

##################

rm -f a
gcc -o a main.o a.o
./a
echo $?

rm -f a
gcc -o a a.o main.o
./a
echo $?

##################

ar r liba.a a.o
ranlib liba.a

rm -f a
gcc -o a main.o liba.a
./a
echo $?


rm -f a
gcc -o a main.o -la -L./
gcc -o a main.o -L./ -la
./a
echo $?

# failed
rm -f a
gcc -o a liba.a main.o
./a
echo $?

# failed
rm -f a
gcc -o a -L./ -la main.o
./a
echo $?
