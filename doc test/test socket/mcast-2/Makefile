all : recv send

recv.o: recv.c
	gcc -c recv.c -o recv.o

recv: recv.o
	gcc -o recv recv.o

send.o: send.c
	gcc -c send.c -o send.o

send: send.o
	gcc -o send send.o

clean:
	rm -f *.o recv send

tgz:
	tar cvzf srcPartie1.tgz recv.c send.c Makefile