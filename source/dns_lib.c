#ifndef _DNS_LIB_C__
#define _DNS_LIB_C__

#include <stdlib.h>
#include <string.h>
#include "dns_lib.h"
#include "dns_msg_error.h"


#define SWAP_WORD_BYTES(x) x = ((x << 8) | (x >> 8))
#define SWAP_DWORD_BYTES(x) x = \
((x << 24 ) | (x >> 24)) | (((x & 0x00ff0000) >> 8) | ((x & 0x0000ff00) << 8))
#define SET_ERROR(x, y) (x = ((y >= 0) ? (x) : (1)))

/*
*
* If everyting goes well, returns length of output array 
* otherwise returs negative value. 
*
*/
int build_dns_message(DNS_MESSAGE *dns_message, char **output)
{	
	if(dns_message == NULL)
	{
		
		return -1;
	}
	
	int packet_length = 0;
	
	char *header     = NULL;
	char *question   = NULL;
	char *answer     = NULL;
	char *authority  = NULL;
	char *additional = NULL;
	
	int header_len     = build_dns_message_header(dns_message, &header);
	int question_len   = build_dns_message_question(dns_message, &question);
	int answer_len     = build_dns_message_resource(dns_message->Answer,dns_message->Header.ANCOUNT, &answer);
	int authority_len  = build_dns_message_resource(dns_message->Authority,dns_message->Header.NSCOUNT, &authority);
	int additional_len = build_dns_message_resource(dns_message->Additional,dns_message->Header.ARCOUNT, &additional);
	
	int sections_arrays_count = 5;
	
	char *sections_arrays[] = 
	{
		header, 
		question,
		answer,
		authority,
		additional
	};
	
	int lengths[] = 
	{
		header_len,
		question_len,
		answer_len,
		authority_len,
		additional_len
	};
	
	char *builded_packet;
	int full_size = merge_arrays(
	sections_arrays,
	lengths,
	sections_arrays_count,
	&builded_packet);
	
	//section arrays are used only here, we must deallocate memory.
	for(int i = 0; i < sections_arrays_count; i++)
	{
		free(sections_arrays[i]);
	}
	
	*output = builded_packet;
	return full_size;
}

int build_dns_message_header(DNS_MESSAGE *dns_message, char **output_header)
{
	int header_len = 12;
	*output_header = calloc(12, 1);
	
	char *header = *output_header;
	header[0] = dns_message->Header.ID[0];
	header[1] = dns_message->Header.ID[1];
	header[2] = 
	(char)(dns_message->Header.QR << 7) 	| 
	(char)(dns_message->Header.Opcode << 6) |
	(char)(dns_message->Header.AA << 2) 	| 
	(char)(dns_message->Header.TC << 1) 	| 
	(char)(dns_message->Header.RD);
	header[3] = 
	(char)(dns_message->Header.RA << 7) | 
	(char)(dns_message->Header.Z << 6)  | 
	(char)(dns_message->Header.RCODE); 
	
	/*4 - 11*/
	uint16_t qdcount = dns_message->Header.QDCOUNT;
	uint16_t ancount = dns_message->Header.ANCOUNT;
	uint16_t nscount = dns_message->Header.NSCOUNT;
	uint16_t arcount = dns_message->Header.ARCOUNT;
	
	//little-endian
	qdcount = SWAP_WORD_BYTES(qdcount);
	ancount = SWAP_WORD_BYTES(ancount);
	nscount = SWAP_WORD_BYTES(nscount);
	arcount = SWAP_WORD_BYTES(arcount);
	
	
	*(uint16_t*)(&header[4 ])  = qdcount;
	*(uint16_t*)(&header[6 ])  = ancount;
	*(uint16_t*)(&header[8 ])  = nscount;
	*(uint16_t*)(&header[10])  = arcount;
	
	return header_len;
}

