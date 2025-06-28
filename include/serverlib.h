#include "all.h"
#include "../const/const.h"


typedef struct theme THEME;
typedef struct player PLAYER;

typedef struct node NODE; 

struct node{
    char testo[Q_MAX];
    NODE* next;
};

struct score{
    PLAYER *p;
    int punteggio;
};


struct player{
    char name[NAME_MAX];
    bool active;
    bool temi_iniziati[THEME_MAX];
    bool temi_finiti[THEME_MAX];
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

bool cercaGiocatore(char* nome, PLAYER* lista_g, pthread_mutex_t* m);

int inserisciGiocatore(char* nome,PLAYER** lista_g, int* giocatore_attuali, pthread_mutex_t* m );
void aggiornaPunteggio(char* nome, int num_tema, THEME *lista_temi);
int ord(const void* a, const void* b);
PLAYER creaGiocatore();