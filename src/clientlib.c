#include "../include/clientlib.h"

void printMenu()
{
    printf("Trivia Quiz\n++++++++++++++++++++++++++++++ \nMenù:\n1 - Comincia una sessione di Trivia\n2 - Esci\n++++++++++++++++++++++++++++++\n");
}
PLAYER_STATUS sceltaMenu()
{
    int scelta = -1;
    
    printf("La tua scelta:\n");
    scanf("%d", &scelta); 
        
    switch (scelta)
    {
        case 1:
            return SCELTANOME;
            break;
            
        case 2:
            return ENDQUIZ;
            break;

        default:
        return ENDQUIZ;
            break;
    }
}

bool checkCommand(char* buff){
    return (strcmp(buff, SHOW_SCORE) == 0 ? true : false);
}
void initTema(const int id, const char* nome, const int len_nome ){
    strncpy(temi[id].nome, nome, len_nome);
    temi[id].attivo = false;
    temi[id].punteggio = 0;
}

bool sendInput(const int sock, char* buffer_in, int SIZE_BUFFER){
    char buffer_risultato[NAME_RESULT];
    size_t len = 0;
    do
    {
    
        memset(buffer_in, 0, SIZE_BUFFER);
        memset(buffer_risultato, 0, NAME_RESULT);

        do{
            fgets(buffer_in, NAME_MAX, stdin);
            len = strlen(buffer_in);

            if (len > 0 && buffer_in[len - 1] == '\n') {
                buffer_in[len - 1] = '\0';
            }

        }while(strlen(buffer_in)==0);

        buffer_in[strcspn(buffer_in, "\n")] = 0;
        // Ricevi la risposta
        
        send(sock, buffer_in, strlen(buffer_in), 0);

        if(checkCommand(buffer_in)){
            //la forma dell'output è: "- nome punteggio" 
            // while(recvRiga(sock, SCOREBUFF_SIZE));
            return false;
        }
        
        int byte_ricevuti = recv(sock, buffer_risultato, strlen(ACK_NAME),0);
        buffer_risultato[byte_ricevuti] = '\0';

        if (byte_ricevuti <= 0) {
            printf("Connessione chiusa dal server\n");
            return false;
        }
        printf("\n%s\n", buffer_risultato);

        if(strcmp(buffer_risultato, ACK_NAME) == 0)
            return true;
        else
            printf("\n[ERRORE]: %s\n", buffer_risultato);

    } while (true);
}

PLAYER_STATUS richiediNome(const int sock){
    
    char buffer_nome[NAME_MAX];
    char buffer_risultato[NAME_RESULT];
    size_t len = 0;
    printf("\nTrivia Quiz\n+++++++++++++++\n");
    
    printf("Scegli il nuo nickname (deve essere univoco)\n");

    if(!sendInput(sock,buffer_nome,NAME_MAX))
        return ENDQUIZ;

    return ATTIVO;
}

void recvAll(const int sock){
    char nome[NAME_MAX];
    char riga[NAME_MAX + 20];
    for(int i = 0; i < NUM_THEME; i++)
    {
        ssize_t ret = recv(sock, (void*)nome, NAME_MAX, 0);
        nome[ret] = '\0';
        
        initTema(i, nome,ret);

        snprintf(riga, sizeof(riga), "\n%d - %s ", i+1, nome);
        strncat(tabellone_temi, riga, BUFFER_SIZE - strlen(tabellone_temi) - 1);    
     }
}

void recvTema(const int sock){

    char nome[NAME_MAX];
    ssize_t ret = recv(sock, (void*)nome, NAME_MAX, 0);
    nome[ret] = '\0';
    
    printf("Quiz - %s\n", nome);
    printf("++++++++++++++++++++++++++++++\n");

    send(sock, ACK_NAME, strlen(ACK_NAME),MSG_NOSIGNAL);
    
}
bool recvRiga(const int sock, const int LEN_BUFFER){
    
    char buffer_recv[LEN_BUFFER];
    memset(buffer_recv, 0, LEN_BUFFER);
    
    ssize_t ret = recv(sock, buffer_recv, LEN_BUFFER, 0);
    
    if (ret <= 0){
        perror("recv non funzionante");
        exit(EXIT_FAILURE);
    }
    
    buffer_recv[ret] = '\0';  
    printf("%s\n", buffer_recv);

    if(strcmp(buffer_recv,EOQ) == 0)
        return false;
    
    return true;


}
int sendTemaScelto(const int sock){

    uint32_t tema_in, tema_out;
    ssize_t bytes_inviati;

    printf("Quiz disponibili\n");
    printf("++++++++++++++++++++++++++++++\n");

    printf("%s\n", tabellone_temi);
    printf("++++++++++++++++++++++++++++++\n");

    printf("La tua scelta:\n");
    
    do{
        scanf("%d", &tema_in);
    }while (tema_in <= 0 || tema_in > NUM_THEME);

    tema_out = htonl(tema_in);

    bytes_inviati = send(sock, &tema_out, sizeof(tema_in), 0);
    if (bytes_inviati != sizeof(tema_out)) {
        perror("Errore nell'invio del tema");
        exit(EXIT_FAILURE);
    }
    return tema_in;
}

PLAYER_STATUS quiz(const int sock){
    
    char riposta[A_MAX];

    recvAll(sock);
    u_int32_t tema_index = 0;
    do{
        tema_index = sendTemaScelto(sock);
        recvTema(sock);
        temi[tema_index].attivo = true;
        
        while(recvRiga(sock, Q_MAX)){
            memset(riposta,0,A_MAX);

            if(sendInput(sock, riposta, A_MAX)){
                temi[tema_index].punteggio++;
            }
        }

        temi[tema_index].attivo = false;
    }while(true);

    return ENDQUIZ;
}
