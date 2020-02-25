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
	//list function
        else if (i == 2)
        {
		if (0 != sockfd) //only attempts list function is socket has been connected
            list(sockfd);
		else i = 0;  //if not connected, goes back to the start of the loop and picks a new command
        }
        else if (i == 3)
        {
            retrieve(sockfd);
        }
        else if (i == 4)
        {
            store(sockfd);
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
void list(int sockfd)

{
	//this is the file we write two
	FILE* fp;
	char buffer[255];
	//opens the file
	fp = fopen("gotThis.txt", "w");
	//send a command to the server "2"
	write(sockfd,"2", 18);
	//reads from socket and outputs to screen
	while ( read(sockfd, buffer, 255) > 0){
       //prints file names from socket
	printf("\n%s", buffer);
	
	fputs(buffer, fp);
	}
	//closes file
	fclose(fp);
	//removes unnecessary file that has list of server's files
	system("rm gotThis.txt");
    
}
/**
 * funciton to get a file from the server
 * */
void retrieve(int sockfd)
{
    printf("\nretrieve\n");

    FILE *fp;
    char buffer[256];
    char *fileName;	//name of file to be read in.  256 char limit
    fileName = (char*) malloc(sizeof(255));
    int c;

    do {
	    //gets filename from upser
	printf("Enter file name to retrieve: \n");
	scanf("%s", fileName);
	//sends file request to server
	write(sockfd,  fileName, 256);
	//creates file
	fp= fopen(fileName, "w");
	//scans input from socket until no more info is sent
	while (read (sockfd, buffer, 256) > 0){
		fputs(buffer, fp);
	}

	c = fgetc(fp);//checks if last character in file is empty
//	if (c ==EOF){
//	free(fp); //deletes empty file
	}
    //if last character in file indicates the end of the file, it stops reading the file
    while (c !=EOF || '3'== buffer[0]);
	fclose(fp);
	//memory management for file name and buffer
    free(fileName);
    }	    

    /**
     * store function moves file from client to server
     * */
void store(int sockfd)
{
	FILE *fp;
	char buffer[255];
    printf("\nEnter name of file to store: \n");
    scanf("%s", buffer);
    
	//if user passes a valid file name, send the file
	if (NULL != fopen(buffer, "r")){
		fp = fopen(buffer, "r");}
		//sends a message to the server to go to the 'store' option
		write(sockfd, "4", 20);
		buffer[0]='0';
		//keeps reading from the socket until the server is ready to recieve a file.
		while ('0' == buffer[0]){
			read(sockfd, buffer, 255);
		}
		//writes sends the file name to the server
		write(sockfd, buffer, 255);
		//slight pause
		system("sleep 1s");
		//puts contents of file into socket
		while (fscanf(fp, "%s ", buffer) != EOF){
			write(sockfd, buffer, 255);
			write(sockfd, " ", 20);
		}
	//	write(sockfd, EOF, 255);
}
void quit()
{
    printf("\nExiting Now\n");
    exit(0);
}
