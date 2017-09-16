#ifndef __WINSOCK_HANDLER_
#define __WINSOCK_HANDLER_

#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SOCK_HANDLER
{
	unsigned int protocol;
	SOCKET sock;
	struct sockaddr_in sockinfo;
	
} SOCK_HANDLER;

 
static SOCK_HANDLER* sh_create(char *server_ip, unsigned short server_port, int family);

int sh_prepare_env(WSADATA *wsadata);
int sh_close_env();

SOCK_HANDLER* sh_create_udp(char *server_ip, unsigned short port);
SOCK_HANDLER* sh_create_tcp(char *server_ip, unsigned short port);

int sh_tcp_open_connection(SOCK_HANDLER *sh);
int sh_tcp_close_connection(SOCK_HANDLER *sh);
int sh_receive(SOCK_HANDLER *sh, char *buffer, int buf_size);
int sh_send(SOCK_HANDLER *s, char *buf,int len);
int sh_delete_handler(SOCK_HANDLER *ush);

#endif