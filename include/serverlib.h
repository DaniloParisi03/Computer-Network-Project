#include "all.h"
#include "../const/const.h"


typedef struct theme THEME;
typedef struct node NODE; 
typedef struct player PLAYER;
typedef struct player_ord PLAYER_ORD;
typedef enum status_cl STATO_CLIENT;

enum status_cl{SCELTANOME, INVIA_TABELLONE, DOMANDE, SHOWSCORE, ENDQUIZ};

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

bool caricaDomande(size_t num, THEME * tema);
bool caricaRisposte(size_t num_tema, THEME* lista_domande);

bool caricaTemi(THEME* temi);

//la domanda deve essere univoca per essere inserita
// +1 inserito correttamente , 0 se non inserito, -1 se già presente

void stampa_menu();

// funzioni per la gestione dei giocatori

PLAYER* trovaUtenteDalNome(const char* nome);
PLAYER* aggiungiGiocatore(const char* giocatore);
PLAYER* mallocGiocatore(const char* nome);




int totStrTemiSize(THEME* temi);
char* concatenaStrTemi(THEME* temi, const int size_all, PLAYER* client);

bool checkCommand(const char* buff);
void sendOrdScore(const int sock);
void sendOrdScore( const int sock);
STATO_CLIENT assegnaNome(PLAYER** new_player, const int sock);
