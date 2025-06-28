#include "serverlib.h"



bool caricaDaFile(FILE *fd, QUESTION *lista)
{

    return false;
}

bool caricaDomande(int num_tema, QUESTION *lista_domande)
{

    return false;
}

bool caricaRisposte(const char* path_name, int num_tema, QUESTION *lista_domande)
{
    const char* punto = strrchr(path_name,'.');

    size_t old_len =  punto - path_name;

    char num_str[10];  b 
    itoa(num_tema,num_str,20);
    size_t new_len =  old_len + strlen(num_str) +  strlen(punto);


    return false;
}

bool caricaTemi(THEME *temi)
{
    FILE* fd  = fopen(THEME_PATH,'r');
    char domanda[Q_MAX];
    THEME* new_tema ;
    if (!fd) {
        perror("Errore nell'apertura di temi.txt\n");
        return 1;
    }
    for(int i= 0; i< NUM_THEME && fgets(domanda, sizeof fd, fd)!=NULL; i++) {
        new_tema = &temi[i];
        strcpy(new_tema->name, strdup(domanda)); 
        free(domanda);
        const char * question = Q_PATH;
        caricaDomande(i,new_tema->lista_domande);
        caricaRisposte(i,new_tema->lista_domande);

        new_tema->lista_domande = NULL;  
    }

    fclose(fd);
    return 0;
}

