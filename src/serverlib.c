#include "../include/serverlib.h"

void trimStr(char* str){
    char* temp = str;
    do {
        while (*temp == ' ') {
            ++temp;
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
        return 1;
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
        return 1;
    }
    
    for(size_t i= 0; i< NUM_THEME && fgets(tema_nome, sizeof(tema_nome), fd) != NULL; i++) {
        
        //rimozione carattere new line con ritorno a capo
        tema_nome[strcspn(tema_nome, "\n")] = '\0';

        new_tema = &temi[i];
        //@todo #6 i nomi dei temi andranno liberati alla fine
        new_tema->name = strdup(tema_nome);

        if (new_tema->name == NULL) {
            perror("strdup fallita");
            return 0;
        }
        
        caricaDomande(i,new_tema);
        caricaRisposte(i,new_tema);
    }
    
    fclose(fd);
    return 1;
}

// lista utenti
PLAYER* mallocGiocatore(char* nome){

    PLAYER* new_node = (PLAYER*)malloc(sizeof(PLAYER));
    strcpy(new_node->name,nome);

    //new_node->name[NAME_MAX - 1] = '\0'; 
    new_node->attivo = true;

    for(int i = 0; i < NUM_THEME; i++){
        new_node->temi_finiti[i] = false;
        new_node->temi_punteggi[i] = -1;
    }

    new_node->next = NULL;
    return new_node;

}

bool trovaUtenteDalNome(char* nomeGiocatore, PLAYER* lista_g, pthread_mutex_t* m){
    
    pthread_mutex_lock(m);
    
    for (PLAYER* p = lista_g; p != NULL;  p = p->next)
    {
        //@todo scegli una condizione tra le due
        if(strcmp(p->name,nomeGiocatore) == 0){
            pthread_mutex_unlock(m);
            return true;
        }
    }
    
    pthread_mutex_unlock(m);
    return false;
}

PLAYER* aggiungiGiocatore(char* nome, PLAYER** lista_g, int* num_players, pthread_mutex_t* m){
    
    PLAYER* new_node = mallocGiocatore(nome);
    pthread_mutex_lock(m);

    new_node->next = *(lista_g);
    *(lista_g) = new_node;
    *(num_players)++;
    pthread_mutex_unlock(m);

    return new_node;

}
void stampa_menu(THEME* lista_temi,PLAYER* giocatori, int giocatore_attuali, pthread_mutex_t *m)
{

    printf("Trivia Quiz\n");
    printf("+++++++++++++++++++++++++\n");
    printf("Temi:\n");

    for (int i = 0; i< NUM_THEME; i++ )
    {
        printf("%d - %s\n", i, lista_temi[i].name);
    }
    printf("+++++++++++++++++++++++++\n");
    
    printf("\nPartecipanti (%d)\n", giocatore_attuali);
    for (PLAYER* giocatore = giocatori; giocatore != NULL; giocatore = giocatore->next)
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
bool checkCommand(char* buff){
    return (strcmp(buff, SHOW_SCORE) == 0 ? true : false);
}
void sortUser(char* buff){
    
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
void sendOrdScore( PLAYER* lista_player, const int num_players, const int sock, pthread_mutex_t *m){

    PLAYER_ORD utenti[num_players];
    char score_buffer[SCOREBUFF_SIZE];
    pthread_mutex_lock(m);
    for (int itema = 0; itema < NUM_THEME; itema++)
    {

        int j = 0;
        PLAYER* player = lista_player; 

        while (player && j < num_players)
        {   
            strcpy(utenti[j].name, player->name);
            utenti[j].punteggio = player->temi_punteggi[itema];

            player = player->next;
            j++;
        }
        
    }
    pthread_mutex_unlock(m);
    
    qsort(utenti, num_players, sizeof(PLAYER_ORD), compare);
    
    for (int k = 0; k < num_players; k++){
        sprintf(score_buffer, sizeof(score_buffer),"- %s %d\n", utenti[k].name, (utenti[k].punteggio == -1) ? 0: utenti[k].punteggio );
        send(sock, (void*)score_buffer, strlen(score_buffer), MSG_NOSIGNAL);
    }
   
}
