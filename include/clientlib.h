#include "../include/all.h"
#include "../const/const.h"

typedef enum StatiUtente PLAYER_STATUS;
enum StatiUtente{STARTPAGE, SCELTANOME, ATTIVO, SHOWSCORE, ENDQUIZ};

void printMenu();
PLAYER_STATUS sceltaMenu();
PLAYER_STATUS richiediNome(const int);
PLAYER_STATUS quiz(const int sock);
void recvNomeTemaScelto(const int sock);
void showScore();