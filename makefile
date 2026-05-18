# nmake makefile
#
# Tools used:
#  Compile::Watcom Resource Compiler
#  Compile::GNU C
#  Make: nmake or GNU make
# GNU Make / NMAKE Makefile for OS/2 GCC
all : bgrid.exe
bgrid.exe : bgrid.obj bgrid.def 
	gcc -Zomf bgrid.obj bgrid.def -o bgrid.exe
    
bgrid.obj : bgrid.c bgrid.h
	gcc -Wall -Zomf -c -O2 bgrid.c -o bgrid.obj
clean :
	rm -rf *.exe *.RES *.obj *.hlp