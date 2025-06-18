CC = gcc
CFLAGS = -Wall -g
OBJDIR = bin

CLIENT_SRC = src/client.c
CLIENT_OBJ = $(OBJDIR)/client.o 
CLIENT_BIN = client

SERVER_SRC = src/server.c
SERVER_OBJ = $(OBJDIR)/server.o 
SERVER_BIN = server

# controlla se esiste la cartella bin
$(OBJDIR):
	@mkdir -p $(OBJDIR)

# compilazione client e server
$(CLIENT_OBJ): $(CLIENT_SRC) | $(OBJDIR)
	$(CC) $(CFLAGS) -c $(CLIENT_SRC) -o $(CLIENT_OBJ)

$(SERVER_OBJ): $(SERVER_SRC) | $(OBJDIR)
	$(CC) $(CFLAGS) -c $(SERVER_SRC) -o $(SERVER_OBJ)

$(CLIENT_BIN): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) $(CLIENT_OBJ) -o $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_OBJ)
	$(CC) $(CFLAGS) $(SERVER_OBJ) -o $(SERVER_BIN)

# di default compila sia client che server
all: $(CLIENT_BIN) $(SERVER_BIN)

# pulizia dei file generati
clean:
	rm -rf $(OBJDIR) $(CLIENT_BIN) $(SERVER_BIN)
