#include "../include/clientlib.h"
#include "clientlib.h"

void printMenu()
{
    printf("Trivia Quiz\n+++++++++++++++ \nMenù:\n1 - Comincia una sessione di Trivia\n2 - Esci\n+++++++++++++++\n");
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
    do
    {
        printf("Enter message: ");
        fgets(buffer_nome, NAME_MAX, stdin);

        buffer_nome[strcspn(buffer_nome, "\n")] = 0;

        // Invia il Nome come messaggio
        send(sock, buffer_nome, NAME_MAX, 0);

        // Ricevi la risposta
        int byte_ricevuti = recv(sock, buffer_risultato, NAME_RESULT,0);

        if (byte_ricevuti <= 0) {
            printf("Connessione chiusa dal server\n");
            return ENDQUIZ;
        }

        buffer_nome[byte_ricevuti] = '\0';
        printf("%s", buffer_risultato);

        if (strcmp(buffer_risultato, "Risposta corretta") == 0) 
            return ATTIVO;
        
    } while (true);
    

    return ATTIVO;
}
