#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

void dostuff(int); /* function prototype */
void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen, pid;
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    while (1)
    {
        newsockfd = accept(sockfd,
                           (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        if (pid == 0)
        {
            close(sockfd);
            dostuff(newsockfd);
            exit(0);
        }
        else
        {
            close(newsockfd);
        }
    }         /* end of while */
    return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff(int sock)
{
    //	printf("entered doStuff\n");//debug line
    int n;
    char buffer[256] = {0};
    char b[256];
    bzero(buffer, 256);
    bzero(b, 256);
    n = read(sock, buffer, 255);
    if (n < 0)
        error("ERROR reading from socket");
    //
    FILE *fp; //file pointer for file created
    //checks if user inputs command 2 for list
    if ('2' == buffer[0])
    {
        //puts all the output into a new file
        system("ls >> dummyFile.txt");
        fp = fopen("dummyFile.txt", "r");

        while (fscanf(fp, "%s ", b) != EOF)
        {
            n = write(sock, b, 255);
            n = write(sock, " ", 255);
            if (n < 0)
                error("fscanf failed to write to socket");
        }

        fclose(fp);
        system("rm dummyFile.txt");
        n = write(sock, "exit", 4);
    }

    //store function
    else if ('4' == buffer[0])
    {
        int c = 0;
        char *fileName;
        fileName = (char *)malloc(sizeof(256));
        printf("Got a request to store a file\n");
        system("sleep 5s");
        //asks client for name of file
        write(sock, "4", 20); //step 3
        //reads file name from socket
        while ('4' == buffer[0])
        {
            read(sock, buffer, 256);
            //opens a file with requested name
            fp = fopen(buffer, "w");
            //once fileName is caught, sends signal to client to stop
            write(sock, "1", 20); //step 6
        }
        //actual file transfer
        bzero(buffer, 255);
        while (strcmp(buffer, "exit") != 0)
        {

            bzero(buffer, 255);
            //read the data
            read(sock, buffer, 255);
            //send ack
            write(sock, "recieved", 20);
            //if end of file is reached, stop writing
            if (strcmp(buffer, "exit") == 0)
            {
                break;
            }
            fprintf(fp, "%s", buffer);
        }
        printf("Finished reading file\n\n");

        //closes file
        fclose(fp);
        free(fileName);
    }

    //retrieve function
    //client either sends valid file name or gets sent to "else"
    else if (NULL != fopen(buffer, "r"))
    {
        //file exists ok message
        write(sock, "ok", 10);
        fp = fopen(buffer, "r");
        read(sock, buffer, 255);
        //checks that input is valid file
        if (fp != NULL)
        {
            while (fscanf(fp, "%s", b) != EOF)
            {
                n = write(sock, b, 255);
                read(sock, buffer, 255);
                n = write(sock, " ", 255);
                read(sock, buffer, 255);
            }
        }
        printf("end of transfer\n");
        write(sock, "exit", 10);
        read(sock, buffer, 255);
        fclose(fp);
    }
    //user input fron retrieve is sent to server
    //if no valid file with that name, go here
    else
    {
        //file does not exist message to client
        write(sock, "exit", 10);
        printf("client wants a file that doesn't exist");
    }

    //check for more commands from same client
    bzero(buffer, 255);
    n = read(sock, buffer, 255);
    if (n < 0)
        error("ERROR reading from socket");
    printf("buffer: %s\n", buffer);
    if ('1' == buffer[0])
    {
        dostuff(sock);
    }
}
