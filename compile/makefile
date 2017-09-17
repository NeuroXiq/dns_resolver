CC = gcc
RELASE_OBJ = debug.o\
	winsock_handler.o\
	program_error.o\
	dns_lib.o\
	dns_msg_error.o\
	dns_resolver.o\
	args.o
	
LIBS = -lws2_32
HDIR = ../headers/
SDIR = ../source/

all: COMPILE CLEAN
COMPILE: $(RELASE_OBJ)
	$(CC) $(RELASE_OBJ) -o dns -lws2_32
CLEAN:
	del *.o	
dns_resolver.o:
	$(CC) -c $(SDIR)dns_resolver.c -o dns_resolver.o
debug.o:
	$(CC) -c $(SDIR)debug.c -o debug.o
winsock_handler.o:
	$(CC) -c $(SDIR)winsock_handler.c -o winsock_handler.o
program_error.o:
	$(CC) -c $(SDIR)program_error.c -o program_error.o
dns_lib.o:
	$(CC) $(SDIR)dns_lib.c -c -o dns_lib.o
dns_msg_error.o:
	$(CC) $(SDIR)dns_msg_error.c -c -o dns_msg_error.o
args.o:
	$(CC) $(SDIR)args.c -c -o args.o