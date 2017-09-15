#include "dns_msg_error.h"
#include <stdio.h>
#include <stdlib.h>

int error_header(ERROR_INFO* error, char *message)
{
	
	error->type = HEADER_ERROR;
	error->message = message;
	
	return 0;
}

int error_question_qname_format(ERROR_INFO *error, int offset)
{
	char *message = calloc(0x30, 1);
	snprintf(message,0x30, 
	"Cannot parse qname section from Question record.\n"
	"Raw data offset: %d\n", offset);
	
	error->type = QUESTION_ERROR;
	error->message = message;
	
	return 0;
}

int error_question_qname_size(ERROR_INFO *error, int offset, int qname_real_len)
{
	char *message = calloc(0x30, 1);
	snprintf(message, 0x30,
	"QNAME section in Question record is too big.\n"
	"Maximum length is 255 bytes but actual value is :%d\n", qname_real_len);
	
	error->type = QUESTION_ERROR;
	error->message = message;
	
	return 0;
}

int error_resource_name_format(ERROR_INFO *error, int offset)
{
	char *message = calloc(0x60, 1);
	
	snprintf(message, 0x60, 
	"Cannot parse name section in resource record.\n"
	"Raw data offset:%d\n", offset);
	
	error->type = RESOURCE_ERROR;
	error->message = message;
	
	return 0;
}

int error_resource_out_of_buffer(ERROR_INFO *error, char *detail_msg, 
int offset, int expected_bytes)
{
	char *message = calloc(0x200, 1);
	
	snprintf(message, 0x200,
	"Cannot parse resource section.\n"
	"Buffer length is less than expected.\n"
	"Raw data offset: %d\n"
	"Expected bytes from this point: %d\n"
	"Details: %s\n", offset, expected_bytes, detail_msg);
	
	error->type = RESOURCE_ERROR;
	error->message = message;
	
	return 0;
}