#include "../include/serverlib.h"

extern THEME temi[NUM_THEME]; 
extern PLAYER* lista_player;
extern pthread_mutex_t m;
extern int num_giocatori;

void trimStr(char* str){
    if(!str)
        return;

    char* temp = str;
    do {
        while (*temp == ' ') {
            temp++;
        }
    } while (*str++ = *temp++);
}
void strMinuscolo(char* str){
    for(int i = 0; str[i]; i++){
        if(str[i] == '\n')
            continue;
        str[i] = tolower(str[i]);
    }
}

/// @brief crea una linkedlist di nodi, in ogni nodo è salvata una stringa
/// @param fd file da cui salvare le tringhe
/// @return testa della linkedlist
NODE* caricaRiga(FILE *fd)
{
    char str_riga[Q_MAX];
    NODE* testa = NULL;
    NODE* coda = NULL;;

    while(fgets(str_riga, sizeof(str_riga), fd)!=NULL){
        
        //elimino il ritorno a capo alla fine della stringa
        str_riga[strcspn(str_riga, "\n")] = '\0';

        char str_spazi_bianchi[Q_MAX];
        //controlla e ignora di presenza di righe di soli spazi bianchi
        strncpy(str_spazi_bianchi, str_riga, Q_MAX-1);
        str_spazi_bianchi[strcspn(str_spazi_bianchi, " ")] = '\0';

        if(strlen(str_riga) == 0 ||  strlen(str_spazi_bianchi) == 0 )
            continue;
        
        NODE* new_node = (NODE*)malloc(sizeof(NODE));
        if (!new_node) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        
        // valori nodo
        strncpy(new_node->testo, str_riga, Q_MAX-1);
        new_node->next = NULL;


        if (new_node->testo == NULL) {
            perror("strdup fallita");
            return NULL;
        }

        if(testa == NULL)
            testa = coda = new_node;
        else{
            coda->next = new_node;
            coda = new_node;
        }
        
    }

    return testa;
}

void printLista(NODE* lista){

    printf("\n--- Lista ----\n");
    for (NODE* nodo_stampa = lista; nodo_stampa != NULL ;nodo_stampa = nodo_stampa->next)
    {
        printf("- %s\n",nodo_stampa->testo);
    }
    
}


/// @brief carica le domande dal file di testo "domande"numtema.txt
/// @param num_tema numero del tema, passato per permettere la concatenzione tra stringhe in modo da accedere al relativo file tramite il numero del tema 
/// @param tema tema in question
/// @return risultato dell'operazione
bool caricaDomande(size_t num_tema, THEME *tema)
{
    FILE* fd;
    char file_name[100];
    snprintf(file_name, sizeof(file_name), "txt/domande%ld.txt", num_tema+1);
    //printf("%s\n",file_name);

    fd = fopen(file_name, "r"); 
    if (!fd) {
        perror("Errore nell'apertura di domande.txt: ");
        return false;
    }

    tema->lista_domande = caricaRiga(fd);

    fclose(fd);
    return true;
}

/// @brief carica le risposte dal file di testo "risposte"numtema.txt
/// @param num_tema numero del tema, passato per permettere la concatenzione tra stringhe in modo da accedere al relativo file tramite il numero del tema 
/// @param tema tema in question
/// @return 
bool caricaRisposte(size_t num_tema, THEME *tema)
{
    FILE* fd;
    char file_name[100];
    snprintf(file_name, sizeof(file_name), "txt/risposte%ld.txt", num_tema+1);

    fd = fopen(file_name, "r"); 
    if (!fd) {
        perror("Errore nell'apertura di un file risposte.txt: ");
        return false;
    }

    tema->lista_risposte = caricaRiga(fd);

    fclose(fd);
    return true;
}

