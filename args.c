#include <string.h>
#include "args.h"

int arg_contains(ARGS_INFO* args, char *arg)
{
	if(arg == NULL)
		return -1;

	if(args->count > 0)
	{
		for(int i = 0 ; i < args->count; i++)
		{
			if(strcmp(args->arguments[i], arg) == 0)
			{
				return i;
			}
		}
		return -3;
	}
	return -2;
}

char* arg_get_value(ARGS_INFO* args, char *arg)
{
	int index = arg_contains(args, arg);
	if(index < 0)
		return NULL;
	if(args->count - 1 > index)
	{
		return args->arguments[index + 1];
	}
	else return NULL;
}