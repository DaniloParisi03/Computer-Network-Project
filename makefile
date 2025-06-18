# Directory di output
OBJDIR = bin
# File sorgente
SRCSERVER = src/server.c
SRCCLIENT = src/client.c

# File oggetto
OBJSERVER = bin/server.o
OBJCLIENT= bin/client.o


# Compila tutto (default)
make: server client

# Regola per compilare il file oggetto del server
$(OBJSERVER): $(SRCSERVER)
	gcc -c $(SRCSERVER) -o $(OBJSERVER)

server: $(OBJSERVER)
	gcc $(OBJSERVER) -o server

$(OBJCLIENT): $(SRCCLIENT)
	gcc -c $(SRCCLIENT) -o $(OBJCLIENT)

client: $(OBJCLIENT)
	gcc $(OBJCLIENT) -o client



# Pulisce i file generati
cls:
	rm -f $(OBJSERVER) server $(OBJCLIENT) client