/// @brief funzione che carica i nomi dei temi, per ogni tema vengono salvate in apposite strutture le domande e le risposte
/// @param temi array di temi attualmente in corso
/// @return risultato dell'operazione
bool caricaTemi(THEME *temi){
    FILE* fd  = fopen(THEME_PATH,"r");
    char tema_nome[Q_MAX];

    THEME* new_tema;

    if (!fd) {
        perror("Errore nell'apertura di temi.txt\n");
        return false;
    }
    
    for(size_t i= 0; i< NUM_THEME && fgets(tema_nome, sizeof(tema_nome), fd) != NULL; i++) {
        
        //rimozione carattere new line con ritorno a capo
        tema_nome[strcspn(tema_nome, "\n")] = '\0';

        new_tema = &temi[i];
        //@todo #6 i nomi dei temi andranno liberati alla fine
        new_tema->name = strdup(tema_nome);
        if (!new_tema->name) { 
            perror("strdup fallita"); 
            fclose(fd);
            return false; 
            }


        new_tema->lista_domande = NULL;
        new_tema->lista_risposte = NULL;

        new_tema->NUM_DOMANDE = 0; 
        if (new_tema->name == NULL) {
            perror("strdup fallita");
            return 0;
        }
        
        caricaRisposte(i, new_tema);
        caricaDomande(i,new_tema);
        
        // NODE* nodo = new_tema->lista_domande;

        // for (; nodo != NULL; nodo = nodo->next) {
        //     printf("%s\n", nodo->testo);

        // }
        // nodo = new_tema->lista_risposte;

        // for (; nodo != NULL; nodo = nodo->next) {
        //     printf("%s\n", nodo->testo);
            
        // }
    }
    
    fclose(fd);
    return true;
}

// lista utenti
PLAYER* mallocGiocatore(const char* nome){
    PLAYER* new_node = (PLAYER*)malloc(sizeof(PLAYER));
    if (!new_node) { 
        perror("errore malloc"); 
        return NULL; 
    }
    strncpy(new_node->name, nome, NAME_MAX-1);
    new_node->name[NAME_MAX-1] = '\0';
    new_node->attivo = true;
    //inzialmente ogni giocatore avrà nessun tema finito 
    //è niente punteggi
    for(int i = 0; i < NUM_THEME; i++){
        new_node->temi_finiti[i] = false;
        new_node->temi_punteggi[i] = 0;
    }

    new_node->next = NULL;

    return new_node;

}

PLAYER* trovaUtenteDalNome(const char* nomeGiocatore){
    
    pthread_mutex_lock(&m);
    
    for (PLAYER* p = lista_player; p != NULL;  p = p->next)
    {
        //@todo scegli una condizione tra le due
        if(strcmp(p->name,nomeGiocatore) == 0){
            pthread_mutex_unlock(&m);
            return p;
        }
    }
    
    pthread_mutex_unlock(&m);
    return NULL;
}

PLAYER* aggiungiGiocatore(const char* nome){
    
    PLAYER* new_node = mallocGiocatore(nome);
    pthread_mutex_lock(&m);
    new_node->next = lista_player;
    lista_player = new_node;
    num_giocatori++;
    pthread_mutex_unlock(&m);

    return new_node;

}
void stampa_menu()
{

    printf("Trivia Quiz\n");
    printf("+++++++++++++++++++++++++\n");
    printf("Temi:\n");

    for (int i = 0; i< NUM_THEME; i++ )
    {
        printf("%d - %s\n", i+1, temi[i].name);
    }
    printf("+++++++++++++++++++++++++\n");
    
    printf("\nPartecipanti (%d)\n", num_giocatori);
    for (PLAYER* giocatore = lista_player; giocatore != NULL; giocatore = giocatore->next)
    {
        printf("- %s\n",  giocatore->name);
    }
    
}

int totStrTemiSize(THEME* temi){
    int size_all = 0;
    char buffer[100];

    for(int i = 0 ; i < NUM_THEME; i++){
        sprintf(buffer, "\n%d - ", (i+1) );
        size_all+= strlen(temi[i].name) + strlen(buffer);
    } 

    return size_all;
 }


