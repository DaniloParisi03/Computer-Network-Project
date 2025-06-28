#include "../include/serverlib.h"


//data structure usate dal server
THEME temi[THEME_MAX]; 
PLAYER* giocatori = NULL;
int NUM_GIOCATORI  = 0;
pthread_mutex_t mutex_giocatori;


void th_client(void *arg){

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
        perror("Errore: bind() fallita\n");
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
    printf("Server in ascolto\n");

    stampa_menu(temi, giocatori, NUM_GIOCATORI, &mutex_giocatori);

    while(1){
        socklen_t len_a = sizeof(client_addr);

        //il server accetta la connessione proveniente da un client
        if((new_sock = accept(serv_sock, (struct sockaddr*)&client_addr, &len_a)) == -1)
        {
            perror("Errore, connessione non accettata");
            continue;
        }
        printf("Connessione con socket %d accettata\n", new_sock);

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