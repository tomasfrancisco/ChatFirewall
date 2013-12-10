#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>

#define BUF_SIZE 1024

void erro(char *msg);
void process_client(int client_fd);
void process_server(int server_fd, char *username);

int main(int argc, char * argv[]){
	char endServer[100];
	char username[100];
	int fd;
	struct sockaddr_in addr;
	struct hostent *hostPtr;

	int nread = 0;
	char buffer[BUF_SIZE];

	char final[BUF_SIZE + 200];
	final[0] = '\0';

	if (argc != 4){
		printf("cliente <host> <port> <username>\n");
		exit(-1);
	}

	strcpy(endServer,argv[1]);
	if ((hostPtr = gethostbyname(endServer)) == 0)	//Encontra a ligação através do nome
		erro("Nao consegui obter o endereço");

	bzero((void *)&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
	addr.sin_port = htons((short)atoi(argv[2]));

	if ((fd = socket(AF_INET,SOCK_STREAM,0)) == -1)	//Cria um novo socket - SOCK_STREAM (TCP link)
		erro("socket");

	if (connect(fd,(struct sockaddr *)&addr, sizeof(addr)) < 0)	//Conecta ao servidor
		erro("connect");

	//*********************************************** Aguarda pergunta pelo ip e porta **********************************
	nread = read(fd, buffer, BUF_SIZE-1);
	buffer[nread - 1] = '\0';	//Termina a string
	puts(buffer);
	fflush(stdout);								//Limpa o buffer de saida
	fflush(stdin);

	fgets(final, BUF_SIZE + 200 - 1, stdin);

	write(fd, final, 1 + strlen(final));	//Escreve para o servidor
	final[0] = '\0';

	nread = read(fd, buffer, BUF_SIZE-1);
	buffer[nread - 1] = '\0';	//Termina a string
	puts(buffer);
	fflush(stdout);								//Limpa o buffer de saida
	fflush(stdin);

	fgets(final, BUF_SIZE + 200 - 1, stdin);

	write(fd, final, 1+strlen(final));	//Escreve para o servidor
	final[0] = '\0';
	//*********************************************** Aguarda pergunta pelo ip e porta **********************************
	fflush(stdin);
	strcpy(username,argv[3]);
	write(fd, username, 1 + strlen(username));	//Escreve para o servidor o user
	printf("username: %s\n", username);

	if (fork() == 0) 
	{
		process_client(fd);
		exit(0);
	}
	else
	{
		process_server(fd, username);
		exit(0);
	}
	close(fd);
	exit(0);
}

void process_client(int client_fd)
{
	int nread = 0;
	char buffer[BUF_SIZE];

	//------ Leitura do cliente ----------
	while(1)
	{
		printf("%c[2K", 27);
		nread = read(client_fd, buffer, BUF_SIZE-1);
		buffer[nread - 2] = '\0';	//Termina a string
		puts(buffer);
		fflush(stdout);								//Limpa o buffer de saida
		fflush(stdin);
	}
	
	close(client_fd);
}

void process_server(int server_fd, char *username)
{
	char buffer[BUF_SIZE];
	char usersay[200];
	char final[BUF_SIZE + 200];
	final[0] = '\0';
	
	sprintf(usersay,"%s disse: ", username);	
	while(1)
	{		
		fgets(buffer, BUF_SIZE - 1, stdin);

		if(strcmp(buffer, ".quit\n") == 0)
		{
			break;
		}

		strcat(final, usersay);
		strcat(final, buffer);

		write(server_fd, final, 1+strlen(final));	//Escreve para o servidor
		final[0] = '\0';
	}
}

void erro(char *msg){
	printf("ERROR: %s\n",msg);
	exit(-1);
}