int build_dns_message_question(DNS_MESSAGE *dns_message, char **output)
{
	int question_count = dns_message->Header.QDCOUNT;
	if(question_count == 0)
		return 0;
	
	int chunk_length = 0;
	int qname_length = 0;
	int full_length = 0;
	uint16_t qtype, qclass;
	char *raw_data = NULL;
	
	for(int i = 0; i < question_count; i++)
	{
		qname_length = strlen(dns_message->Question[i].QNAME) + 1; //zero-byte is also included. 
		
		//4 for QTYPE and QCLASS int16.		
		chunk_length += qname_length + 4;
		full_length += chunk_length;
		
		raw_data = realloc(raw_data, full_length);
		
		memcpy(
		raw_data + full_length - chunk_length, 
		dns_message->Question[i].QNAME,
		qname_length);
		
		qtype =  dns_message->Question[i].QTYPE;
		qclass = dns_message->Question[i].QCLASS;
				
		//little-endian
		qtype = SWAP_WORD_BYTES(qtype);
		qclass = SWAP_WORD_BYTES(qclass);
		
		
		*(uint16_t*)(raw_data + full_length - 4) = qtype;
		*(uint16_t*)(raw_data + full_length - 2) = qclass;
		
		chunk_length = 0; // new chunk
		qname_length = 0;
	}
	*output = raw_data;
	return full_length;
}

int build_dns_message_resource(DNS_MESSAGE_RESOURCE *resource, int count, char **output)
{
	if(count == 0)
	{
		return 0;
	}
	
	char *error;
	int error_appear = 0;
	if(resource->NAME == NULL)
	{
		error = "RNAME parameter in resource is NULL";
		error_appear = 1;
	}
	else if((resource->RDATA == NULL ) && (resource->RDLENGTH != 0))
	{
		error = "RDATA parameter is equal null";
		error_appear = 1;
	}
	else error_appear = 0;
	
	if(error_appear)
	{
		//packet_build_error("%s %s %s %d \n", "RESOURCE BUILD\n", error, "resource count: ", count);
		return -1;	
	}
	
	char *full_output = NULL;
	int full_length = 0;
	int const_addition = 10; // type/class/ttl/rdlength bytes
	int last_length = 0;
	
	for(int i = 0; i < count; i ++)
	{ 
		int qname_length = strlen(resource->NAME) + 1; // zero-byte must be included
		last_length = qname_length + const_addition + resource->RDLENGTH;
		full_length += last_length;
		full_output = realloc(full_output,full_length);
		
		if(full_output == NULL)
		{
			//packet_build_error("%s ","RESOURCE BUILD\nALLOCATION MEMORY FAIL");
			return -1;
		}
		
		//NAME
		memcpy(full_output + full_length - last_length, resource->NAME, qname_length);
		
		int insert_index = full_length - last_length + qname_length;
		//TYPE
		full_output[insert_index + 0] = (char)(resource->TYPE >> 8);
		full_output[insert_index + 1] = (char)(resource->TYPE & 0xff);
		//CLASS
		full_output[insert_index + 2] = (char)(resource->CLASS >> 8);
		full_output[insert_index + 3] = (char)(resource->CLASS & 0xff);
		//TTL
		full_output[insert_index + 4] = (char)((resource->TTL  >> 24) & 0xff);
		full_output[insert_index + 5] = (char)((resource->TTL  >> 16) & 0xff);
		full_output[insert_index + 6] = (char)((resource->TTL  >>  8) & 0xff);
		full_output[insert_index + 7] = (char)((resource->TTL  >>  0) & 0xff);
		//RDLENGTH
		full_output[insert_index + 8] = (char)(resource->RDLENGTH >> 8);
		full_output[insert_index + 9] = (char)(resource->RDLENGTH & 0xff);
		
		//RDATA
		memcpy(full_output + insert_index + 10,resource->RDATA, resource->RDLENGTH);
	}
	
	*output = full_output;
	
	return full_length;
}

int merge_arrays(char *arrays[], int lengths[], int arrays_count, char **output)
{
	int full_len = 0;
	for(int i = 0; i < arrays_count; i++)
	{
		full_len += lengths[i];
	}
	
	*output = malloc(full_len);
	int next_index = 0;
	
	for(int i = 0; i < arrays_count; i++)
	{
		if(arrays[i] != NULL && lengths[i] > 0)
		{
			memcpy(*output + next_index, arrays[i], lengths[i]);
			next_index += lengths[i];
		}
	}
	return full_len;
}

