#ifndef _ARGS__
#define _ARGS__

typedef struct ARGS_INFO
{
	int count;
	char **arguments;
	
} ARGS_INFO;

int arg_contains(ARGS_INFO* args, char *arg);
char* arg_get_value(ARGS_INFO* args, char *arg);


#endif