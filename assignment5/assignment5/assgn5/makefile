CC=gcc
AR=ar
LIB=libmsocket.a
FLAGS=-Wall
LIBMYSOCK_FLAGS=-rcs
MYSOCK_FLAGS=-Wall -c -fPIC -lpthread



build:
	make server
	make client
.PHONY: build

server: server.o $(LIB)
	$(CC) $(FLAGS) $< -o $@ -L. -lmsocket -lpthread

server.o: server.c
	$(CC) $(FLAGS) -c $< -o $@ 

client: client.c $(LIB)
	$(CC) $(FLAGS) $< -o $@ -L. -lmsocket -lpthread

client.o: client.c
	$(CC) $(FLAGS) -c $< -o $@ 

$(LIB): mysocket.o
	$(AR) $(LIBMYSOCK_FLAGS) $(LIB) $^

mysocket.o: mysocket.c mysocket.h
	$(CC) $(MYSOCK_FLAGS) $^

clean:
	rm *.a *.o *.gch
.PHONY: clean