#include "../include/serverlib.h"


//data structure usate dal server
THEME temi[NUM_THEME]; 
PLAYER* lista_player = NULL;
pthread_mutex_t m;
int num_giocatori  = 0;

bool isShowScore(char* str){
    return (strcmp(str,"show score") == 0);
}

bool isEndQuiz(char* str){

    return (strcmp(str,"endquiz") == 0);

}

void* th_client(void *arg){

    //informazioni da scambiare col client
    int client_sock = *(int*)(arg);

    char buffer_risposta[A_MAX];
    
    //player associato al client
    PLAYER* new_player = NULL;
    THEME* tema_scelto;
    STATO_CLIENT STAR = SCELTANOME;
    uint32_t index_tema = 0;
    ssize_t bytes_ricevuti = 0;
    bool first_time = true; 
    
    do{
        switch (STAR)
        {
            case SCELTANOME:
                STAR = assegnaNome(&new_player, client_sock);

            break;

            case INVIA_TABELLONE:
                //invio dei nomi dei temi
                for(int i = 0; i < NUM_THEME; i++)
                    send(client_sock, (void*)temi[i].name, NAME_MAX, MSG_NOSIGNAL);

                bytes_ricevuti = recv(client_sock, &index_tema, sizeof(index_tema), MSG_NOSIGNAL);
                STAR = DOMANDE;
                break;
                
            case DOMANDE:
                bytes_ricevuti = recv(client_sock, &index_tema, sizeof(index_tema), MSG_NOSIGNAL);
                index_tema = ntohl(index_tema);
                printf("tema %d\n", index_tema);

                tema_scelto = &temi[index_tema];
                if(bytes_ricevuti<= 0)
                break;
                
                
                NODE* domanda = tema_scelto->lista_domande;
                NODE* risposta = tema_scelto->lista_risposte;
                
                
                while (domanda != NULL && risposta != NULL) {

                    send(client_sock, domanda->testo, strlen(domanda->testo), MSG_NOSIGNAL);

                        
                    memset(buffer_risposta, 0, A_MAX);

                    bytes_ricevuti = recv(client_sock, buffer_risposta, A_MAX, MSG_NOSIGNAL);
                    
                    if(bytes_ricevuti <= 0 || strlen(buffer_risposta) == 0)
                        continue;
                    
                    buffer_risposta[bytes_ricevuti] = '\0';
                    
                    // strMinuscolo(risposta->testo);
                    // strMinuscolo(buffer_risposta);
                    
                    if(checkCommand(buffer_risposta)){
                        printf("comando utente %s: '%s'\n", new_player->name ,buffer_risposta );
                        sendOrdScore(client_sock);
                        send(client_sock, EOS, strlen(EOS), MSG_NOSIGNAL);
                        break;
                    }
                    printf("domanda: %s\n", domanda->testo);
                    printf("riposta utente: %s\n", buffer_risposta);
                    printf("riposta corretta: %s\n", risposta->testo);
                    printf((strcmp(buffer_risposta, risposta->testo) == 0) ? "Uguali\n": "Diverse\n");
                    
                    // controllo risposta corretta
                    if(strcmp(buffer_risposta, risposta->testo) == 0) {
                        //aumenta punteggio giocatore 
                        
                        pthread_mutex_lock(&tema_scelto->mutex);
                        new_player->temi_punteggi[index_tema]++;
                        pthread_mutex_unlock(&tema_scelto->mutex);

                        send(client_sock, ACK_NAME, strlen(ACK_NAME), MSG_NOSIGNAL);

                        //la domanda avanza solo in caso di corretta risposta
                        domanda = domanda->next;
                        risposta = risposta->next;
                    }
                    else if(buffer_risposta)
                        send(client_sock, NACK_NAME, strlen(NACK_NAME), MSG_NOSIGNAL);

                }

                new_player->temi_finiti[index_tema] = true;
                send(client_sock, EOQ, strlen(EOQ), MSG_NOSIGNAL);
                sendOrdScore(client_sock);
                break;

            case SHOWSCORE:
                break;

            case ENDQUIZ:
            break;
                break;

            default:
                break;
        }
    }while(true);

    free(arg);
}


int main(){

    //dichiaro il thread
    int *arg;
    pthread_t thread_player;
    pthread_mutex_init(&m,NULL);
    
    //descrittori di socket e indirizzi degli stessi
    int serv_sock, new_sock;
    struct sockaddr_in my_addr, client_addr; 
    
    //init temi
    caricaTemi(temi);
    stampa_menu();

    if((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Errore: creazione del socket fallita\n");
        exit(EXIT_FAILURE);
    }

    int option = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &(option), sizeof(option));
    //creazione indirizzo e porta associati al socket
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(3456);
    inet_pton(AF_INET, "127.0.0.1", &my_addr.sin_addr);

    if(bind(serv_sock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1)
    {
        perror("Errore: bind() fallita: ");
        close(serv_sock);
        exit(EXIT_FAILURE);
    }

    //messa in ascolto del socket
    if(listen(serv_sock, 10) == -1)
    {
        perror("Errore: listen() fallita\n");
        close(serv_sock);
        exit(EXIT_FAILURE);
    }

    printf("\n");

    stampa_menu(temi, lista_player, num_giocatori, &m);

    while(1){
        socklen_t len_a = sizeof(client_addr);

        //il server accetta la connessione proveniente da un client
        if((new_sock = accept(serv_sock, (struct sockaddr*)&client_addr, &len_a)) == -1)
        {
            perror("Errore, connessione non accettata");
            continue;
        }

        //allocazione e creazione di un thread per gestire il nuovo client
        arg = malloc(sizeof(int));
        //l'argomento è il valore del descrittore del nuovo socket
        *arg = new_sock;
        if(pthread_create(&thread_player, NULL, th_client, (void*)arg) != 0)
        {
            pthread_detach(thread_player);
            perror("Errore: pthread_create() fallita\n");
            close(new_sock);
        }


    }
    return 0;
}