MESSAGE_PARSE_RESULT* get_message(char *data, int buffer_length)
{	
	int error = 0;
	
	MESSAGE_PARSE_RESULT *result = calloc(sizeof(MESSAGE_PARSE_RESULT),1);
	result->message = calloc(sizeof(DNS_MESSAGE), 1);
	result->error = calloc(sizeof(ERROR_INFO),1);
	DNS_MESSAGE *msg = result->message;
	
	PARSING_INFO *info = calloc(sizeof(PARSING_INFO), 1);
	info->insert_fixed = &msg->Header;
	info->buffer = data;
	info->buffer_length = buffer_length;
	info->start_offset = 0;
	info->count = 0;
	
	int question_len = 0, header_len = 0, answer_len = 0, authority_len = 0, additional_len = 0;
	header_len = insert_dns_message_header(result->error, info);
	
	/*
	* If some errors occurs OR if 'error' contains positive value, set 'error' to positive value.
	* Initial value of 'error' is zero.
	*/
	
	SET_ERROR(error, header_len);
	//printf("header:%d\n", header_len);
	
	if(!error)
	{
		update_parsing_info(info, &msg->Question, header_len, msg->Header.QDCOUNT);
		question_len = insert_dns_message_question(result->error, info);
		
		SET_ERROR(error, question_len); 
		//printf("question:%d\n", question_len);
		
	}
	
	if(!error)
	{
		update_parsing_info(info, &msg->Answer, question_len, msg->Header.ANCOUNT);
		answer_len = insert_dns_message_resource(result->error, info);
		SET_ERROR(error, answer_len);
		//printf("answer:%d\n", answer_len);
	}
	
	if(!error)
	{
		update_parsing_info(info,&msg->Authority, answer_len, msg->Header.NSCOUNT);
		authority_len = insert_dns_message_resource(result->error, info);
		SET_ERROR(error, authority_len);
		//printf("authority:%d\n", authority_len);
	}
	
	if(!error)
	{
		update_parsing_info(info, &msg->Additional, authority_len, msg->Header.ARCOUNT);
		additional_len = insert_dns_message_resource(result->error, info);
		SET_ERROR(error, additional_len);
		//printf("additional:%d\n", additional_len);
	}
	
	if(error)
	{
		result->type = RESULT_ERROR;
	}
	else
	{
		result->type = RESULT_SUCCESS;
	}
	
	return result;
}

int update_parsing_info(PARSING_INFO *info, void *insert, int add_offset, int count)
{
	info->insert = insert;
	info->start_offset += add_offset;
	info->count = count;
	return 1;
}

int insert_dns_message_header(ERROR_INFO *error, PARSING_INFO *info)
{
	DNS_MESSAGE_HEADER *header = (DNS_MESSAGE_HEADER*)info->insert_fixed;
	
	char *buffer = info->buffer;
	int buffer_length = info->buffer_length;
	// minimum length of header is 12 bytes.
	if(buffer_length < 12)
	{
		error_header(error, "Cannot parse header. Buffer length must have at least 12 bytes");	
		return -1;
	}
	
	header->ID[0] = buffer[0];
	header->ID[1] = buffer[1]; 
	
	
	// buffer[2] and buffer[3]: 
	uint16_t bit_section = *(uint16_t*)(&buffer[2]);
	bit_section = SWAP_WORD_BYTES(bit_section); // now data is in big endian.
	
	header->QR = (bit_section & QR_MASK) >> QR_MASK_SHIFT;
	header->Opcode = (bit_section & OPCODE_MASK) >> OPCODE_MASK_SHIFT;
	header->AA = (bit_section & AA_MASK) >> AA_MASK_SHIFT;
	header->TC = (bit_section & TC_MASK) >> TC_MASK_SHIFT;
	header->RD = (bit_section & RD_MASK) >> RD_MASK_SHIFT;
	header->AA = (bit_section & RA_MASK) >> RA_MASK_SHIFT;
	header->AA = (bit_section & Z_MASK) >> Z_MASK_SHIFT;
	header->AA = (bit_section & RCODE_MASK) >> RCODE_MASK_SHIFT;
	
	uint16_t qdcount = *(uint16_t*)(&buffer[4]);
	uint16_t ancount = *(uint16_t*)(&buffer[6]);
	uint16_t nscount = *(uint16_t*)(&buffer[8]);
	uint16_t arcount = *(uint16_t*)(&buffer[10]);
	
	qdcount = SWAP_WORD_BYTES(qdcount);
	ancount = SWAP_WORD_BYTES(ancount);
	nscount = SWAP_WORD_BYTES(nscount);
	arcount = SWAP_WORD_BYTES(arcount);
	
	header->QDCOUNT = qdcount;
	header->ANCOUNT = ancount;
	header->NSCOUNT = nscount;
	header->ARCOUNT = arcount;
	
	return 12; // header have fixed size
}

