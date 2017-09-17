//#ifndef WIN32_LEAN_AND_MEAN
//#define WIN32_LEAN_AND_MEAN
//#endif

#include "../headers/dns_resolver.h"
#include "../headers/debug.h"
#include "../headers/winsock_handler.h"
#include "../headers/dns_servers_info.h"
#include "../headers/program_error.h"
#include "../headers/dns_lib.h"
#include "../headers/args.h"

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
	result->error = calloc(sizeof(REQUEST_PROCESS_ERROR), 1);
	
	char *ip = prepare_valid_ip_string(info->dns_server_ip);
	if(ip == NULL)
	{
		result->result_type = REQUEST_PROCESS_RESULT_ERROR;
		result->error->error_message = "Invalid IP address string.\n";
		return result;
	}
	
	
	
}

char* prepare_valid_ip_string(char *arg_ip_string)
{
	// arg is something like: "124.32.21.1" ?
	if(is_ipv4_string(arg_ip_string))
	{
		return arg_ip_string;
	}
	// maybe its constant value (ROOT_A, ROOT_B ???)
	char *ip = get_root_dns_server_ip(arg_ip_string);
	// if its valid const value, function above returns pts to string ip
	// otherwise returns null. In both cases we can return this value.
	
	return ip;
}

char* get_root_dns_server_ip(char *constant_value)
{
	char *arg = constant_value;
	
	if(strcmp(arg, "ROOT_A") == 0)
	{
		return ROOT_A_v4;
	}
	else if(strcmp(arg, "ROOT_B") == 0)
	{
		return ROOT_B_v4;
	}
	else if(strcmp(arg, "ROOT_C") == 0)
	{
		return ROOT_C_v4;
	}
	else if(strcmp(arg, "ROOT_D") == 0)
	{
		return ROOT_D_v4;
	}
	else if(strcmp(arg, "ROOT_E") == 0)
	{
		return ROOT_E_v4;
	}
	else if(strcmp(arg, "ROOT_F") == 0)
	{
		return ROOT_F_v4;
	}
	else if(strcmp(arg, "ROOT_G") == 0)
	{
		return ROOT_G_v4;
	}
	else if(strcmp(arg, "ROOT_H") == 0)
	{
		return ROOT_H_v4;
	}
	else if(strcmp(arg, "ROOT_I") == 0)
	{
		return ROOT_I_v4;
	}
	else if(strcmp(arg, "ROOT_J") == 0)
	{
		return ROOT_J_v4;
	}
	else if(strcmp(arg, "ROOT_K") == 0)
	{
		return ROOT_K_v4;
	}
	else if(strcmp(arg, "ROOT_L") == 0)
	{
		return ROOT_L_v4;
	}
	else if(strcmp(arg, "ROOT_M") == 0)
	{
		return ROOT_M_v4;
	}
	
	
	return NULL;
}


int is_ipv4_string(char *ip_string)
{
	// X == number from 0-9
	// "X[1-3].X[1-3].X[1-3].X[1-3]"
	
	int len = strlen(ip_string);
	int numbers_in_chunk = 0;
	int dots = 0;
	int ok = 1;
	
	for(int i = 0; i < len; i++)
	{
		if((dots > 3) || (numbers_in_chunk > 3))
		{
			ok = -1;
			break;
		}
		
		if(is_number(ip_string[i]) > 0)
		{
			numbers_in_chunk++;
		}
		else if(ip_string[i] == '.')
		{
			if(numbers_in_chunk == 0)
			{
				// dot after dot eg
				// 123.2..1
				ok = -1;
				break;
			}
			numbers_in_chunk = 0;
			dots++;
		}
		else 
		{
			//some char which cannot be in IP string (not number and dot)
			ok = -1;
			break;
		}
	}
	// 3 dots, must be some number after last dot: 111.22.3.4
	if((dots != 3) || (numbers_in_chunk == 0))
		ok = -1;
	
	return ok;
}

int is_number(char c)
{
	if((c >= '0') && (c <= '9'))
		return 1;
	else return -1;
}

int check_protocol_type(char *arg_protocol_string)
{
	if((strcmp(arg_protocol_string, "tcp") == 0) || (strcmp(arg_protocol_string, "udp") == 0))
		return 1;
	return 0;
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








