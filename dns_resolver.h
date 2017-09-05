#ifndef _DNS_RESOLVER__
#define _DNS_RESOLVER__

#include "args.c"

typedef struct REQUEST_PROCESS_INFO
{
	char *server_ip;
	char *domain_name;
	int protocol;
	int v;
	int vv;
} REQUEST_PROCESS_INFO;

int execute_args(ARGS_INFO *args);
int run_builder();
int execute_resolve_args(ARGS_INFO *args);
//int send_request();
int mandatory_args_exist(ARGS_INFO *args, char **error);

#endif