int insert_dns_message_question(ERROR_INFO *error, PARSING_INFO *info)
{	
	DNS_MESSAGE_QUESTION *question;
	int buffer_length = info->buffer_length;
	char *buffer = info->buffer;
	
	if(info->count > 0)
	{
		*info->insert = calloc(sizeof(DNS_MESSAGE_QUESTION), info->count);
		question = (DNS_MESSAGE_QUESTION*)(*info->insert);
	}
	else return 0;
	
	int total_parsed_bytes = 0;
	int offset = info->start_offset;
		
	for(int i = 0; i < info->count; i++)
	{
		char *message_name;
		int chunk_string_length;
		
		int readed_qname = extract_message_name(
		buffer,
		offset,
		buffer_length,
		&message_name,
		&chunk_string_length); //decompressed qname length
		
		if(chunk_string_length < 0)
		{
			error_question_qname_format(error, offset);
			return -1;
		}
		else if(chunk_string_length > 255)
		{
			error_question_qname_size(error, offset, chunk_string_length);
			return -1;
		}
		uint16_t qtype = *(uint16_t*)(&buffer[offset + readed_qname]);
		uint16_t qclass = *(uint16_t*)(&buffer[offset + readed_qname + 2]);
		
		SWAP_WORD_BYTES(qtype); // in raw data values are in big endian
		SWAP_WORD_BYTES(qclass);
		
		question[i].QNAME = message_name;
		question[i].QTYPE = qtype;
		question[i].QCLASS = qclass;
		offset += readed_qname + 4; // 4 -> qtype + qclass
		
		total_parsed_bytes += readed_qname + 4;
	}
	return total_parsed_bytes;
}

int insert_dns_message_resource(ERROR_INFO *error, PARSING_INFO *info)
{
	DNS_MESSAGE_RESOURCE *res;
	char *buffer =       info->buffer;
	int start_offset =   info->start_offset;
	int buffer_length =  info->buffer_length;
	int resource_count = info->count;
	
	*info->insert = calloc(sizeof(DNS_MESSAGE_RESOURCE), resource_count);
	res = (DNS_MESSAGE_RESOURCE*)(*info->insert);
	
	
	if(resource_count < 0)
		return -1;
	if(resource_count == 0)
		return 0;
	
	int next_offset = start_offset;
	int all_readed_bytes = 0;
	
	for(int i = 0; i < resource_count; i++)
	{
		int decompressed_name_length = -1234;
		char *decompressed_name;
		
		int name_section_length = extract_message_name(
		buffer, 
		next_offset,
		buffer_length,
		&decompressed_name,
		&decompressed_name_length);
		
		if(name_section_length < 0)
		{
			error_resource_name_format(error, next_offset);
			return -1;
		}
		
		int tct_section = next_offset + name_section_length;
		
		if(tct_section + 10 >= buffer_length) // can read type/class/ttl/rdlength data ???
		{
		  
			error_resource_out_of_buffer(error, "Cannot read type/class/ttl/rdlength.\n",
			tct_section, 10);
			return -1;
		}
		
		int type, class, ttl, rdlength;
		get_resource_fields(next_offset + name_section_length + buffer, &type, &class, &ttl, &rdlength);
		
		int rd_section = tct_section + 10;
		
		if(rdlength + rd_section > buffer_length)
		{			
			error_resource_out_of_buffer(
			error,
			"RDLENGTH countains value bigger than buffer length.\n",
			next_offset,
			rd_section);
			
			return -1;
		}
		
		res[i].NAME = decompressed_name;
		res[i].TYPE = type;
		res[i].CLASS = class;
		res[i].TTL = ttl;
		res[i].RDLENGTH = rdlength;
		res[i].RDATA = malloc(rdlength);
		memcpy(res[i].RDATA, (char*)(next_offset + name_section_length + 10 + buffer), rdlength);
		
		next_offset += name_section_length + 10 + rdlength;
		all_readed_bytes += name_section_length + 10 + rdlength;
	}
	
	return all_readed_bytes;
}


