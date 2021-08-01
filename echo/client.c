#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

void error_handling(char *message);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);

char name[NAME_SIZE]="[DEFALULT]";
char msg[BUF_SIZE];

int main(int argc, char *argv[])
{
	int sock;
	pid_t pid;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in serv_adr;

	if(argc!=4) {
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}
	sprintf(name, "[%s]", argv[3]);
	sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	
	pid=fork();
	if(pid==0)
		write_routine(sock, msg);
	else 
		read_routine(sock, msg);
	/*while(1) 
	{
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);
		
		if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
			break;

		char name_msg[NAME_SIZE+BUF_SIZE];
		sprintf(name_msg,"%s %s", name, message);
		write(sock, name_msg, strlen(name_msg));
		
		char name_msgp[NAME_SIZE+BUF_SIZE];
		str_len=read(sock, name_msgp, BUF_SIZE-1);
		name_msgp[str_len]=0;
		fputs(name_msgp,stdout);
	}
	*/
	close(sock);
	return 0;
}
void read_routine(int sock, char *buf)
{	char name_msgp[NAME_SIZE+BUF_SIZE];
	while(1)
	{
		int str_len=read(sock, name_msgp, NAME_SIZE+BUF_SIZE);
		if(str_len==0)
			return;

		name_msgp[str_len]=0;
		fputs(name_msgp,stdout);
	}
}
void write_routine(int sock, char *buf)
{	char name_msg[NAME_SIZE+BUF_SIZE];
	while(1)
	{
		fgets(buf, BUF_SIZE, stdin);
		if(!strcmp(buf,"q\n") || !strcmp(buf,"Q\n"))
		{	
			shutdown(sock, SHUT_WR);
			return;
		}
		sprintf(name_msg,"%s %s", name, buf);
		write(sock, name_msg, strlen(name_msg));
	}
}
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}