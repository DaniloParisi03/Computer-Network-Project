# Directory di output
OBJDIR = bin
# File sorgente
SRCSERVER = src/server.c
SRCCLIENT = src/client.c


#liberie
SRCLIBCLIENT= src/clientlib.c
SRCLIBSERVER= src/serverlib.c

# File oggetto
OBJSERVER = $(OBJDIR)/server.o
OBJCLIENT= $(OBJDIR)/client.o

OBJLIBSERVER = $(OBJDIR)/serverlib.o
OBJLIBCLIENT = $(OBJDIR)/clienlib.o

# Compila tutto (default)
all: server client

# Regola per compilare il file oggetto del server

$(OBJSERVER): $(SRCSERVER)
	gcc -c $< -o $@
$(OBJLIBSERVER): $(SRCLIBSERVER)	
	gcc -c $< -o $@

server: $(OBJSERVER) $(OBJLIBSERVER)
	gcc $(OBJSERVER) $(OBJLIBSERVER) -o server


$(OBJCLIENT): $(SRCCLIENT)
	gcc -c $(SRCCLIENT) -o $(OBJCLIENT)

$(OBJLIBCLIENT): $(SRCCLIENT)	
	gcc -c $(SRCLIBCLIENT) -o $(OBJLIBCLIENT)

client: $(OBJCLIENT) $(OBJLIBCLIENT)
	gcc $(OBJCLIENT) $(LIBCLIENT) -o client



# Pulisce i file generati
cls:
	rm -f $(OBJDIR)/*.o server client
