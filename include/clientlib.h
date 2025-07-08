#include "../include/all.h"
#include "../const/const.h"

typedef enum StatiUtente PLAYER_STATUS;
typedef struct temi_quiz TEMI;
enum StatiUtente{STARTPAGE, SCELTANOME, ATTIVO, SHOWSCORE, ENDQUIZ};

struct temi_quiz{

    char nome[NAME_MAX];
    bool attivo;
    uint16_t punteggio;
    
};

char tabellone_temi[BUFFER_SIZE] ="";
TEMI temi[NUM_THEME];




void printMenu();
void recvAll(const int);
bool recvRiga(const int sock, const int LEN_BUFFER);
PLAYER_STATUS sceltaMenu();
PLAYER_STATUS richiediNome(const int);
PLAYER_STATUS quiz(const int sock);
void recvNomeTemaScelto(const int sock);
void showScore();