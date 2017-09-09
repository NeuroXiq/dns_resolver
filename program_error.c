#include "program_error.h"
#include <string.h>

/*
*
* This file contains functions, which build some error messages 
* for dns_resolver executable. This file not include building/parsing DNS_MESSAGE errors.
* To check errors for DNS_MESSAGE parsing/building check dns_msg_error.c file.
*
*/


char* perror_empty_args()
{
	char *error = "Argument list are empty.\n"
				  "'-help' for more informations\n";
	return error;
}

char* perror_unrecognized_arg(char *arg)
{
	int len = strlen(arg);
	int error_len = len + 0x40;
	char *error = calloc(error_len, 1);
	
	snprintf(error, error_len,
	"Unrecogized argument '%s'\n", arg);
	
	return error;
	
}

char* perror_arg_not_found(char *arg)
{
	int len = strlen(arg)+1+0x30;
	char *error = calloc(len, 1);
	
	snprintf(error, len,
	"No mandatory '%s' argument can be found.\n", arg);
	
	return error;
}

char *perror_arg_value_not_found(char *arg)
{
	int len = strlen(arg) + 0x30;
	char *error = calloc(len, 1);
	
	snprintf(error, len,
	"Cannot find value of '%s' argument\n", arg);
	
	return error;
}