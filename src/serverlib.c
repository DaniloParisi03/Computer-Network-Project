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

        //controlla di presenza di righe di soli spazi bianchi
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

//gestione albero degli score

TNODE* creaNodo(PLAYER* giocatore, int punteggio){
    TNODE* new_node = (TNODE*)malloc(sizeof(TNODE));
    new_node->giocatore = giocatore;
    new_node->punteggio = punteggio;
    new_node->left = NULL;
    new_node->right = NULL;
}

void freeNodo(TNODE* nodo){
    if(nodo == NULL)
        return;

    free(nodo);
}

void freeTree(TNODE* root) {

    if (root == NULL) 
        return;

    freeTree(root->left);
    freeTree(root->right);
    freeNodo(root);
}

TNODE* trovaSuccessore(TNODE* padre ){

    for (padre = padre->right; padre != NULL && padre->left != NULL; padre = padre->left);
    return padre;
}

TNODE* trovaNodo(){

    return NULL;
}
TNODE* inserisciPlayer(TNODE* node,  PLAYER *giocatore, int punteggio)
{
    if(node == NULL)
        return creaNodo(giocatore,punteggio);

    if(node->punteggio >= punteggio)
        node->left = inserisciPlayer( node->left, giocatore, punteggio);

    else
        node->right = inserisciPlayer(node->right, giocatore, punteggio);
    return node;
}
TNODE* rimuoviPlayer(TNODE *node, PLAYER *giocatore, int punteggio)
{
    if(node == NULL)
        return node;
        
    if(node->punteggio >= punteggio && node->giocatore != giocatore)
        node-> left = rimuoviPlayer( node->left, giocatore, punteggio);

    else if(node->punteggio < punteggio && node->giocatore != giocatore)
        node-> right = rimuoviPlayer(node->right, giocatore, punteggio);
    

    else if(node->punteggio == punteggio && node->giocatore == giocatore){
        TNODE* temp_node;
        if(node->left == NULL){
            temp_node = node->right;
            freeNodo(node);
            return temp_node;
        }

        else if(node->right == NULL){
            temp_node = node->left;
            freeNodo(node);
            return temp_node;
        }

        //ha entrambi i figli
        else{

            temp_node = trovaSuccessore(node);
            node->punteggio = temp_node->punteggio;
            node->giocatore = temp_node->giocatore;

            node->right = rimuoviPlayer(node->right, giocatore, node->punteggio);
        }
    }
    return node;

}
void stampaOrdinato(TNODE* root) {
    if (root != NULL) {
        stampaOrdinato(root->left);
        printf("%s: (%d)\n ", root->giocatore->name, root->punteggio);
        stampaOrdinato(root->right);
    }
}

// lista utenti
PLAYER* mallocGiocatore(char* nome){

    PLAYER* new_node = (PLAYER*)malloc(sizeof(PLAYER));
    strcpy(new_node->name,nome);

    //new_node->name[NAME_MAX - 1] = '\0'; 
    new_node->attivo = true;

    for(int i = 0; i < NUM_THEME; i++){
        new_node->temi_finiti[i] = false;
        new_node->temi_iniziati[i] = -1;
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

PLAYER* aggiungiGiocatore(char* nome, PLAYER** lista_g, pthread_mutex_t* m){
    
    PLAYER* new_node = mallocGiocatore(nome);
    pthread_mutex_lock(m);

    new_node->next = *(lista_g);
    *(lista_g) = new_node;

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

 char* concatenaStrTemi(THEME* temi, const int size_all, PLAYER* client){

    char buffer[100];
    char* str_concatenata = (char*)malloc(size_all);

    for(int i = 0 ; i < NUM_THEME; i++){

        if(client->temi_finiti[i] == true)
            continue;;
        sprintf(buffer, "\n%d - ", (i+1));
        
        if(i==0)
            strcpy(str_concatenata, buffer);
        
        else
            strcat(str_concatenata, buffer);
        strcat(str_concatenata, temi[i].name);

    }

    return str_concatenata;
} 


// comunicazione client
