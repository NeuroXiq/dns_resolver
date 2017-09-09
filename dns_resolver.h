#ifndef _DNS_RESOLVER__
#define _DNS_RESOLVER__

#include "args.c"
#include "dns_lib.c"

typedef enum REQUEST_PROCESS_RESULT_TYPE
{
	ERROR = 0,
	SUCCESS = 1
	
} REQUEST_PROCESS_RESULT_TYPE;

typedef struct REQUEST_PROCESS_INFO
{
	char *dns_server_ip;
	char *domain_name;
	int   protocol;
	int v;
	int vv;
	
} REQUEST_PROCESS_INFO;

typedef struct REQUEST_PROCESS_RESULT
{
	int result;
	MESSAGE_PARSE_RESULT *parse_result;
	
	
	
} REQUEST_PROCESS_RESULT;

int execute_args(ARGS_INFO *args);
int run_builder();
int execute_resolve_args(ARGS_INFO *args);
int mandatory_args_exist(ARGS_INFO *args, char **error);
int show_help();
int vinfo(REQUEST_PROCESS_INFO *info, char *message);
int vvinfo(REQUEST_PROCESS_INFO *info, char *message);

REQUEST_PROCESS_RESULT* run_request_process(REQUEST_PROCESS_INFO *info);

#endif