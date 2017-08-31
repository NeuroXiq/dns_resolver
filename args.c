#include <string.h>
#include "args.h"

int arg_contains(ARGS_INFO args, char *arg)
{
	
	if(args.count > 0)
	{
		for(int i = 0 ; i < args.count; i++)
		{
			if(strcmp(args.arguments[i], arg) == 0)
			{
				return i;
			}
		}
		return -3;
	}
	else return -2;
}

ARGS_INFO args_get_args(int argc, char *argv[])
{	
	ARGS_INFO args;
	args.count = argc;
	args.arguments = argv;
	
	return args;
}