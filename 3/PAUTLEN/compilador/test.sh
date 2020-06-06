#!/bin/bash
nasm -g -o probar.o -f elf32 salida.asm 
gcc -m32 -o probar probar.o alfalib.o 
./probar 
