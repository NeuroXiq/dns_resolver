#ifndef _ARGS__
#define _ARGS__

typedef struct ARGS_INFO
{
	int count;
	char **arguments;
	
} ARGS_INFO;

int args_contains(ARGS_INFO* args, char *arg);
ARGS_INFO args_get_args(int argc, char *argv[]);
char* arg_get_value(ARGS_INFO* args, char *arg);


#endif