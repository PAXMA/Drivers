obj-m += Cdriver.o

Cdriver.o: Cdriver.c
	make -C/usr/src/linux-headers-`uname -r` M=`pwd` modules