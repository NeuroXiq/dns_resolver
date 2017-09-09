//#ifndef WIN32_LEAN_AND_MEAN
//#define WIN32_LEAN_AND_MEAN
//#endif

#include "dns_resolver.h"
#include "debug.c"
#include "winsock_handler.c"
#include "dns_servers_info.h"
#include "dns_msg_print.c"
#include "program_error.c"

#define GOOGLE "8.8.8.8"

int main(int argc, char *argv[])
{	
	ARGS_INFO *args = calloc(sizeof(ARGS_INFO), 1);
	args->count = argc - 1;
	args->arguments = (argv + 1); // ignore 1 arg (path of exe file)
	
	execute_args(args);
	
	return 0;
}

int execute_args(ARGS_INFO *args)
{
	if(args->count == 0)
	{
		char *message = perror_empty_args();
		printf("%d\n",message);
		return 1;
	}
	
	if(args->count == 1)
	{
		if(arg_contains(args, "-help") || 
			arg_contains(args, "help") ||
			arg_contains(args, "-h")   ||
			arg_contains(args, "/h")   ||
			arg_contains(args, "/?")   ||
			arg_contains(args, "-?"))
		{
			show_help();
			return 0;
		}
		
		if(arg_contains(args, "-build"))
		{
			run_builder();
		}
		else 
		{
			char *error = perror_unrecognized_arg(args->arguments[0]);
			printf(error);
			return 2;
		}
	}
	else /* run-execute-close */ 
	{
		execute_resolve_args(args);
	}
	
	return 0;
}



int run_builder()
{
	return 0;
}

int show_help()
{
	puts("Help should be there ....");
	return 0;
}

int execute_resolve_args(ARGS_INFO *args)
{
	int arg_error = 0;
	int value_not_found = 0;
	
	char *error;
	
	if(mandatory_args_exist(args, &error) < 1)
	{
		printf(error);
		return -1;
	}
	
	REQUEST_PROCESS_INFO *rp_info = calloc(sizeof(REQUEST_PROCESS_INFO), 1); 

	rp_info->domain_name = arg_get_value(args, "-name");
	rp_info->dns_server_ip = arg_get_value(args, "-server");
	rp_info->v = (arg_contains(args, "-v") > -1) ? 1 : 0;
	rp_info->vv = (arg_contains(args, "-vv") > -1) ? 1 : 0;	
	
	if(arg_contains(args, "-prot") > -1)
	{
		rp_info->protocol = arg_get_value(args, "-prot");
	}
	else rp_info->protocol = "udp";
	
	run_request_process(rp_info);
	
	return 0;
}

int mandatory_args_exist(ARGS_INFO *args, char **error)
{
	if(arg_contains(args, "-server") < 0)
	{
		*error = perror_arg_not_found("-server");
		return -1;
	}
	
	if(arg_contains(args, "-name") < 0)
	{
		*error = perror_arg_not_found("-name");
		return -1;
	}
	
	if(arg_get_value(args, "-server") == NULL)
	{
		*error = perror_arg_value_not_found("server");
		return -1;
	}
	
	if(arg_get_value(args, "-name") == NULL)
	{
		*error = perror_arg_value_not_found("-name");
		return -1;
	}
	
	return 1;
}

REQUEST_PROCESS_RESULT* run_request_process(REQUEST_PROCESS_INFO *info)
{
	REQUEST_PROCESS_RESULT *result = calloc(sizeof(REQUEST_PROCESS_RESULT), 1);
	
	if(strcmp(info->protocol, "tcp") == 0)
	{
		
	}
	else if(strcmp(strcmp(info->protocol, "udp"))
	
	
}

char* prepate_valid_ip_string(char *arg_ip_string)
{
	
}

int check_protocol_type(char *arg_protocol_string)
{
	if((strcmp(arg_protocol_string, "tcp") == 0) || (strcmp(arg_protocol_string, "udp") == 0))
		return 1;
	return 0
}

int vinfo(REQUEST_PROCESS_INFO *info, char *message)
{
	if(info->v > 0)
	{
		printf("%s\n",message);
	}
	
	return 0;
}
int vvinfo(REQUEST_PROCESS_INFO *info, char *message)
{
	if(info->vv > 0)
	{
		printf("%s\n", message);
	}
	return 0;
}








