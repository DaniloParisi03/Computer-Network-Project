#include "../include/serverlib.h"


//data structure usate dal server
THEME temi[2]; 
PLAYER* lista_player = NULL;
pthread_mutex_t mutex_giocatori;
int num_giocatori  = 0;

bool isShowScore(char* str){
    // strMinuscolo(str);
    if(strcmp(str,"show score"))
        return true;
    return false;
}

bool isEndQuiz(char* str){

    // strMinuscolo(str);
    if(strcmp(str,"endquiz"))
        return true;
    //elimina utente 
    return false;
}

void* th_client(void *arg){

    //informazioni da scambiare col client
    int client_sock = *(int*)(arg);

    char buffer_nome[NAME_MAX];
    char buffer_risposta[A_MAX];
    int ret;
    

    
    //player associato al client
    PLAYER* new_player;
    THEME* tema_scelto;
    uint32_t index_tema = 0;

    //se l'utente è attivo invia temi
    uint32_t str_temi_len, str_nome_tema_len;
    char* str_temi;

    
    //scelta del nome
    do
    {
        memset(buffer_nome, 0, NAME_MAX);
        ret = recv(client_sock, buffer_nome, NAME_MAX, 0);

        if (ret <= 0)
            continue; 

        buffer_nome[ret] = '\0';
        trimStr(buffer_nome);
        
        //stringa vuota    
        if(strlen(buffer_nome) == 0)
            continue;

        strMinuscolo(buffer_nome);
        printf("\nUtente propone %s\n", buffer_nome);

        if(trovaUtenteDalNome(buffer_nome,lista_player, &mutex_giocatori) == false){
            //invio ack per indicare la correttezza del nome
            send(client_sock, ACK_NAME, strlen(ACK_NAME), 0);
            new_player = aggiungiGiocatore(buffer_nome, &lista_player, &num_giocatori, &mutex_giocatori);
            break;
        }

        else if(trovaUtenteDalNome(buffer_nome,lista_player, &mutex_giocatori) == true)
            send(client_sock, NACK_NAME, strlen(NACK_NAME), 0);
    } 
    while (true);

    //aggiorno numero dei giocatori
    num_giocatori++;

    //invio dei nomi dei temi
    for(int i = 0; i < NUM_THEME; i++)
        send(client_sock, (void*)temi[i].name, NAME_MAX, MSG_NOSIGNAL);

   
    do
    {
        ssize_t bytes_ricevuti = recv(client_sock, &index_tema, sizeof(index_tema), 0);
        if (bytes_ricevuti != sizeof(index_tema)) {
            perror("recv non funzionante");
            exit(EXIT_FAILURE);
        }
        
        index_tema = ntohl(index_tema)-1;
        tema_scelto = &temi[index_tema];

        printf("Tema ricevuto dal client: %d\n", index_tema);

        send(client_sock, tema_scelto->name, strlen(tema_scelto->name), MSG_NOSIGNAL);

        int byte_ricevuti = recv(client_sock, buffer_risposta, strlen(ACK_NAME), 0);
        buffer_risposta[byte_ricevuti] = '\0';
        new_player->temi_punteggi[index_tema] = 0;

        if(strcmp(buffer_risposta, ACK_NAME) == 0){
                
            NODE* domanda = tema_scelto->lista_domande;
            //in caso si showscore rimanda l'ultima domanda non risposta
            NODE* risposta = tema_scelto->lista_risposte;
            
            while (domanda != NULL) {
                
                send(client_sock, domanda->testo, Q_MAX, MSG_NOSIGNAL);
                printf("Domanda: %s\n", domanda->testo); 

                do{
                    memset(buffer_risposta, A_MAX, 0);
                    bytes_ricevuti = recv(client_sock, buffer_risposta, A_MAX, MSG_NOSIGNAL);
                    
                    if(bytes_ricevuti<= 0)
                        continue;
                    buffer_risposta[bytes_ricevuti] = '\0';
                    
                    strMinuscolo(risposta->testo);
                    strMinuscolo(buffer_risposta);

                    if(checkCommand(buffer_risposta)){
                        printf("comando utente: %s\n", buffer_risposta );
                        sendOrdScore(lista_player,num_giocatori, client_sock, &mutex_giocatori);
                        break;
                    }


                    //controllo risposta corretta
                    if(strcmp(buffer_risposta, risposta->testo) == 0) {
                        //aumenta punteggio giocatore 
                        new_player->temi_punteggi[index_tema]++;
                        send(client_sock, ACK_NAME, strlen(ACK_NAME), MSG_NOSIGNAL);

                        //la domanda avanza solo in caso di corretta risposta
                        domanda = domanda->next;
                        risposta = risposta->next;
                        break;
                    }
                    else
                        send(client_sock, NACK_NAME, strlen(NACK_NAME), MSG_NOSIGNAL);

                }while (true);

                //una volta usciti saranno finiti i temi
                new_player->temi_finiti[index_tema] = true;
            }
            send(client_sock, EOQ, strlen(EOQ), MSG_NOSIGNAL);
        }

    } while (true);
    
    free(arg);
}


int main(){

    //dichiaro il thread
    int *arg;
    pthread_t thread_player;
    pthread_mutex_init(&mutex_giocatori,NULL);
    
    //descrittori di socket e indirizzi degli stessi
    int serv_sock, new_sock;
    struct sockaddr_in my_addr, client_addr; 
    
    //init temi
    caricaTemi(temi);
    stampa_menu(temi, lista_player, num_giocatori, &mutex_giocatori);

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

    stampa_menu(temi, lista_player, num_giocatori, &mutex_giocatori);

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
            perror("Errore: pthread_create() fallita\n");
            close(new_sock);
        }

    }
    return 0;
}