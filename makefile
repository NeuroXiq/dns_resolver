CC = gcc
MAIN_OBJ = debug.o\
	winsock_handler.o\
	program_error.o\
	dns_lib.o\
	
LIBS = -lws2_32

all: dns_resolver.c debug.o winsock_handler.o program_error.o dns_lib.o dns_msg_error.o args.o
	$(CC) dns_resolver.c debug.o winsock_handler.o program_error.o dns_lib.o dns_msg_error.o args.o -o dns -lws2_32
debug.o: debug.c debug.h
	$(CC) -c debug.c -o debug.o
winsock_handler.o: winsock_handler.c winsock_handler.h 
	$(CC) -c winsock_handler.c -o winsock_handler.o
program_error.o:
	$(CC) -c program_error.c -o program_error.o
dns_lib.o: dns_lib.c dns_lib.h
	$(CC) dns_lib.c -c -o dns_lib.o
dns_msg_error.o: dns_msg_error.c dns_msg_error.h
	$(CC) dns_msg_error.c -c -o dns_msg_error.o
args.o: args.c args.h
	$(CC) args.c -c -o args.o



