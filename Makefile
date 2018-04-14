SERVER_OBJ = server.c
CLIENT_OBJ = client.c
CC = gcc
COMPILER_FLAGS = -Wall -Wextra
SERVER_OBJ_NAME = server
CLIENT_OBJ_NAME = client

all: $(SERVER_OBJ) $(CLIENT_OBJ)
	$(CC) $(SERVER_OBJ) $(COMPILER_FLAGS) $(SERVER_LINKER_FLAGS) -o $(SERVER_OBJ_NAME)
	$(CC) $(CLIENT_OBJ) $(COMPILER_FLAGS) -o $(CLIENT_OBJ_NAME)

clean:
	rm $(SERVER_OBJ_NAME) $(CLIENT_OBJ_NAME)
