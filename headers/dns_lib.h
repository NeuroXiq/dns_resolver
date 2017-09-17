#ifndef _DNS_LIB_H__
#define _DNS_LIB_H__

#include <stdint.h>
#include "../headers/dns_msg_error.h"

/*
*
* All Headers, structures and 
* Enums are defined below.
*
*/	

typedef enum HEADERMASK
{
	QR_MASK = 0x8000,
	OPCODE_MASK = 0x7800,
	AA_MASK = 0x400,
	TC_MASK = 0x200,
	RD_MASK = 0x100,
	RA_MASK = 0x80,
	Z_MASK = 0x70,
	RCODE_MASK = 0xF,
	
	QR_MASK_SHIFT = 15,
	OPCODE_MASK_SHIFT = 11,
	AA_MASK_SHIFT = 10,
	TC_MASK_SHIFT = 9,
	RD_MASK_SHIFT = 8,
	RA_MASK_SHIFT = 7,
	Z_MASK_SHIFT = 4,
	RCODE_MASK_SHIFT = 0
	
} HEADERMASK;

typedef enum QR_TYPE
{
	QUERY = 0,
	RESPONSE = 1
} QR_TYPE;

typedef enum OPCODE_TYPE 
{
	STANDARD_QUERY = 0,
	INVERSE_QUERY = 1,
	SERVER_STATUS_REQUEST = 2,
	
} OPCODE_TYPE;

typedef enum RCODE_TYPE
{
	NO_ERROR_CONDITION = 0,
	FORMAT_ERROR = 1,
	SERVER_FAILURE = 2,
	NAME_ERROR = 3,
	NOT_IMPLEMENTED = 4,
	REFUSE = 5
	
} RCODE_TYPE;

typedef enum QTYPE_TYPE
{
	AXFR   = 252,  // A request for a transfer of an entire zone
	MAILB  = 253,  // A request for mailbox-related records (MB, MG or MR)
    MAILA  = 254,  //A request for mail agent RRs (Obsolete - see MX)
	ALL    = 255   //A request for all records
	
} QTYPE_TYPE;

typedef enum CLASS_TYPE
{
	INTERNET = 1, // the Internet
	CSNET = 2,    // the CSNET class (Obsolete - used only for examples in
	CHAOS = 3,    // the CHAOS class
	HESIOD = 4    //Hesiod [Dyer 87]
	
} QCLASS_TYPE;

typedef enum TYPE_TYPE
{
	A      = 1,  // a host address
	NS     = 2,  //an authoritative name server
	MD     = 3,  //a mail destination (Obsolete - use MX)
	MF     = 4,  //a mail forwarder (Obsolete - use MX)
	CNAME  = 5,  //the canonical name for an alias
	SOA    = 6,  //marks the start of a zone of authority
	MB     = 7,  //a mailbox domain name (EXPERIMENTAL)
	MG     = 8,  //a mail group member (EXPERIMENTAL)
	MR     = 9,  //a mail rename domain name (EXPERIMENTAL)
	NUL    = 0,  //a null RR (EXPERIMENTAL)
	WKS    = 11, //a well known service description
	PTR    = 12, //a domain name pointer
	HINFO  = 13, //host information
	MINFO  = 14, //mailbox or mail list information
	MX     = 15, //mail exchange
	TXT    = 16  //text strings
	
} TYPE_TYPE;

typedef struct DNS_MESSAGE_HEADER
{
	char ID[2];
	/* This below is a 16 bit section, should be interpreted as one 16 bit value. */
	uint16_t QR : 1;
	uint16_t Opcode : 4;
	uint16_t AA : 1;
	uint16_t TC : 1;
	uint16_t RD : 1;//
	uint16_t RA : 1;
	uint16_t Z  : 3;
	uint16_t RCODE : 4;
	/* End of this section. */
	uint16_t QDCOUNT;
	uint16_t ANCOUNT;
	uint16_t NSCOUNT;
	uint16_t ARCOUNT;
	
	
} DNS_MESSAGE_HEADER;

typedef struct DNS_MESSAGE_QUESTION
{
	char *QNAME;
	uint16_t QTYPE  ;
	uint16_t QCLASS ;
	
} DNS_MESSAGE_QUESTION;

typedef struct DNS_MESSAGE_RESOURCE 
{
	/* Answer, Authority and additional have same format.*/
	char *NAME;
	uint16_t TYPE;
	uint16_t CLASS;
	uint32_t TTL;
	uint16_t RDLENGTH;
	char *RDATA;
	
} DNS_MESSAGE_RESOURCE;

typedef struct DNS_MESSAGE
{	
	struct DNS_MESSAGE_HEADER 	Header;
	//sections below are arrays
	struct DNS_MESSAGE_QUESTION *Question;   
	struct DNS_MESSAGE_RESOURCE *Answer;
	struct DNS_MESSAGE_RESOURCE *Authority;
	struct DNS_MESSAGE_RESOURCE *Additional;
	
} DNS_MESSAGE;


typedef enum RESULT_TYPE
{
	RESULT_SUCCESS = 0,
	RESULT_ERROR = 1
	
} RESULT_TYPE;

typedef struct MESSAGE_PARSE_RESULT
{
	int type;
	int parsed_bytes;
	struct ERROR_INFO *error;
	struct DNS_MESSAGE *message;
	
} MESSAGE_PARSE_RESULT;

typedef struct PARSING_INFO
{
	void **insert;
	void *insert_fixed;
	char *buffer;
	int buffer_length;
	int start_offset;
	unsigned short count;
	
	
} PARSING_INFO;

/*
* Functions
*/

/*
*  Conversion from 'DNS_MESSAGE' struct to 'char array'.
*/
int build_dns_message(DNS_MESSAGE *dns_message, char **output);
int build_dns_message_header(DNS_MESSAGE *msg, char **output);
int build_dns_message_question(DNS_MESSAGE *msg, char **output);
int build_dns_message_resource(DNS_MESSAGE_RESOURCE *resource, int count, char **output);
int merge_arrays(char *arrays[], int lengths[], int arrays_count, char **output);

/*
* Convert from raw data to DNS_MESSAGE
*/
MESSAGE_PARSE_RESULT* get_message(char *data, int buffer_length);
int insert_dns_message_header  (ERROR_INFO *error, PARSING_INFO *info);
int insert_dns_message_question(ERROR_INFO *error, PARSING_INFO *info);
int insert_dns_message_resource(ERROR_INFO *result, PARSING_INFO *info);
int update_parsing_info(PARSING_INFO *info, void *insert, int start_offset, int count);


int extract_message_name(char *buffer,
 int start_offset, 
 int buffer_length,
 char **output,
 int *message_length);
 
int destroy_dns_message(DNS_MESSAGE *msg);
int get_resource_fields(char *start_ptr, int *type, int *class, int *ttl, int *rdlen);

int packet_parsing_error(char *format, ...);
int packet_build_error(char *format, ...);

#endif