#include "../include/all.h"
#include "clientlib.c"

PLAYER_STATUS my_status = STARTPAGE;
int temi_todo = NUM_THEME;

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

    if(connect(sock, (struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1) {
        printf("Errore nella connessione al server \n");
        exit(-1);
    }


    printMenu();
    while (true)
    {
        
        switch (my_status)
        {
            case STARTPAGE:
                my_status = sceltaMenu();
                break;

            case SCELTANOME:
                my_status = richiediNome(sock);

                recvTemi(sock);
                break;

            case ATTIVO:
            
                int i = 0;
                
                
                do
                {
                    printTemi();
                    while (!scegliTema(sock, &i));
                    my_status = quiz(sock,i);
                    if(my_status == ENDQUIZ)
                        break;
                    
                }while(temi_todo > 0);

                break;

            case ENDQUIZ:
                close(sock);
                return 0;
                break;

        }
    }
    
    return 0;
}
