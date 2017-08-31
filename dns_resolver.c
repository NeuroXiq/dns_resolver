#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "dns_resolver.h"
#include "dns_lib.c"
#include "debug.c"
#include "winsock_handler.c"
#include "dns_servers_info.h"
#include "args.c"
#include "dns_msg_print.c"

#define ROOT_S_K "8.8.8.8"

char* test(int *len);

int main(int argc, char argv[])
{
	
	WSADATA wsadata;
	sh_prepare_env(&wsadata);
	
	
	/* SENDING TEST */
	SOCK_HANDLER *sh = sh_create_udp("8.8.8.8", 53);
	int len;
	char *to_send = test(&len);
	sh_send(sh,to_send,len);
	
	puts("sended");
	
	char *receive = calloc(0x800, 1);
	int r_len = sh_receive(sh, receive, 0x800);
	
	
	printf("RECEIVED: %d\n", r_len);
	//
	//
	//DEBUG_hex_dump(receive, r_len, 16);
	MESSAGE_PARSE_RESULT *result = get_message(receive, r_len);
	
	DEBUG_hex_dump(receive, r_len, 16);
	
	if(result->type == RESULT_ERROR)
	{
		
		printf("%s\n", result->error->message);
		//return 1;
	}
	
	print_header(result->message);
	
	print_questions(result->message);
	
	print_resources(result->message->Answer,     result->message->Header.ANCOUNT, "ANSWER");
	
	print_resources(result->message->Authority,  result->message->Header.NSCOUNT, "AUTHORITY");
	
	print_resources(result->message->Additional, result->message->Header.ARCOUNT, "ADDITIONAL");
	
	sh_close_env();
	
	return 0;
}

char* test(int *len)
{
	DNS_MESSAGE *msg = calloc(sizeof(DNS_MESSAGE),1);
	msg->Header.ID[0] = 0xbe;
	msg->Header.ID[1] = 0xef;
	msg->Header.QR = QUERY;
	msg->Header.QDCOUNT = 1;
	msg->Header.RD = 1;
	
	msg->Question = calloc(0x30,1);
	//msg->Question[0].QNAME = "\x03""www""\x06""google""\x03""com""\x00";
	msg->Question[0].QNAME = "\x03""www""\x02""wp""\x02""pl""\x00";
	msg->Question[0].QTYPE = ALL;
	msg->Question[0].QCLASS = INTERNET;
	
	char *raw;
	*len = build_dns_message(msg,&raw);
	
	return raw;
	
}