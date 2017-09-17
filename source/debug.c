#ifndef __DEBUG_LIB_
#define __DEBUG_LIB_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/debug.h"

#define DEBUG_SET 1
#define ABS(x) (x >= 0 ? (x) : (-x))

#define debug_print_ext(fmt, msg)do					\
													\
{													\
	if(DEBUG_SET) 									\
	{												\
		fprintf(stderr, "%s:%d:%s "msg, __FILE__ 	\
			__LINE__, __func__);  		\
	}  												\
}													\
while(0) 											\


#define debug_print_ext_COPY(fmt, ...) 					\
do 													\
{													\
	if(DEBUG_SET) 									\
	{												\
		fprintf(stderr, "%s:%d:%s "fmt, __FILE__ 	\
			__LINE__, __func__, _VA_ARGS__);  		\
	}  												\
}													\
while(0); 					
	
#define debug_print(fmt, ...) \
do 							  \
{  							  \
	if(DEBUG_SET)  \
	{			   \
		fprintf(stderr, fmt, _VA_ARGS__); \
	}	\
	\
}\
while(0) 



int DEBUG_hex_dump(char *start, int len, int row_size)
{
	
	int len_length = DEBUG_chars_count_in_int(len);
	puts("+-------------------------------------------------------------");
	printf("Offset");
	for(int i = 0; i < len_length - 4; i++)
	{ printf(" "); }
	printf("  | ");
	for(int i = 0 ; i < row_size; i++) // printing top offsets
	{
		printf("%.2X ",(unsigned char)i);
	}
	printf("\n\n");
	for(int i = 0; i < len; i += row_size)
	{
		//printing left column offsets
		printf("%d", i);
		for(int j = 0; j < len_length - DEBUG_chars_count_in_int(i) + 4 ; j++)
		{
			printf(" ");
		}
		printf(" |");
		//printing hex values
		int writed = 0;
		for(int j = 0; j < row_size; j++)
		{
			if(j + i >= len)
			{ break; }
			printf(" %.2X", (unsigned char)start[i+ j]);
			writed++;
		}
		//filling empty spaces (probably this is a last row and len % row_size != 0)
		if(writed != row_size)
		{
			for(int q = 0 ; q < (row_size - writed); q++)
			{
				printf("   ");
			}
		}
		printf(" |");
		//printing chars
		for(int j = 0; j < row_size; j++)
		{
			if(j + i >= len)
			{ break; }
			if((start[i+j] == '\n') ||
			   (start[i+j] == '\r') )
			{
				printf("  ");
			}
			else 
			{
				printf(" %c", (unsigned char)start[i + j]);
			}
		}
		printf("\n");
	}
}



int DEBUG_chars_count_in_int(int i)
{
	int count = 1;
	while(ABS(i) > 9)
	{
		i /= 10;
		count++;
	}
	return count;
}

		
#endif