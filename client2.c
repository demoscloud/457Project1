#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int selectCommand();
int connectServer();
void list(int);
void retrieve();
void store();
void quit();
void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int i = 0;
    int sockfd = 0;
    while (i >= 0)
    {
        i = selectCommand();
        if (i == 1 && sockfd == 0)
        {
            sockfd = connectServer();
        }
        else if(i == 1){
            printf("\nAlready Connected\n");
        }
        else if (i == 2)
        {
            list(sockfd);
        }
        else if (i == 3)
        {
            retrieve();
        }
        else if (i == 4)
        {
            store();
        }
        else if (i == 5)
        {
            quit();
        }
    }
    printf("\nerror executing command\n");
    return 0;
}

int selectCommand()
{
    int n = -1;
    int i = 0;
    char str[5];
    while (n == -1 && i < 5)
    {
        printf("1.Connect\n2.List\n3.Retrieve\n4.Store\n5.Quit\n");
        printf("Input Command:");
        scanf("%s", &str);
        n = atoi(str);
        if (n == 0 || n > 5)
        {
            printf("\nInvalid Input, try again\n");
            n = -1;
        }
        i++;
    }

    return n;
}

int connectServer()
{
    char host[20];
    char port[10];
    int portNum = -1;
    int sockfd = 0;
    struct hostent *server;
    struct sockaddr_in serv_addr;
    //user input for port and host
    printf("\nInput Hostname:");
    scanf("%s", &host);
    printf("Input Port Number:");
    scanf("%s", &port);
    portNum = atoi(port);
    if (portNum <= 0)
    {
        error("\nInvalid Port Number\n");
    }
    //attempt connection
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("Error opening socket");
    server = gethostbyname(host);
    if(server == NULL){
        error("ERROR no such host\n");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portNum);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    return sockfd;
}
void list(int sock)
{
    FILE *rf;
    ssize_t len;
    int fsize;
    char size[255];
    char buffer[255];
    printf("\nlist\n");
    int n = 0;
    n = write(sock, "2", 1);
    if(n < 0){
        error("ERROR writing to socket\n");
    }
    bzero(size, 255);
    n = read(sock, size, 255);
    if(n < 0){
        error("ERROR reading from socket\n");
    }
    fsize = atoi(size);
    printf("\nsize = %d\n", fsize);
    //read data and write to file
    rf = fopen("output.txt","w");
    if(rf == NULL)
        error("failed to open file\n");
    while((fsize > 0) && ((len = recv(sock, buffer, 255, 0)) > 0)){
        printf("\nmessage recieved:   %s \n", buffer);
        fwrite(buffer, sizeof(char), len, rf);
        fsize -= len;
    }
    fclose(rf);
}
void retrieve()
{
    printf("\nretrieve\n");
}
void store()
{
    printf("\nstore\n");
}
void quit()
{
    printf("\nExiting Now\n");
    exit(0);
}