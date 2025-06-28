#include "serverlib.h"



NODE* caricaRiga(FILE *fd)
{
    char str_riga[Q_MAX];
    NODE* testa = NULL;
    NODE* coda = NULL;;

    while(fgets(str_riga, sizeof(str_riga), fd)!=NULL){
        
        //elimino il ritorno a capo alla fine della stringa
        str_riga[strcspn(str_riga, "\n")] = '\0';
        char str_spazi_bianchi[Q_MAX];

        //controlla di presenza di righe di pure spazi bianchi
        strncpy(str_spazi_bianchi, str_riga, Q_MAX-1);
        str_spazi_bianchi[strcspn(str_spazi_bianchi, " ")] = '\0';

        if(strlen(str_riga) == 0 ||  strlen(str_spazi_bianchi) == 0 )
            continue;
        
        NODE* new_node = (NODE*)malloc(sizeof(NODE));
        if (!new_node) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        
        // valori nodo
        strncpy(new_node->testo, str_riga, Q_MAX-1);
        new_node->next = NULL;

        //printf("- %s",new_node->testo);

        if (new_node->testo == NULL) {
            perror("strdup fallita");
            return NULL;
        }

        if(testa == NULL)
            testa = coda = new_node;
        else{
            coda->next = new_node;
            coda = new_node;
        }
        
    }

    return testa;
}

void printLista(NODE* lista){

    printf("\n--- Lista ----\n");
    for (NODE* nodo_stampa = lista; nodo_stampa != NULL ;nodo_stampa = nodo_stampa->next)
    {
        printf("- %s\n",nodo_stampa->testo);
    }
    
}

bool caricaDomande(size_t num_tema, THEME *tema)
{
    FILE* fd;
    char file_name[100];
    snprintf(file_name, sizeof(file_name), "txt/domande%ld.txt", num_tema+1);
    //printf("%s\n",file_name);

    fd = fopen(file_name, "r"); 
    if (!fd) {
        perror("Errore nell'apertura di domande.txt: ");
        return 1;
    }

    tema->lista_domande = caricaRiga(fd);

    fclose(fd);
    return true;
}

bool caricaRisposte(size_t num_tema, THEME *tema)
{
    FILE* fd;
    char file_name[100];
    snprintf(file_name, sizeof(file_name), "txt/risposte%ld.txt", num_tema+1);

    fd = fopen(file_name, "r"); 
    if (!fd) {
        perror("Errore nell'apertura di un file risposte.txt: ");
        return false;
    }

    tema->lista_risposte = caricaRiga(fd);

    fclose(fd);
    return true;
}

bool caricaTemi(THEME *temi)
{
    FILE* fd  = fopen(THEME_PATH,"r");
    char tema_nome[Q_MAX];

    THEME* new_tema;

    if (!fd) {
        perror("Errore nell'apertura di temi.txt\n");
        return 1;
    }
    
    for(size_t i= 0; i< NUM_THEME && fgets(tema_nome, sizeof(tema_nome), fd)!=NULL; i++) {
        
        new_tema = &temi[i];
        //@todo #6 i nomi dei temi andranno liberati alla fine
        new_tema->name = strdup(tema_nome);

        if (new_tema->name == NULL) {
            perror("strdup fallita");
            return 1;
        }
        
        //printf("nome del tema: %s\n", new_tema->name);
        caricaDomande(i,new_tema);
        printLista(new_tema->lista_domande);

        caricaRisposte(i,new_tema);
        printLista(new_tema->lista_risposte);


        //caricaRisposte(i,new_tema);
    }
    
    fclose(fd);
    return 0;
}

