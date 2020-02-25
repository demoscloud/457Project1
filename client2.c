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
	else if (1==i){
		printf("Already connected\n");
	}
	else if(5 == i){
		quit();
	} else if (0 == sockfd){
		printf("please connect first");
	}
	//list function
        else if (i == 2 )
        {
            list(sockfd);
        }
        else if (i == 3)
        {
            retrieve(sockfd);
        }
        else if (i == 4)
        {
            store(sockfd);
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
    while (n ==-1 )//debug removed && i<5 condition
    {
        printf("1.Connect\n2.List\n3.Retrieve\n4.Store\n5.Quit\n");
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


/**
 * Number 2 List command
 *
 * Client sends request to server for the list of files available in the server's directory
 * The file names are printed on screen
 **/
void list(int sockfd){
	//this is the file we write to
	FILE* fp;
	char buffer[255];
	//opens the file
	fp = fopen("gotThis.txt", "w");
	//send a command to the server "2"
	write(sockfd,"2", 18);
	//reads from socket and outputs to screen
	while ( read(sockfd, buffer, 255) > 0){
       		//looping through menue
		
	//prints file names from socket
	printf("\n%s", buffer);
	
	fputs(buffer, fp);
	if (strcmp(buffer, "exit") ==0) break;
	}
	//closes file
	fclose(fp);
	//removes unnecessary file that has list of server's files
	system("rm gotThis.txt");
       
	//menu management
	continuing(sockfd);
}
/**
 * Number 3 Retrieve command
 *
 * function to get a file from the server and store it on the client side
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
	}
    //if last character in file indicates the end of the file, it stops reading the file
    while (c !=EOF || '3'== buffer[0]);
	
	fclose(fp);
//	continuing(sockfd);    
}	    

    /**
     * Number 4 Store command
     *
     * store function moves file from client to server
     * if the file doesn't exist, nothing happens
     * */
void store(int sockfd)
{
	FILE *fp; //file being read from
	char buffer[256] = {0};
	char *fileName;
	fileName = (char*) malloc(sizeof(255));
    
	//asks client what file they want to store
	printf("\nEnter name of file to store: \n");
	//scans user input into fileName
	scanf("%s", fileName);
    
	//if user passes a valid file name, client sends the file
	if (NULL != fopen(fileName, "r")){
		//opens the file
		fp = fopen(fileName, "r");
		//sends a message to the server to go to the 'store' option
		write(sockfd, "4", 20);	//step 1
		buffer[0]='0';
		//keeps reading from the socket until the server is ready to recieve a file.
		while ('0' == buffer[0]){//step 4
			read(sockfd, buffer, 256);
		}
		//writes sends the file name to the server
		while ('4' == buffer[0]){
		write(sockfd, fileName, 255);//step 5
		read(sockfd, buffer, 255);//step 7
		}
		//puts contents of file into socket
		while (fscanf(fp, "%s ", buffer) != EOF){//step 8
			write(sockfd, buffer, 256);
			write(sockfd, " ", 20);
		}
		fclose(fp);
		} else printf("File not found\n");
			
		
	free(fileName);
	//menu management
//	continuing(sockfd);
}

void quit()
{
    printf("\nExiting Now\n");
    exit(0);
}


//menu management
void continuing(int sock){
	printf("Continuing called\n");
	char str[5] = "hi";
	int n =0;
	int i =0;
	while (n !=1 && n != 2 && i != 5){
		printf("\n Continue?\n1. YES\n 2. NO\n");
		scanf("%s", &str);
		n = atoi(str);
		i++;
	}
	write(sock, str, strlen(str));
	if (n!= 1){
		printf("\nQuit Selected\n");
			quit();
	}
}

