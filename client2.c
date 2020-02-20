#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

int selectCommand();
void connectServer();
void list();
void retrieve();
void store();
void quit();
void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]){
    int i = 0;
    i = selectCommand();
    if(i == 1){
        connectServer();
    }
    else if(i == 2){
        list();
    }
    else if(i == 3){
        retrieve();
    }
    else if(i == 4){
        store();
    }
    else if(i == 5){
        quit();
    }
    else{
        printf("\nerror executing command\n");
        exit(0);
    }
    return 0;
}

int selectCommand(){
    int n = -1;
    int i = 0;
    char str[5];
    while(n == -1 && i < 5){
        printf("1.Connect\n2.List\n3.Retrieve\n4.Store\n5.Quit\n");
        printf("Input Command:");
        scanf("%s", &str);
        n = atoi(str);
        if(n == 0 || n > 5){
            printf("\nInvalid Input, try again\n");
            n = -1;
        }
        i++;
    }
    
    return n;
}

void connectServer(){
    char host[20];
    char port[10];
    int portNum = -1;
    int sockfd;
    printf("\nInput Hostname:");
    scanf("%s", &host);
    printf("Input Port Number:");
    scanf("%s", &port);
    portNum = atoi(port);
    if(portNum <= 0){
        printf("\nInvalid Port Number\n");
    }
    /*
    else{
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0)
            error("Error opening socket")
    }
    */
}
void list(){
    printf("\nlist\n");
}
void retrieve(){
    printf("\nretrieve\n");
}
void store(){
    printf("\nstore\n");
}
void quit(){
    printf("\nquit\n");
}