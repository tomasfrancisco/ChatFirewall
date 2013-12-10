#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>

#define FIREWALL_PORT 9000

#define IPSIZE 100
#define ANSSIZE 100
#define PORTSIZE 100
#define BUF_SIZE 1024

void erro(char *msg);
void process_client(int client_fd, int server_fd);

int main(int argc, char * argv[]){

	int fdclient, fdserver, client;
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;
	int client_addr_size;
	struct hostent *hostPtr;

	char ans[ANSSIZE];
	char ip[IPSIZE];
	char port[PORTSIZE];
	int nread;

	pid_t pid;

	char username[100] = "admin";
	char usertemp[100];
	char password[100] = "admin";
	char passtemp[100];

	while(1)
	{
		printf("Username: ");
		scanf("%s", usertemp);

		printf("Password: ");
		scanf("%s", passtemp);

		if((strcmp(usertemp, username) != 0) || (strcmp(passtemp, password) != 0))
		{
			printf("Credenciais inválidas...\nTente de novo.\n");
			continue;
		}		
		break;
	}
	printf("Bem vindo!\n");
	printf("A aguardar ligação...");

	//************************************************************************************************ Recebe ligação do client.c 
	bzero((void *) &client_addr, sizeof(client_addr));
	client_addr.sin_family = AF_INET;					//Dominio - Familia de endereços no protocolo da internet
	client_addr.sin_addr.s_addr = htonl(INADDR_ANY);	
	client_addr.sin_port = htons(FIREWALL_PORT);

	//int socket(int domain, int type, int protocol) = Descritor de socket
	if ( (fdclient = socket(AF_INET, SOCK_STREAM, 0)) < 0)	//Cria um novo socket; fd = descritor do socket
		erro("na funcao socket");
	
	//int bind(int fd, const struct sockaddr *address, socklen_t address_len) = 0 se sucesso, -1 se erro
	if ( bind(fdclient,(struct sockaddr*)&client_addr,sizeof(client_addr)) < 0)	//Associa um socket a um determinado endereço
	{
		close(fdclient);
		erro("na funcao bind");
	}
	
	//int listen(int fd, int backlog) = 0 se sucesso, -1 se erro
	if( listen(fdclient, 5) < 0)
		erro("na funcao listen");

	printf("A receber ligação...");
	while(1)
	{
		client_addr_size = sizeof(client_addr);
		client = accept(fdclient,(struct sockaddr *)&client_addr,&client_addr_size);

		if(client > 0)
		{
			sprintf(ans, "Insira o ip do servidor: ");
			write(client, ans, 1+strlen(ans));	//Pede o ip do servidor ao cliente
			ans[0] = '\0';

			nread = read(client, ip, IPSIZE-1);
			ip[nread] = '\0';						//Termina a string
			printf("%s\n", ip);
			fflush(stdout);								//Limpa o buffer de saida

			sprintf(ans, "Insira a porta de ligação: ");
			write(client, ans, 1+strlen(ans));	//Pede o ip do servidor ao cliente
			ans[0] = '\0';

			nread = read(client, port, PORTSIZE-1);
			port[nread] = '\0';						//Termina a string
			printf("%s\n", port);
			fflush(stdout);	
			break;
		}
	}

	//*********************************************************************************************** Efectua a ligação ao server.c
	if ((hostPtr = gethostbyname(ip)) == 0)	//Encontra a ligação através do nome
		erro("Nao consegui obter o endereço");

	bzero((void *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons((short)atoi(port));
	server_addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
	
	if ((fdserver = socket(AF_INET,SOCK_STREAM,0)) == -1)	//Cria um novo socket - SOCK_STREAM (TCP link)
		erro("socket");

	if (connect(fdserver,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)	//Conecta ao servidor
		erro("connect");

	printf("Ligação efectuada ao endereço %s na porta %s...\n", ip, port);
	
	if (client > 0)	//Se a ligação ainda existir
	{
		if ((pid = fork()) == 0) 
		{
			close(fdclient);
			process_client(client, fdserver);
			exit(0);
		}
		else
		{
			close(fdclient);
			process_client(fdserver, client);
			kill(pid, SIGTERM);
		}
		close(client);
		close(fdserver);
	}
	return 0;
}

void process_client(int client_fd, int server_fd)
{
	int nread = 0;
	char buffer[BUF_SIZE];

	//------ Leitura do cliente ----------
	while(1)
	{
		nread = read(client_fd, buffer, BUF_SIZE-1);
		buffer[nread-2] = '\0';
		puts(buffer);
		fflush(stdout);								//Limpa o buffer de saida

		if(strstr(buffer, ".quit") != NULL)
		{
			break;
		}

		write(server_fd, buffer, 1+strlen(buffer));	
		buffer[0] = '\0';
	}
	
	close(client_fd);
}

/*void process_server(int server_fd, char* username)
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
		strcat(final,usersay);
		strcat(final, buffer);
		write(server_fd,final,1+strlen(final));	//Escreve para o servidor
		final[0] ='\0';
	}
}*/

void erro(char *msg)
{
	printf("Erro: %s\n", msg);
	exit(-1);
}