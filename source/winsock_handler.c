#include "winsock_handler.h"
#include <stdio.h>

int sh_close_env()
{
	WSACleanup();
	return 0;
}

int sh_receive(
 SOCK_HANDLER *ush,
 char *buffer, 
 int buf_size)
 {
	int result = 0 ;
	int slen = sizeof(struct sockaddr_in);
	
	result = 
	recvfrom(
	ush->sock,
	buffer,
	buf_size,
	0,
	(struct sockaddr *)(&ush->sockinfo), 
	&slen);
	
	return result;
 }


static SOCK_HANDLER* sh_create(char *server_ip, unsigned short server_port, int proto)
{
	SOCK_HANDLER *sh = calloc(sizeof(SOCK_HANDLER),1);
	
	int sock_type;
	
	if(proto == IPPROTO_UDP)
	{
		sock_type = SOCK_DGRAM;
	}
	else if(proto == IPPROTO_TCP)
	{
		sock_type = SOCK_STREAM;
	}
	else 
	{
		return NULL;
	}
	
	sh->sock = socket(AF_INET, sock_type, proto);
	sh->protocol = proto;
	
	memset((char*)&sh->sockinfo,0,sizeof(struct sockaddr_in));
	
	sh->sockinfo.sin_family = AF_INET;
	sh->sockinfo.sin_port = htons(server_port);
	sh->sockinfo.sin_addr.S_un.S_addr = inet_addr(server_ip);
	
	if(sh->sock == SOCKET_ERROR)
	{
		free(sh);
		return NULL;
	}
	else
	{
		return sh;
	}
	
}

/*
* Public
*/


int sh_tcp_open_connection(SOCK_HANDLER *sh)
{
	int result = connect(
	sh->sock,
	(struct sockaddr *)&sh->sockinfo,
	sizeof(struct sockaddr_in)
	);
	
	return result;
}
int sh_tcp_close_connection(SOCK_HANDLER *sh)
{
	closesocket(sh->sock);
	
	return 0;
}

SOCK_HANDLER* sh_create_udp(char *server_ip, unsigned short port)
{
	return sh_create(server_ip, port, IPPROTO_UDP);
}

SOCK_HANDLER* sh_create_tcp(char *server_ip, unsigned short port)
{
	return sh_create(server_ip, port, IPPROTO_TCP);
}

int sh_delete_handler(SOCK_HANDLER *ush)
{
	if(ush != NULL)
	{
		free(ush);
	}
	return 0;
}
 
int sh_send(SOCK_HANDLER *ush, char *buffer, int buf_length)
{
	
	int result = sendto(
	ush->sock,
	buffer,
	buf_length,
	0, 
	(struct sockaddr *)(&ush->sockinfo),
	sizeof(struct sockaddr_in));
	
	return result;
}

int sh_prepare_env(WSADATA *wsadata)
{
	return WSAStartup(MAKEWORD(2,2),wsadata);
}
