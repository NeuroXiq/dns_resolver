#ifndef _ARGS__
#define _ARGS__

typedef struct ARGS_INFO
{
	int count;
	char *ignore_prefix;
	char **arguments;
	
} ARGS_INFO;

int args_contains(ARGS_INFO args, char *arg);
ARGS_INFO args_get_args(int argc, char *argv[]);


#endif