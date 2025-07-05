#include "../include/all.h"

int main(int argc,const char *argv[]) 
{
    int sockD = socket(AF_INET, SOCK_STREAM, 0);
    //porta di default
    int porta = 3456;
    struct sockaddr_in servAddr; 
    
    if (argc == 2) {
        porta = atoi(argv[1]);
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(porta);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    int connectStatus  = connect(sockD, (struct sockaddr*)&servAddr,
                  sizeof(servAddr));

    if (connectStatus == -1) {
        printf("Error...\n");
    }

    else {
        char strData[255];
        recv(sockD, strData, sizeof(strData), 0);
        printf("Message: %s\n", strData);
    }

    return 0;
}
