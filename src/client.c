#include "../include/all.h"
#include "clientlib.c"

PLAYER_STATUS my_status = STARTPAGE;

int main(int argc,const char *argv[]) 
{

    int sock , porta;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    //porta di default
    porta = 3456;
    struct sockaddr_in serv_addr; 
    
    if (argc == 2) {
        porta = atoi(argv[1]);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(porta);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(sock, (struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1) {
        printf("Error...\n");
        exit(-1);
    }

    //char strData[255];
    //recv(sock, strData, sizeof(strData), 0);
    //printf("Message: %s\n", strData);

    printMenu();
    while (1)
    {
        
        switch (my_status)
        {
        case STARTPAGE:
            my_status = sceltaMenu();
            break;

        case SCELTANOME:
            my_status = richiediNome(sock);
            break;

        case ATTIVO:
            my_status = sceltaQuiz(sock);
            break;

        case ENDQUIZ:
            /* code */
            break;
        default:
            break;
        }
    }
    
    return 0;
}
