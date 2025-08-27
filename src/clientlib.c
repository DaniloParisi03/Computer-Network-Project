#include "../include/clientlib.h"
#include "../include/all.h"
int temi_todo;
void printMenu()
{
    system("clear");
    printf("Trivia quiz\n");
    printf("+++++++++++++++++++++++++++++++++++\n");
    printf("Menù:\n");
    printf("1 - Comincia una sessione di Trivia\n");
    printf("2 - Esci\n");
    printf("+++++++++++++++++++++++++++++++++++\n");
    printf("La tua scelta: \n");
}

bool checkCommand(const char* buff){
    return (strcmp(buff, SHOW_SCORE) == 0 || strcmp(buff, "show score") == 0) ? true : false;
}
void initTema(const int id, const char* nome, const int len_nome ){
    strncpy(temi[id].nome, nome, len_nome);
    temi[id].finito = false;
    temi[id].punteggio = 0;
}

bool inputStr(char* buffer_in, const int size_buff){
    if (fgets(buffer_in, size_buff, stdin) != NULL) 
        {
            size_t len = strlen(buffer_in);
            if (len > 0 && buffer_in[len - 1] == '\n') 
                buffer_in[len - 1] = '\0';
            else
            {
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF);
            }
            return true;
        }
        return false;
}

PLAYER_STATUS sceltaMenu(){
    
    char scelta[NUM_THEME];
    if(!inputStr(scelta,NUM_THEME)){
        printf("Errore nella recezione dell'input");
        return ENDQUIZ;
    }
    int num_scelto = atoi(scelta);
    switch (num_scelto)
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
    char nome_client[A_MAX];
    char ret_serv[strlen(ACK_NAME)+1];
    int ret;

    do{
            memset(ret_serv,0, strlen(ACK_NAME));
            memset(nome_client,0,A_MAX);

            printf("Scegli  nickname\n");
            inputStr(nome_client, NAME_MAX);

            if(strcmp(nome_client, "") == 0)
            {
                printf("Nickname non valido, inserire almeno un carattere\n");
                continue;
            }

            if(checkCommand(nome_client))
                break;
                //mostra_classifica(sock);

            
            //comando endquiz
            else if(strcmp(nome_client, END_QUIZ) == 0)
                return ENDQUIZ;

            // invio nel nome al server
            else
            {
                //invio del nickname
                if(send(sock, nome_client, NAME_MAX, MSG_NOSIGNAL) == -1){
                    perror("Errore invio nickname");
                    return ENDQUIZ;
                }

                ret = recv(sock, ret_serv, strlen(ACK_NAME),0);
                if (ret <= 0) {
                    perror("Errore ricezione ACK");
                    return ENDQUIZ;
                }
                ret_serv[ret] = '\0';
                printf("risposta server: %s\n",ret_serv);
                
                if(strcmp(ret_serv, ACK_NAME) == 0){
                    printf("\nNickname Valido\n");
                    return  ATTIVO;
                }
                    
                else
                    printf("\nNickname già preso\n");
            }
        } while (true);
    
}

void recvTemi(const int sock){
    int ret;
    char ret_serv[NAME_MAX];

    for(int i = 0; i < NUM_THEME; i++)
        {
            if((ret = recv(sock, (void*)ret_serv, NAME_MAX, 0)) <= 0)
                break;
            ret_serv[ret]  = '\0';
            initTema(i,ret_serv,strlen(ret_serv));
        }
}

void printTemi(){
    printf("\nQuiz disponibili\n");
    printf("+++++++++++++++++++++++++++++++++++\n");

    for(int i = 0; i < NUM_THEME; i++)
        if(temi[i].finito == false )
            printf( "%d - %s \n", i+1, temi[i].nome);
    
    printf("+++++++++++++++++++++++++++++++++++\n");

}


bool scegliTema(const int sock, int* i){
    char ret_serv[NAME_MAX];
    char tema_id[A_MAX];
    uint32_t tema_scelto;
    do
    {
        printf("La tua scelta:\n");
        inputStr(tema_id, A_MAX);

        if(checkCommand(tema_id))
            break; //mostra_classifica(sock);

            
        else if(strcmp(tema_id, END_QUIZ) == 0)
            return -1;

        tema_scelto = atoi(tema_id);
        tema_scelto--;
        if(tema_scelto < 0 || tema_scelto >= NUM_THEME)
        printf("\nTema non valido\n"); 
        
        else if(temi[tema_scelto].finito){
            printf("\nTema già terminato, scegliene un altro\n"); 
            continue;
        }
        
        else
        break;
        
    } while (true);
    
    uint32_t tema_net = htonl(tema_scelto);
    *i = tema_scelto;
    send(sock, &tema_net, sizeof(tema_net), MSG_NOSIGNAL);
    printf("tema %d\n", tema_scelto);
    printf("\nQuiz - %s\n", temi[tema_scelto].nome);
    return true;

}

PLAYER_STATUS quiz(const int sock,const int tema_scelto){
    
    char risposta[A_MAX];
    char ret_serv[Q_MAX];
    int ret;
    RECV_STATUS comunication_status = RECV_QUESTION;
    u_int32_t tema_index = 0;

    while(true)
    {
        memset(ret_serv, 0, Q_MAX);
        //ricezione della domanda
        if((ret = recv(sock, ret_serv, Q_MAX, 0)) <= 0)
            break;

        if(strcmp(ret_serv,EOQ) == 0)
                break;
        //si esce da questo ciclo quando viene inserita una risposta alla domanda
        do
        {
            printf("+++++++++++++++++++++++++++++++++++\n");
            printf("%s\n\n", ret_serv);
            
            //inserimento della risposta
            printf("Risposta:\n");
            inputStr(risposta, A_MAX);

            if(strcmp(risposta, "") == 0)
            {
                printf("Risposta vuota\n");
                continue;
            }

            if(checkCommand(risposta))
                break; //mostra_classifica(sock);

            
            //comando endquiz
            else if(strcmp(risposta, END_QUIZ) == 0)
                return ENDQUIZ;

            else
                break;
            
            
        } while (true);
        
        //invio della risposta alla domanda
        if(send(sock, (void*)risposta, A_MAX, MSG_NOSIGNAL) == -1)
            return ENDQUIZ;

        ret = recv(sock, ret_serv, strlen(ACK_NAME),0);
        if (ret <= 0) {
            perror("Errore ricezione ACK");
            return ENDQUIZ;
        }
        ret_serv[ret] = '\0';
        printf("ritorno: %s\n", ret_serv);
        //ricezione dell'esito
        if(strcmp(ret_serv,ACK_NAME) == 0)
            continue;

        else
            printf("\nRisposta errata: %s\n", ret_serv);
        
    }

    printf("+++++++++++++++++++++++++++++++++++\n");
    printf("Quiz %s completato\n", temi[tema_scelto].nome);

    temi[tema_scelto].finito = true;
    temi_todo--;

    return ATTIVO;
}
