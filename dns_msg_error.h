#ifndef _DNS_MSG_ERROR__
#define _DNS_MSG_ERROR__

typedef enum PARSE_ERROR_TYPE
{
	UNDEFINED_ERROR = 0,
	HEADER_ERROR = 1,
	QUESTION_ERROR = 2,
	RESOURCE_ERROR = 3
	
} PARSE_ERROR_TYPE;

typedef struct ERROR_INFO
{
	int type;
	char *message;
} ERROR_INFO;

int error_header(ERROR_INFO *error, char *message);
int error_question_qname_format(ERROR_INFO *error, int offset);
int error_question_qname_size(ERROR_INFO *error, int offset, int real_qname_size);
int error_resource_name_format(ERROR_INFO *error, int offset);


#endif