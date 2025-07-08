#include "all.h"
#include "../const/const.h"


typedef struct theme THEME;
typedef struct node NODE; 
typedef struct player PLAYER;
typedef struct player_ord PLAYER_ORD;

struct node{
    char testo[Q_MAX];
    NODE* next;
};


struct player_ord{
    char name[NAME_MAX];
    uint32_t punteggio;
};
struct player{

    char name[NAME_MAX];
    bool attivo;

    int temi_punteggi[NUM_THEME];
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

    //atto per gli accessi in classifica
    pthread_mutex_t mutex;
};


//carica le domande dal file di testo 1 ok, 0 errore
void trimStr(char* str);
void strMinuscolo(char* str);

//

bool caricaDomande(size_t , THEME *);
bool caricaRisposte(size_t num_tema, THEME* lista_domande);

bool caricaTemi(THEME* temi);

//la domanda deve essere univoca per essere inserita
// +1 inserito correttamente , 0 se non inserito, -1 se già presente

void stampa_menu(THEME *lista_temi, PLAYER* giocatori, int giocatore_attuali ,pthread_mutex_t* m );

// funzioni per la gestione dei giocatori

bool trovaUtenteDalNome(char* nome, PLAYER* lista_g, pthread_mutex_t* m);
PLAYER* aggiungiGiocatore(char* giocatore, PLAYER** lista_g, int* num_players, pthread_mutex_t* m);
PLAYER* mallocGiocatore(char* nome);

//gestione tramite albero di ricerca per una question di complessità, ovvero 
//ordinandno ogni volta un array complessità n*log(n), 
//così la: 2*O(log(n)) + O(n) e la stampa sarà O(n)


PLAYER* addScore(int* player, pthread_mutex_t* m);


int totStrTemiSize(THEME* temi);
char* concatenaStrTemi(THEME* temi, const int size_all, PLAYER* client);
char* concatenaStrClassifica(THEME* temi, const int size_all);

bool checkCommand(char* buff);
void sortUser(char* lista_utenti);
void sendOrdScore( PLAYER* lista_player, const int num_players, const int sock, pthread_mutex_t *m);