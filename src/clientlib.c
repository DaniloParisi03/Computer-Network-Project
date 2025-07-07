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


PLAYER_STATUS richiediNome(const int sock){
    
    char buffer_nome[NAME_MAX];
    char buffer_risultato[NAME_RESULT];
    
    printf("\nTrivia Quiz\n+++++++++++++++\n");
    
    printf("Scegli il nuo nickname (deve essere univoco)\n");
    do
    {
    
        memset(buffer_nome, 0, NAME_MAX);
        memset(buffer_risultato, 0, NAME_RESULT);

        do{
            fgets(buffer_nome, NAME_MAX, stdin);
        }while(strlen(buffer_nome)<=1);

        buffer_nome[strcspn(buffer_nome, "\n")] = 0;

        // Invia il Nome come messaggio
        send(sock, buffer_nome, strlen(buffer_nome), 0);

        // Ricevi la risposta
        int byte_ricevuti = recv(sock, buffer_risultato, NAME_RESULT, 0);
        
        buffer_risultato[byte_ricevuti] = '\0';

        if (byte_ricevuti <= 0) {
            printf("Connessione chiusa dal server\n");
            return ENDQUIZ;
        }
        if(strcmp(buffer_risultato, ACK_NAME) == 0)
            break;
        else
            printf("\n[ERRORE] Nome già in uso riprovare\n");

    } while (true);

    return ATTIVO;
}

bool recvTemi(const int sock){

    char* str_temi;
    uint32_t size_buffer_temi;
    ssize_t byte_ricevuti = 0;

    byte_ricevuti = recv(sock, &size_buffer_temi,sizeof(uint32_t), 0);
    size_buffer_temi = ntohl(size_buffer_temi);

    str_temi = (char*)malloc(size_buffer_temi);
    
    byte_ricevuti = recv(sock, str_temi, size_buffer_temi, 0);
    
    //carattere di teminazione
    str_temi[byte_ricevuti] = '\0';

    printf("\nQuiz disponibili\n++++++++++++++++++++++++++++++");

    printf("%s\n", str_temi);
    printf("++++++++++++++++++++++++++++++\n");

    free(str_temi);

    return true;
}
void sendTemaScelto(const int sock){

    int tema_in;
    printf("La tua scelta:\n\n");
    
    do{
        scanf("%d", &tema_in);
    }while (tema_in <= 0 || tema_in > NUM_THEME);

    tema_in = htonl(tema_in);
    send(sock, &tema_in, sizeof(uint32_t), 0);
}

void recvNomeTemaScelto(const int sock){
    uint32_t num;
    ssize_t byte_ricevuti = 0;
    char* str_tema;

    byte_ricevuti = recv(sock, &num, sizeof(uint32_t), 0);

    num = ntohl(num);
    printf("\n%d\n", num);
    str_tema = (char*)malloc(sizeof(char)*num+1);

    byte_ricevuti = recv(sock, str_tema, num, 0);
    if (byte_ricevuti <= 0) {
            printf("Connessione chiusa dal server\n");
            return;
        }
    //carattere di teminazione
    str_tema[byte_ricevuti] = '\0';
    printf("Quiz - %s\n",str_tema);
    printf("++++++++++++++++++++++++++++++\n");

}

PLAYER_STATUS quiz(const int sock){
    
    char domanda[Q_MAX];
    char risposta[A_MAX];
    ssize_t byte_ricevuti = 0;

    if(!recvTemi(sock))
        return ENDQUIZ;
    
    // do
    // {
        sendTemaScelto(sock);
        recvNomeTemaScelto(sock);
        // do
        // {
        //     byte_ricevuti = recv(sock, domanda, Q_MAX, 0);
        //     if (byte_ricevuti <= 0) {
        //             printf("Connessione chiusa dal server\n");
        //             return false;
        //         }
        //     //carattere di teminazione
        //     domanda[byte_ricevuti] = '\0';
        //     if(strcmp(domanda,EOQ) == 0)
        //         break;;
        //     printf("%s\n\n", domanda);
        // } while (true);

        // recvTemi(sock);
    // } while (true);
    
    return ENDQUIZ;
}
