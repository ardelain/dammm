#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <errno.h>
 
#include <unistd.h>     /* pour read(2)/write(2) */
 
#include <netdb.h>      /* pour getaddrinfo*/
#include <string.h>     /* pour memset */
 
#include <arpa/inet.h>  /* pour inet_ntop */
 
#define LINE_MAX 1024  /* taille MAX en réception */
#define GROUP   "230.0.0.0"
#define PORT	"5000"




int main(int argc, char **argv) {
  	struct sockaddr_in dst_addr, src_addr;	
	struct hostent * hostent;
	int s, ret;
	socklen_t len_src_addr;
 
	char response[LINE_MAX];
 	int taille;
 	taille = strlen(argv[1]);
	char* msg = (char*)malloc(taille);
	strcpy(msg, argv[1]);


	if((s=socket(AF_INET, SOCK_DGRAM, 0))==-1) {
		perror("socket"); exit(1);
	}

	if((hostent=gethostbyname(GROUP))==NULL) {
		herror("gethostbyname"); exit(1);
	}

	dst_addr.sin_family = AF_INET;
	dst_addr.sin_port = htons(atoi(PORT));  
	dst_addr.sin_addr = *((struct in_addr *)hostent->h_addr);
	memset(dst_addr.sin_zero, '\0', sizeof(dst_addr.sin_zero));
	
	int ttl=2;
	if(setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, 
		(void *) &ttl, sizeof(ttl)) == -1)
	   perror("set IP_MULTICAST_TTL");

	/*int loopback=0;
	if(setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, 
		(void *) &loopback, sizeof(loopback)) == -1)
	   perror("set IP_MULTICAST_LOOP");*/

	if(sendto(s, msg, strlen(msg)+1, 0, \
	   (struct sockaddr*) &dst_addr, sizeof(dst_addr))==-1) {
		perror("sendto"); exit(1);
	}

 
  return 0;
}
