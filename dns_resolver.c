//#ifndef WIN32_LEAN_AND_MEAN
//#define WIN32_LEAN_AND_MEAN
//#endif

#include "dns_resolver.h"
#include "dns_lib.c"
#include "debug.c"
#include "winsock_handler.c"
#include "dns_servers_info.h"
#include "dns_msg_print.c"
#include "program_error.c"

#define GOOGLE "8.8.8.8"

int main(int argc, char *argv[])
{	
	ARGS_INFO *args = calloc(sizeof(ARGS_INFO), 1);
	args->count = argc;
	args->arguments = argv;

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
	else 
	{
		execute_resolve_args(args);
	}
	
	return 0;
}

int run_builder()
{
	return 0;
}

int execute_resolve_args(ARGS_INFO *args)
{
	int arg_error = 0;
	int value_not_found = 0;
	char *server_ip   ;
	char *domain_name ;
	
	char *error;
	
	if(mandatory_args_exist(args, &error) < 1)
	{
		printf(error);
	}

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
	
	if(arg_get_value(args, "-name"))
	{
		*error = perror_arg_value_not_found("-name");
		return -1;
	}
	
	return 1;
}












