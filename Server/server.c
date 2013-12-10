/*******************************************************************
* SERVIDOR no porto 9000, à escuta de novos clientes. Quando surjem
* novos clientes os dados por eles enviados são lidos e descarregados no ecran.
*******************************************************************/

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>

#define SERVER_PORT 9001
#define BUF_SIZE 1024

void process_client(int fd);
void erro(char *msg);
int check_vowel(char c);
void delete_vowels(char *buffer, char *buffer_to_client);

int main(int argc, char * argv[]) 
{
	int fd, client, server;
	struct sockaddr_in addr, client_addr;
	int client_addr_size;
	char username[100];

	if (argc != 2){
		printf("server <username>\n");
		exit(-1);
	}

	bzero((void *) &addr, sizeof(addr));

	addr.sin_family = AF_INET;					//Dominio - Familia de endereços no protocolo da internet
	addr.sin_addr.s_addr = htonl(INADDR_ANY);	
	addr.sin_port = htons(SERVER_PORT);

	//int socket(int domain, int type, int protocol) = Descritor de socket
	if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)	//Cria um novo socket; fd = descritor do socket
		erro("na funcao socket");
	//int bind(int fd, const struct sockaddr *address, socklen_t address_len) = 0 se sucesso, -1 se erro
	if ( bind(fd,(struct sockaddr*)&addr,sizeof(addr)) < 0)	//Associa um socket a um determinado endereço
		erro("na funcao bind");
	//int listen(int fd, int backlog) = 0 se sucesso, -1 se erro
	if( listen(fd, 5) < 0)
		erro("na funcao listen");

	strcpy(username,argv[1]);
	
	while (1) 
	{
		client_addr_size = sizeof(client_addr);
		//int accept(int fd, const struct sockaddr *address, socklent_t* address_len) = Descritor do socket da ligação aceite
		client = accept(fd,(struct sockaddr *)&client_addr,&client_addr_size);
		server = client;
		if (client > 0)	//Se recebeu ligação cria um processo
		{
			if (fork() == 0) 
			{
				close(fd);
				process_client(client);
				exit(0);
			}
			else
			{
				close(fd);
				process_server(server,username);
				exit(0);
			}
			close(client);
			close(server);
		}
	}
	return 0;
}

void process_client(int client_fd)
{
	int nread = 0;
	char buffer[BUF_SIZE];
	char buffer_to_client[BUF_SIZE];

	//------ Leitura do cliente ----------
	while(1)
	{
		nread = read(client_fd, buffer, BUF_SIZE-1);
		buffer[nread-2] = '\0';
		puts(buffer);
		fflush(stdout);								//Limpa o buffer de saida
	}
	
	close(client_fd);
}

void process_server(int server_fd, char* username)
{
	char buffer[BUF_SIZE];
	char usersay[200];
	usersay[0]='\0';
	char final[BUF_SIZE + 200];
	final[0] = '\0';

	sprintf(usersay, "%s disse: ", username);

	while(1)
	{
		fgets(&buffer, BUF_SIZE -1, stdin);

		if(strcmp(buffer, ".quit"))
		{
			break;
		}

		strcat(final,usersay);
		strcat(final, buffer);
		write(server_fd,final,1+strlen(final));	//Escreve para o servidor
		final[0] ='\0';
	}
}

void erro(char *msg)
{
	printf("Erro: %s\n", msg);
	exit(-1);
}