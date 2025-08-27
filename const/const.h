

//numero massimo di temi
#define NUM_THEME 2
#define THEME_MAX 100

//lughezza massima nomi giocatori e temi
#define NAME_MAX 20 

//lunghezza domande
#define Q_MAX 1000

//lunghezza risposte
#define A_MAX 30 

#define BUFFER_SIZE NUM_THEME*NAME_MAX
//dimensione del buffer utilizzato per show score
#define SCOREBUFF_SIZE ( NAME_MAX + 3 + (THEME_MAX / 10) )
#define NAME_RESULT 20

//ack e nack sul nome scelta dal client
#define ACK_NAME  "Risposta corretta"
#define NACK_NAME "Risposta errataaa"

//comandi utente
#define SHOW_SCORE "showscore"
#define END_QUIZ   "endquiz"

#define EOQ "domande terminate"
#define EOS "scores  terminati"


#define bool _Bool
#define false 0
#define true 1


#define THEME_PATH "/home/danilo/reti_pr/progetto-reti-informatiche/txt/temi.txt"
#define Q_PATH "../txt/domande.txt"
#define A_PATH "../txt/risposte.txt"
