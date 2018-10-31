#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>     /* pour read(2)/write(2) */
 
#include <netdb.h>      
#include <string.h>     /* pour memset */
 
#include <ctype.h>      /* pour toupper */
 
#include <arpa/inet.h>  /* pour inet_ntop */
 
#define REQUEST_MAX 1024  /* taille MAX en réception */
#define GROUP   "230.0.0.0"
#define PORT	"5000"



int main(int argc, char **argv){

  struct sockaddr_in addr, src_addr;
  struct ip_mreqn mreqn;
  int s, ret;
  socklen_t len_src_addr;
  char request[REQUEST_MAX];

  if((s=socket(AF_INET, SOCK_DGRAM, 0))==-1) {
    perror("socket"); exit(1);
  }

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl (INADDR_ANY);
  addr.sin_port = htons(atoi(PORT));
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  if(inet_aton(GROUP, &mreqn.imr_multiaddr)==0) {
	fprintf(stderr,"Pb Adr multicast : %s !\n", GROUP);
	exit(1);
}
 
mreqn.imr_address.s_addr = htonl(INADDR_ANY);
mreqn.imr_ifindex=0; /* n'importe quelle interface */
 
if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
			(void *) &mreqn, sizeof(struct ip_mreqn)) < 0)
	perror("setsockopt – IP_ADD_MEMBERSHIP");

  if(bind(s, (struct sockaddr *)&addr, sizeof addr)) {
    perror("bind"); exit(1);
  }

while(1){
 
 	len_src_addr=sizeof src_addr;
    if((ret=recvfrom(s, request, REQUEST_MAX-1, 0,
        (struct sockaddr*) &src_addr, &len_src_addr))==-1) {
      perror("recvfrom"); exit(1);
    }
  request[ret]=0;
  printf("%s\n", request);
}

}