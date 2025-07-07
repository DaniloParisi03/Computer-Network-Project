#include "../include/serverlib.h"


//data structure usate dal server
THEME temi[2]; 
PLAYER* lista_player = NULL;
int num_giocatori  = 0;
pthread_mutex_t mutex_giocatori;

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
    uint32_t tema_scelto = -1;

    //se l'utente è attivo invia temi
    uint32_t str_temi_len;
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
            new_player = aggiungiGiocatore(buffer_nome, &lista_player, &mutex_giocatori);
            break;
        }
        else if(trovaUtenteDalNome(buffer_nome,lista_player, &mutex_giocatori) == true)
            send(client_sock, NACK_NAME, strlen(NACK_NAME), 0);
    } 
    while (true);

    
    str_temi_len = htonl(totStrTemiSize(temi));
    str_temi = concatenaStrTemi(temi, str_temi_len, new_player);
    printf("\nServe invia %s\n", str_temi);

    //il serven invia prima la lunghezza della stringa e poi la stringa stessa
    do{
        //scelta temi
        send(client_sock, &str_temi_len, sizeof(uint32_t), 0);
        send(client_sock, str_temi, str_temi_len, 0);
        //client manda tema scelto
        size_t byte_ricevuti;

        byte_ricevuti = recv(client_sock, &tema_scelto, sizeof(uint32_t), 0);
        tema_scelto = ntohl(tema_scelto)-1;


        if (byte_ricevuti <= 0) {
            printf("Connessione chiusa dal client\n");
            break;
        }

        //l'utente sceglie il tema e gli verrano mostrate le domande  di esso

        new_player->temi_iniziati[tema_scelto] = 0;

        //inserisciPlayer(temi[tema_scelto].score_root, new_player, 0);
        send(client_sock, temi[tema_scelto].name, NAME_MAX, 0);

        for (NODE* domanda = temi[tema_scelto].lista_domande; domanda; domanda = domanda->next)
        {
            printf("\n%s", domanda->testo);
            send(client_sock, domanda->testo, strlen(domanda->testo), 0);
            //riposta
            /*byte_ricevuti = recv(client_sock, buffer_risposta, A_MAX, 0);
        
            if (byte_ricevuti <= 0) {
                printf("Connessione chiusa dal client\n");
                break;
            }
            buffer_risposta[byte_ricevuti] = '\0';*/

        }
        //comunica al client che le domande per il tema scelto sono terminate
        //send(client_sock, EOQ, strlen(EOQ), 0);

        

    } while (true);
    


    close(client_sock);
    free(str_temi);
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