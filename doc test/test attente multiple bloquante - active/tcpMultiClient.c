#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <signal.h>

int main(int argc, char** argv){
	char* id = 0;
	short sport = 0;
	int sock = 0;
	pid_t pid;
	struct sockaddr_in moi;
	struct sockaddr_in serveur;
	int ret;
	socklen_t len;
	char buf_read[256];
	
	if (argc != 4) {
		fprintf(stderr,"usage: %s pseudo @IPserveur port\n",argv[0]);
		exit(1);
	}
	id = argv[1];
	sport = atoi(argv[3]);
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr,"%s: socket %s\n", argv[0],strerror(errno));
		exit(1);
	}
	
	serveur.sin_family = AF_INET;
	serveur.sin_port = htons(sport);
	inet_aton(argv[2],(struct in_addr *)&serveur.sin_addr);
	bzero(&(serveur.sin_zero), 8);
	
	socklen_t s = sizeof(serveur);
	if (connect(sock,(struct sockaddr *)&serveur,s) < 0) {
		fprintf(stderr,"%s: connect %s\n",argv[0],strerror(errno));
		perror("bind");
		exit(1);
	}
	
	len = sizeof(moi);
	getsockname(sock,(struct sockaddr *)&moi,&len);
	if ((pid = fork()) < 0) {
		printf("erreur fork\n");
		exit(-1);
	}
	else if (pid==0) for(;;) {
		ret = read(sock,buf_read,256);
		if (ret <= 0) {
			printf("%s:erreur dans read (num=%d,mess=%s)\n", argv[0], ret, strerror(errno));
			exit(2);
		}
	
		printf("%s\n",buf_read);
		}
		else {
			char message[200];
			while (strcmp(message, "q") != 0) {
				char buf_write[256];
				fgets(message, 100, stdin);
				if (message[strlen(message)-1] == "\n") message[strlen(message)-1] = 0;
				sprintf(buf_write, "%s : %s", id, message);
				ret = write(sock,buf_write,strlen(buf_write)+1);
				if (ret <= strlen(buf_write)) {
					printf("%s: erreur dans write (num=%d,mess=%s)\n",argv[0],ret,strerror(errno));
					continue;
				}
			}
			printf("Connexion close\n");
			kill(pid, SIGKILL);
			shutdown(sock, SHUT_RDWR);
		}
	return 0;
}
