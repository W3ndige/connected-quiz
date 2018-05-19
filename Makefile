SERVER_OBJ = src/server/server.c src/server/files.c src/server/helper.c src/server/networking.c src/server/game.c src/server/processes.c
CLIENT_OBJ = src/client/client.c src/client/networking.c src/client/printing.c src/client/interaction.c
CC = gcc
COMPILER_FLAGS = -Wall -Wextra
CLIENT_LIBS_FLAGS = -lSDL -lSDL_ttf
SERVER_OBJ_NAME = server
CLIENT_OBJ_NAME = client

all: $(SERVER_OBJ) $(CLIENT_OBJ)
	$(CC) $(SERVER_OBJ) $(COMPILER_FLAGS) -o $(SERVER_OBJ_NAME)
	$(CC) $(CLIENT_OBJ) $(COMPILER_FLAGS) $(CLIENT_LIBS_FLAGS) -o $(CLIENT_OBJ_NAME)

server: $(SERVER_OBJ)
	$(CC) $(SERVER_OBJ) $(COMPILER_FLAGS) -o $(SERVER_OBJ_NAME)

client: $(CLIENT_OBJ)
	$(CC) $(CLIENT_OBJ) $(COMPILER_FLAGS) $(CLIENT_LIBS_FLAGS) -o $(CLIENT_OBJ_NAME)

clean:
	rm $(SERVER_OBJ_NAME) $(CLIENT_OBJ_NAME)
