#include "all.h"
#include "../const/const.h"


typedef struct theme THEMES;
typedef struct player PLAYERS;
typedef char* question;

uint16_t NUM_THEMES  = 2;
uint16_t NUM_PLAYER  = 0;


struct theme{
    //nome tema
    char* name;
    //numero attuale di domande per tema
    uint16_t NUM_DOMANDE;
    //linked list delle domande
    question list_questions[MAX_DOMANDE];
    question list_answer[MAX_DOMANDE];
};
THEMES themes[MAX_THEME]; 



struct player{
    char* name;
    u_int16_t scores[MAX_THEME];
};
PLAYERS players[USER_MAX];


//carica le domande dal file di testo 1 ok, 0 errore
int loadQuestion(FILE* fd);

//carica le domande dal file di testo 1 ok, 0 errore
int loadAnswers(FILE* fd);

//il tema deve essere univoco per essere inseriro,
// +1 inserito correttamente , 0 se non inserito, -1 se già presente
int AddTheme(uint16_t themeNumber, char* question);

//la domanda deve essere univoca per essere inserita
// +1 inserito correttamente , 0 se non inserito, -1 se già presente
int AddQuestion(uint16_t themeNumber, char* question);

//
int AddUser();
