#include "../include/all.h"
#include "../const/const.h"

typedef enum statiUtente PLAYER_STATUS;
typedef enum statiRecv RECV_STATUS;

typedef struct temi_quiz TEMI;
enum statiUtente {STARTPAGE, SCELTANOME, ATTIVO, SHOWSCORE, ENDQUIZ};

enum statiRecv { RECV_SCORE, RECV_QUESTION, RECV_END};

struct temi_quiz{

    char nome[NAME_MAX];
    bool finito;
    uint16_t punteggio;
    
};

char tabellone_temi[BUFFER_SIZE] ="";
TEMI temi[NUM_THEME];




void printMenu();
void recvAll(const int);
RECV_STATUS recvRiga(const int sock, const int LEN_BUFFER);
PLAYER_STATUS sceltaMenu();
PLAYER_STATUS richiediNome(const int);
bool scegliTema(const int sock, int* i);
void printTemi();