#ifndef _DNS_RESOLVER__
#define _DNS_RESOLVER__

#include "args.c"
#include "dns_lib.c"

typedef enum REQUEST_PROCESS_RESULT_TYPE
{
	REQUEST_PROCESS_RESULT_ERROR = 0,
	REQUEST_PROCESS_RESULT_SUCCESS = 1
	
} REQUEST_PROCESS_RESULT_TYPE;

typedef struct REQUEST_PROCESS_INFO
{
	char *dns_server_ip;
	char *domain_name;
	char *protocol;
	int v;
	int vv;
	
} REQUEST_PROCESS_INFO;

typedef struct REQUEST_PROCESS_ERROR
{
	char *error_message;
	
} REQUEST_PROCESS_ERROR;

typedef struct REQUEST_PROCESS_RESULT
{
	int result_type;
	MESSAGE_PARSE_RESULT *parse_result;
	REQUEST_PROCESS_ERROR *error;
	
	
} REQUEST_PROCESS_RESULT;

int execute_args(ARGS_INFO *args);
int run_builder();
int execute_resolve_args(ARGS_INFO *args);
int mandatory_args_exist(ARGS_INFO *args, char **error);
int show_help();
int vinfo(REQUEST_PROCESS_INFO *info, char *message);
int vvinfo(REQUEST_PROCESS_INFO *info, char *message);
char* get_root_dns_server_ip(char *constant_key);

int is_ipv4_string(char *ip_string);
char* prepare_valid_ip_string(char *arg_ip_string);
int is_number(char c);

REQUEST_PROCESS_RESULT* run_request_process(REQUEST_PROCESS_INFO *info);

#endif