all : tcpMultiServer tcpMultiClient

tcpMultiServer.o: tcpMultiServer.c
	gcc -c tcpMultiServer.c -o tcpMultiServer.o

tcpMultiServer: tcpMultiServer.o
	gcc -o tcpMultiServer tcpMultiServer.o

tcpMultiClient.o: tcpMultiClient.c
	gcc -c tcpMultiClient.c -o tcpMultiClient.o

tcpMultiClient: tcpMultiClient.o
	gcc -o tcpMultiClient tcpMultiClient.o

clean:
	rm -f *.o tcpMultiServer tcpMultiClient

tgz:
	tar cvzf srcTestMulti.tgz tcpMultiServer.c tcpMultiClient.c Makefile