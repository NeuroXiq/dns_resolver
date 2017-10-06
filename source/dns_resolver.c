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

//#pragma comment(lib, "ws2_32.lib")

int main(int argc, char *argv[])
{
	
	ARGS_INFO *args = calloc(10000/*(sizeof(ARGS_INFO)*/, 1);
	args->count = argc - 1;
	args->arguments = (argv + 1); // ignore first arg (path of exe file)
	
	execute_args(args);
	
	
	return 0;
}

int execute_args(ARGS_INFO *args)
{
	if(args->count == 0)
	{
		char *message = perror_empty_args();
		printf("%s\n",message);
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
	
	REQUEST_PROCESS_RESULT *result = run_request_process(rp_info);
	
	if(result->result_type == REQUEST_PROCESS_RESULT_SUCCESS)
	{
		unsigned char *ip_bytes = extract_ip_from_dns_answer(result->parse_result->message);
		printf("RESOLVED: %u.%u.%u.%u\n", ip_bytes[0], ip_bytes[1],ip_bytes[2],ip_bytes[3]);
	}
	else
	{
		printf("Error 	!\n");
		printf("%s\n",result->error->error_message);
	}
	
	return 0;
}

unsigned char* extract_ip_from_dns_answer(DNS_MESSAGE *msg)
{
	
	for(int i = 0; i < msg->Header.ANCOUNT; i++)
	{
		//type class == 1
		if( (msg->Answer[i].TYPE == A) &&
			(msg->Answer[i].CLASS == INTERNET))
			{
				return (unsigned char*)(msg->Answer[i].RDATA);
			}
		
	}
	int *null_addr = calloc(4,1);
	return (unsigned char*)(null_addr);
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
	
	DNS_MESSAGE* message;
	
	int preparing_result = prepare_dns_message_struct(info, &message);	
	
	if(preparing_result < 0)
	{
		result->result_type = REQUEST_PROCESS_RESULT_ERROR;
		result->error->error_message = "INTERNAL_ERROR__ :: Cannot create DNS_MESSAGE struct.";
		return result;
	}
	
	MESSAGE_PARSE_RESULT *resolving_result = try_resolve(info, message);
	
	result->result_type = REQUEST_PROCESS_RESULT_SUCCESS;
	
	result->parse_result = resolving_result;
	
	//destroy_dns_message(message); // free all allocated spac in DNS_MESSAGE
	
	return result;
}

MESSAGE_PARSE_RESULT* try_resolve(REQUEST_PROCESS_INFO *info, DNS_MESSAGE *request_data)
{
	
	char *raw_dns_message;
	int raw_request_len = build_dns_message(request_data, &raw_dns_message);
	
	//winsock_handler -> sending/receiving packets.
	WSADATA *wdata;
	int r = sh_prepare_env(wdata);
	SOCK_HANDLER *shandler = sh_create_udp(info->dns_server_ip, 53);
	
	sh_send(shandler, raw_dns_message, raw_request_len);
	
	char *raw_received_bytes = calloc(0x1000, 1);
	int raw_received_len = sh_receive(shandler, raw_received_bytes, 0x1000);
	
	MESSAGE_PARSE_RESULT *parsed_message = get_message(raw_received_bytes, raw_received_len);
	
	sh_delete_handler(shandler);
	sh_close_env();
	
	return parsed_message;
}

int prepare_dns_message_struct(REQUEST_PROCESS_INFO *info, DNS_MESSAGE **out_message)
{
	/* ip can contains some constant strings e.g. ROOT_A, it must replaced with some ip data eg.(123.23.11.32) */
	char *ip = prepare_valid_ip_string(info->dns_server_ip);
	printf("%s\n", ip);
	if(ip == NULL)
	{
		return -1;
	}
	
	// ip can be converted from constant but if contains valid ip string
	// nothing is changed. If constant was changed we can free it.
	if(ip != info->dns_server_ip)
	{
		free(info->dns_server_ip);
	}
	
	info->dns_server_ip = ip;
	
	DNS_MESSAGE *message = create_dns_msg_struct(info);
	if(message == NULL)
	{
		// error handling
		//out_error_msg = "INTERNAL_ERROR :: \n \
		//							   \rCannot build DNS_MESSAGE with specified data.";
		return -1;
	}
	
	*out_message = message;
	
	return 1;
}

DNS_MESSAGE* create_dns_msg_struct(REQUEST_PROCESS_INFO *info)
{
	char *dns_format_domain_name = create_dns_format_name(info->domain_name);
	
	DNS_MESSAGE *dns_msg = calloc(sizeof(DNS_MESSAGE), 1);
	
	/*  DNS MESSAGE HEADER */
	
	dns_msg->Header.ID[0] = 'N';
	dns_msg->Header.ID[1] = 'X';
	
	dns_msg->Header.QR = QUERY; //query/response
	dns_msg->Header.Opcode = STANDARD_QUERY;
	dns_msg->Header.AA =
	dns_msg->Header.TC =
	dns_msg->Header.RA =
	dns_msg->Header.Z =
	dns_msg->Header.RCODE = 0;
	dns_msg->Header.RD = 1;
	
	dns_msg->Header.QDCOUNT = 1; // MUST be 1 query
	dns_msg->Header.ANCOUNT = 0;
	dns_msg->Header.NSCOUNT = 0;
	dns_msg->Header.ARCOUNT = 0;
	
	/* DNS MESSAGE QUESTION */
	
	dns_msg->Question = calloc(sizeof(DNS_MESSAGE_QUESTION) ,1);
	dns_msg->Question->QNAME = dns_format_domain_name;
	dns_msg->Question->QTYPE = A; // A == host address (see TYPE_TYPE enum in dns_lib.h or RFC)
	dns_msg->Question->QCLASS = INTERNET;
	
	return dns_msg;
}

char* create_dns_format_name(char *name)
{ 

	int len = strlen(name);
	char *formated = calloc(len+2,1); // + null byte + first len byte
	
	int counter = 0;
	int next_insert = 0;
	
	for(int i = 0; i < len; i++)
	{
		if((name[i] != '.') && (i < len))
		{
			formated[i + 1] = name[i];
			counter++;
		}
		else 
		{
			//meet dot now
			formated[next_insert] = counter;
			next_insert = i + 1;
			counter = 0;
		}
	}
	
	formated[next_insert] = counter; 
	
	return formated;
}

char* prepare_valid_ip_string(char *arg_ip_string)
{
	// arg is something like: "124.32.21.1" ?
	int validation = is_ipv4_string(arg_ip_string);
	
	if(validation >= 0)
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