// comunicazione client
bool checkCommand(const char* buff){
    return (strcmp(buff, SHOW_SCORE) == 0 || strcmp(buff, END_QUIZ) == 0) ? true : false;
}
int compare(const void* va, const void* vb)
{
    const PLAYER_ORD *a = va;
    const PLAYER_ORD *b = vb;

    if (a->punteggio != b->punteggio)
        return b->punteggio - a->punteggio;      
    return strcmp(a->name, b->name);
}

//per ogni tema ordina gli utenti in base al punteggio
// prima manda i temi ordinati
// poi manda INIZIO COMUNICAZIONE TEMI FINITI
// utenti che hanno finito i temi 

void sendOrdScore(const int sock){

    
    PLAYER_ORD utenti[num_giocatori];
    char score_buffer[SCOREBUFF_SIZE];
    
    pthread_mutex_lock(&m);
    int nplayers = num_giocatori;
    pthread_mutex_unlock(&m);

    uint32_t punteggio;

    for (int itema = 0; itema < NUM_THEME; itema++)
    {

        memset(score_buffer, 0,SCOREBUFF_SIZE);
        int j = 0;
        
        pthread_mutex_lock(&m);
        for (PLAYER* player = lista_player;  player!= NULL && j < nplayers; player = player->next, j++)
        {

            strncpy(utenti[j].name, player->name, NAME_MAX);
            utenti[j].punteggio = player->temi_punteggi[itema];

        }
        pthread_mutex_unlock(&m);
        
        qsort(utenti, j, sizeof(PLAYER_ORD), compare);
        int size = snprintf(score_buffer, SCOREBUFF_SIZE, "Punteggio tema %d\n" ,itema+1);
        printf("Punteggio tema %d\n" ,itema+1);
        // send(sock, score_buffer, strlen(score_buffer), MSG_NOSIGNAL);

        for (int k = 0; k < nplayers; k++){


            punteggio = (utenti[k].punteggio == 0) ? 0: utenti[k].punteggio ;
            printf("- %s: %d\n", utenti[k].name, utenti[k].punteggio );

            size += snprintf(score_buffer, SCOREBUFF_SIZE,"- %s %d\n", utenti[k].name, punteggio);
            // send(sock, score_buffer, strlen(score_buffer), MSG_NOSIGNAL);
        }

    }
    send(sock, EOS, strlen(EOS), MSG_NOSIGNAL); 
    printf(" FINE SEND ORD, num player %d\n", nplayers );


}
   
void stampaListaGiocatori(){
    PLAYER* player = lista_player; 

        while (player!= NULL)
        {   
            printf("- %s: %d\n", player->name, player->temi_punteggi[1] );    
            player= player->next;
        }

}

STATO_CLIENT assegnaNome(PLAYER** new_player , const int sock){
    char buffer_nome[NAME_MAX];
    int ret;

    do
    {
        memset(buffer_nome, 0, NAME_MAX);
        ret = recv(sock, buffer_nome, NAME_MAX-1, 0);

        if (ret <= 0)
           return ENDQUIZ; 

        buffer_nome[ret] = '\0';

        //trimStr(buffer_nome);

        //stringa vuota    
        if(strlen(buffer_nome) == 0)
            continue;

        strMinuscolo(buffer_nome);
        printf("\nUtente propone %s\n", buffer_nome);

        if(trovaUtenteDalNome(buffer_nome) == NULL){
            //invio ack per indicare la correttezza del nome
            send(sock, ACK_NAME, strlen(ACK_NAME), 0);
            *(new_player) = aggiungiGiocatore(buffer_nome);
            break;
        }

        else 
            send(sock, NACK_NAME, strlen(NACK_NAME), 0);
    } 
    while (true);

    return INVIA_TABELLONE;
}