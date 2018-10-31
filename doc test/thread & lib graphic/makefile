
all:test
	gcc -c test.c -I/tmp/graphic/usr/include
	gcc -o test test.o -L/tmp/graphic/usr/lib -lgraphic -lX11 -lpthread

clean: 
	rm -f *.o test
