CC=gcc
AR=ar
LIB=libmsocket.a
LINKER_OPTS=-Wl,-rpath=.
FLAGS=-Wall $(LINKER_OPTS) -pthread
LIBMYSOCK_FLAGS=-rcs
MYSOCK_FLAGS=-Wall -c -lpthread -fPIC

build:
	make server
	make client
.PHONY: build

server: server.c $(LIB)
	$(CC) $(FLAGS) $^ -o $@

client: client.c $(LIB)
	$(CC) $(FLAGS) $^ -o $@

$(LIB): mysocket.o
	$(AR) $(LIBMYSOCK_FLAGS) $(LIB) $^

mysocket.o: mysocket.c mysocket.h
	$(CC) $(MYSOCK_FLAGS) $^

clean: 
	rm *.a *.o
