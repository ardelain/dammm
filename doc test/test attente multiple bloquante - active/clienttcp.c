#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc,char** argv) {

  char tampon[1000];

  if (argc<2) {
    printf("Usage : %s <num. port>\n",argv[0]);
    return 1;
  }
  
  int i,t,port=atoi(argv[1]);
  printf("Envoi sur le port %d\n",port);

  int s=socket(AF_INET,SOCK_STREAM,0);
  printf("descripteur = %d\n",s);
  
  struct sockaddr_in sout;
  sout.sin_family=AF_INET;
  sout.sin_port=htons(port);
  inet_aton("127.0.0.1",&sout.sin_addr);
  for(i=0;i<8;i++){
	  sout.sin_zero[i]=0;  
  }
  //fgets(tampon, sizeof(tampon),stdin);
  //t=sendto(s,tampon,strlen(tampon),0,(struct sockaddr *)&sout,sizeof(sout));
  //printf("%d octets envoyés.\n",t);

  int ret=connect(s,(struct sockaddr *)&sout,sizeof(sout));
  if (ret<0) {
    printf("Erreur connect !\n");
    exit(1);
  }
  printf("Connexion réussie\n");
  while (fgets(tampon, sizeof(tampon),stdin)!=NULL) {
    t=send(s,tampon,strlen(tampon),0);
    printf("%d octets envoyés.\n",t);
    if(t==5 && strncmp(tampon, "quit\n", 4)==0) exit(1);
  }
  close(s);
  printf("fin normale\n");
  return 0;
}