int get_resource_fields(char *start_ptr, int *type, int *class, int *ttl, int *rdlen)
{
	uint16_t type_e =     *(uint16_t*)&start_ptr[0];
	uint16_t class_e =    *(uint16_t*)&start_ptr[2];
	uint32_t ttl_e =      *(uint32_t*)&start_ptr[4];
	uint16_t rdlength_e = *(uint16_t*)&start_ptr[8];
	
	//convert to little endian
	SWAP_WORD_BYTES(type_e);
	SWAP_WORD_BYTES(class_e);
	SWAP_DWORD_BYTES(ttl_e);
	SWAP_WORD_BYTES(rdlength_e);
		
	*type = type_e;
	*class = class_e;
	*ttl = ttl_e;
	*rdlen = rdlength_e;
	
	return 0;
}

int extract_message_name(char *buffer, int start_offset,  int buffer_length, char **output, int *message_length)
{
	
	int max = buffer_length > 255 ? 255 : buffer_length;
	
	int temp_len = max; // max of dns qname is 255 bytes
	char *temp = calloc(temp_len, 1);
	int met_ptr = 0; // msg can be compressed and can contain only 1 pointer
	int uncompressed_size = 0;
	int index = start_offset;
	
	int readed_from_qname_section = 0;
	
	/*
	*  The compression scheme allows a domain name in a message to be
	*   represented as either:
	* 1. a sequence of labels ending in a zero octet
	* 2. a pointer
	* 3. a sequence of labels ending with a pointer
	* [COMMENT]
	* pointer have followind format: |11|OFFSET| <- ushort
	* first 2 bits are set to 1
	* next 14 bits are offset from the beginnig of the buffer (raw data)
	*/
	
	if((start_offset >= buffer_length) || 
	(start_offset < 0)) // out of bounds
		return -1;
	
	while(1)
	{
		if((uncompressed_size > max) || (index >= buffer_length)) // out of bounds
		{
			return -1;
		}
		
		if((buffer[index] & 0xC0) == 0xC0) // this byte is a pointer ?
		{
			//we are readed 2 bytes from QNAME section (pointer length)
			if(0 == met_ptr)
			{readed_from_qname_section += 2;}
			
			met_ptr = 1;
			
			uint16_t msg_ptr = *(uint16_t*)(buffer + index);
			SWAP_WORD_BYTES(msg_ptr); // convert to little endian;
			msg_ptr &= 0x3FFF; // first 2 bits must be removed
			
			//ptr is out of array bounds ? 
			if(msg_ptr >= buffer_length)
			{ 
				return -1; 
			}
			
			//jump where pointer points to and continue process
			index = msg_ptr;
			
			continue;
		}
		
		temp[uncompressed_size] = buffer[index];
		if(buffer[index] == 0)
			break;
		
		index++;
		uncompressed_size++;
		
		if(met_ptr == 0) // we are not jumped into the pointer section and still reading QNAME section.
		{
			readed_from_qname_section++;
		}
	}
	
	uncompressed_size++;
	if(met_ptr == 0) // readed only qname section  ?
	{
		readed_from_qname_section++;
	}
	
	*output = temp;
	*message_length = uncompressed_size;
	
	return readed_from_qname_section;
}

int destroy_dns_message(DNS_MESSAGE *msg)
{
	int sections_free = 0;
	
	if(msg != NULL)
	{
		if(msg->Question != NULL)
		{
			for(int i = 0; i < msg->Header.QDCOUNT; i++)
			{
				if(msg->Question[i].QNAME != NULL)
					free(msg->Question[i].QNAME);	
				sections_free++;
			}
			free(msg->Question);
			sections_free++;
		}
		if(msg->Answer != NULL)
		{
			for(int i = 0; i < msg->Header.ANCOUNT; i++)
			{
				if(msg->Answer[i].NAME != NULL)
					free(msg->Answer[i].NAME);
				sections_free++;
			}
			free(msg->Answer);
			sections_free++;
		}
		if(msg->Authority != NULL)
		{
			for(int i = 0; i < msg->Header.NSCOUNT; i++)
			{
				if(msg->Authority[i].NAME != NULL)
					free(msg->Authority[i].NAME);
				sections_free++;
			}
			free(msg->Authority);
			sections_free++;
		}
		if(msg->Additional != NULL)
		{
			for(int i = 0; i < msg->Header.ARCOUNT; i++)
			{
				if(msg->Additional[i].NAME != NULL)
					free(msg->Additional[i].NAME);
				sections_free++;
			}
			free(msg->Additional);
			sections_free++;
		}
	}
	
	return sections_free;
}

#endif