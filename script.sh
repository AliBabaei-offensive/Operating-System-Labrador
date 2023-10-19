#! /bin/bash

clear;

cd ..;

make clean;


make;

sudo insmod iutnode.ko

sudo mknod /dev/iutnode c 509 0

clear

cd 2/
sudo rm a.out
gcc 2.c

cd ..

sudo ./2/a.out

sudo rm /dev/iutnode
sudo rmmod iutnode

make clean;

