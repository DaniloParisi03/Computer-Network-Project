#include "all.h"
#include "../const/const.h"


typedef struct theme THEME;
typedef struct node NODE; 
typedef struct node_tree TNODE;
typedef struct player PLAYER;

struct node{
    char testo[Q_MAX];
    NODE* next;
};

struct node_tree
{ 
    int punteggio; 
    PLAYER* giocatore;
    TNODE*left; 
    TNODE*right;
};


struct player{

    char name[NAME_MAX];
    bool attivo;
    bool temi_iniziati[NUM_THEME];
    bool temi_finiti[NUM_THEME];
    PLAYER* next;

};


struct theme{

    //nome tema
    char* name;
    //numero attuale di domande per tema
    int NUM_DOMANDE;

    //linked list delle domande
    NODE* lista_domande;
    NODE* lista_risposte;

    //albero ordinato di ricerca per salvare i dati
    TNODE* score_root;

    //atto per gli accessi in classifica
    pthread_mutex_t mutex;
};


//carica le domande dal file di testo 1 ok, 0 errore

bool caricaDaFile(FILE* fd, NODE* lista);

bool caricaDomande(size_t , THEME *);
bool caricaRisposte(size_t num_tema, THEME* lista_domande);

bool caricaTemi(THEME* temi);

//la domanda deve essere univoca per essere inserita
// +1 inserito correttamente , 0 se non inserito, -1 se già presente
bool aggiungiDomanda(int num_tema, char* domanda);

void stampa_menu(THEME *lista_temi, PLAYER* giocatori, int giocatore_attuali ,pthread_mutex_t* m );

// funzioni per la gestione dei giocatori

bool trovaUtenteDalNome(char* nome, PLAYER* lista_g, pthread_mutex_t* m);
void aggiungiGiocatore(char* giocatore, PLAYER** lista_g, pthread_mutex_t* m);
PLAYER* mallocGiocatore(char* nome);

//gestione tramite albero di ricerca per una question di complessità, ovvero 
//ordinandno ogni volta un array complessità n*log(n), 
//così la: 2*O(log(n)) + O(n) e la stampa sarà O(n)

TNODE* creaNodo(PLAYER* giocatore, int punteggio);
TNODE* inserisciPlayer(TNODE* root, PLAYER *giocatore, int punteggio);


TNODE* rimuoviPlayer(TNODE *node, PLAYER *giocatore, int punteggio);

void stampaOrdinato(TNODE* root);

int totStrTemiSize(THEME* temi);
char* concatenaStrTemi(THEME* temi, const int size_all);
char* concatenaStrClassifica(THEME* temi, const int size_all);
