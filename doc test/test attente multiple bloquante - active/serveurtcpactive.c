#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc,char** argv) {

  char tampon[1000];

  if (argc<2) {
    printf("Usage : %s <num. port>\n",argv[0]);
    return 1;
  }
  
  int i,t,port=atoi(argv[1]);
  printf("Ecoute du port %d\n",port);

  int s=socket(AF_INET,SOCK_STREAM,0);
  printf("descripteur = %d\n",s);
  
  struct sockaddr_in sin,corres;
  sin.sin_family=AF_INET;
  sin.sin_port=htons(port);
  sin.sin_addr.s_addr=htonl(INADDR_ANY);
  for(i=0;i<8;i++) sin.sin_zero[i]=0;
  
  int ret=bind(s,(struct sockaddr *)&sin,sizeof(struct sockaddr_in));
  if (ret<0) printf("erreur bind !");

  ret=listen(s,5);
  if (ret<0) printf("erreur listen !");
	
  
  printf("Ecoute...\n");
  socklen_t taille_cor=sizeof(struct sockaddr_in);
  int fd1=accept(s,(struct sockaddr *)&corres,&taille_cor);
  printf("Connexion reçue, descripteur %i\n",fd1);
  int fd2=accept(s,(struct sockaddr *)&corres,&taille_cor);
  printf("Connexion reçue, descripteur %i\n",fd2);
  int fd3=accept(s,(struct sockaddr *)&corres,&taille_cor);
  printf("Connexion reçue, descripteur %i\n",fd3);

  int attr= fcntl(fd1,F_GETFL); attr=attr|O_NONBLOCK;
  ret=fcntl(fd1,F_SETFL,attr); if (ret<0) printf("erreur fcntl !");
  attr= fcntl(fd2,F_GETFL); attr=attr|O_NONBLOCK;
  ret=fcntl(fd2,F_SETFL,attr); if (ret<0) printf("erreur fcntl !");
  attr= fcntl(fd3,F_GETFL); attr=attr|O_NONBLOCK;
  ret=fcntl(fd3,F_SETFL,attr); if (ret<0) printf("erreur fcntl !");

  int fin=0;

  while(!fin){
    usleep(1000);
    t=recv(fd1,tampon,sizeof(tampon),0);
    if (t!=-1) printf("%d octets reçus.\n",t);
      for(i=0;i<t;i++) printf("%c",tampon[i]);
    if (t==5 && strncmp(tampon,"quit\n",4)==0) fin=1; 
    t=recv(fd2,tampon,sizeof(tampon),0);
    if (t!=-1) printf("%d octets reçus.\n",t);
      for(i=0;i<t;i++) printf("%c",tampon[i]);
    if (t==5 && strncmp(tampon,"quit\n",4)==0) fin=1; 
    t=recv(fd3,tampon,sizeof(tampon),0);
    if (t!=-1) printf("%d octets reçus.\n",t);
      for(i=0;i<t;i++) printf("%c",tampon[i]);
    if (t==5 && strncmp(tampon,"quit\n",4)==0) fin=1; 

  }


  printf("Connexion fermée\n");
  close(fd1);
  close(fd2);
  close(fd3);
  
  
  close(s);
  return 0;
}