#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int selectCommand();
int connectServer();
void list(int sockfd);
void retrieve(int sockfd);
void store(int sockfd);
void quit();
void continuing(int sock);
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
        //only sets socket if user input is 1 and sockfd hasn't been connected prior
        if (i == 1 && 0 == sockfd)
        {
            sockfd = connectServer();
        }
        //if socket is connected, don't do anything
        else if (i == 1)
        {
            printf("Already Connected\n");
        }
        //can quit, even without socket connected
        else if (i == 5)
        {
            quit();
        }
        //to do anything except connect or quit, socket must be connected
        else if (sockfd == 0)
        {
            printf("\nPlease connect first\n");
        }
        //list function
        else if (i == 2)
        {
            list(sockfd);
        }
        //retrieve function
        else if (i == 3)
        {
            retrieve(sockfd);
        }
        //store function
        else if (i == 4)
        {
            store(sockfd);
        }
    }
    printf("\nerror executing command\n");
    return 0;
}

//offers menu of options and retrieves user input
int selectCommand()
{
    int n = -1;
    int i = 0;
    char str[5];
    //while user input is invalid
    //only allows 5 invalid entries
    while (n == -1 && i < 5)
    {
        printf("\n1.Connect\n2.List\n3.Retrieve\n4.Store\n5.Quit\n");
        printf("Input Command:");
        scanf("%s", &str);
        n = atoi(str);
        if (n <= 0 || n > 5)
        {
            printf("\nInvalid Input, try again\n");
            n = -1;
        }
        i++;
    }
    //returns integer of selected command
    return n;
}

//connects to the server and returns the socket number
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
    if (server == NULL)
    {
        error("ERROR no such host\n");
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portNum);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    return sockfd;
}

//list function, client side
void list(int sockfd)
{
    printf("\nList:\n");
    //this is the file we write to

    FILE *fp;
    char buffer[255];
    //opens the file
    fp = fopen("gotThis.txt", "w");
    //send a command to the server "2"
    write(sockfd, "2", 18);
    //reads from socket and outputs to screen
    while (read(sockfd, buffer, 255) > 0)
    {
        //prints file names from socket
        if ((strcmp(buffer, "exit")) == 0)
        {
            break;
        }
        printf("\n%s", buffer);
        fputs(buffer, fp);
    }
    //closes file
    fclose(fp);
    //removes unnecessary file that has list of server's files
    system("rm gotThis.txt");
    continuing(sockfd);
}

/**
 * funciton to get a file from the server
 * */
void retrieve(int sockfd)
{
    printf("\nRetrieve:\n");

    FILE *fp;
    char buffer[256];
    char *fileName; //name of file to be read in.  256 char limit
    fileName = (char *)malloc(sizeof(255));
    int c;

    
    printf("Enter name of file to retrieve:\n");
    scanf("%s", fileName);
    write(sockfd, fileName, 256);
    bzero(buffer, 255);
    read(sockfd, buffer, 255);

    //if server sends does not exist response
    if(strcmp(buffer, "exit") == 0){
        printf("File Does Not Exist\n");
    }
    //if the file does exist on the server
    else{
        fp = fopen(fileName, "w");
        write(sockfd, "rdy", 255);
        //reading file loop
        while(strcmp(buffer, "exit") != 0){
            bzero(buffer, 255);
            //get input
            read(sockfd, buffer, 255);
            //send ack
            write(sockfd, "ack", 10);
            if(strcmp(buffer, "exit") == 0){
                break;
            }
            fprintf(fp, "%s", buffer);
        }
    }

    fclose(fp);
    continuing(sockfd);
    free(fileName);
}

/****
* store function moves file from client to server
****/
void store(int sockfd)
{
    printf("\nStore:\n");
    FILE *fp;
    char buffer[256] = {0};
    char *fileName;
    fileName = (char *)malloc(sizeof(255));
    printf("\nEnter name of file to store: \n");
    scanf("%s", fileName);

    //if user passes a valid file name, send the file
    if (NULL != fopen(fileName, "r"))
    {
        fp = fopen(fileName, "r");
        char ack[20];
        //sends a message to the server to go to the 'store' option
        write(sockfd, "4", 20); //step 1
        buffer[0] = '0';
        //keeps reading from the socket until the server is ready to recieve a file.
        while ('0' == buffer[0])
        { //step 4
            read(sockfd, buffer, 256);
        }
        //writes sends the file name to the server
        while ('4' == buffer[0])
        {
            write(sockfd, fileName, 255); //step 5
            read(sockfd, buffer, 255);    //step 7
        }
        //puts contents of file into socket
        while (fscanf(fp, "%s", buffer) != EOF)
        { //step 8
            //printf("\nwriting buffer: %s\n", buffer);
            write(sockfd, buffer, 256);
            read(sockfd, ack, 20);
            //printf("\nAck: %s\n", ack);
            write(sockfd, " ", 20);
            read(sockfd, ack, 20);
        }
        fclose(fp);
        //exit procedure
        write(sockfd, "exit", 4);
        //printf("\ncontinuing\n");
        continuing(sockfd);
    }
    else
    {
        printf("invalid file\n");
    }
    free(fileName);
}
void quit()
{
    printf("\nExiting Now\n");
    exit(0);
}

void continuing(int sock)
{
    char str[5] = "hi";
    int n = 0, i = 0;
    while (n != 1 && n != 2 && i != 5)
    {
        printf("\nContinue?\n1.YES\n2.NO\n");
        scanf("%s", &str);
        n = atoi(str);
        i++;
    }
    write(sock, str, strlen(str));
    if (n != 1)
    {
        printf("\nQuit Selected");
        quit();
    }
}