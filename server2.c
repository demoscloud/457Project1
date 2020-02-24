/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
   gcc server2.c -lsocket
*/
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

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     while (1) {
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) 
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             dostuff(newsockfd);
             exit(0);
         }
         else close(newsockfd);
     } /* end of while */
     return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff (int sock)
{
//	printf("entered doStuff\n");//debug line
   int n;
   char buffer[256];
   char b[255];
   bzero(buffer,256);
   n = read(sock,buffer,255);
   if (n < 0) error("ERROR reading from socket");
   //start Olivia Code
   FILE* fp; //file pointer for file created 
   //checks if user inputs command 2 for list
   if ('2'== buffer[0]){
	   //puts all the output into a new file 
	   system("ls >> dummyFile.txt");
	   fp = fopen("dummyFile.txt", "r");
	  
	while(fscanf(fp, "%s ", b)!= EOF ){
		n = write(sock, b, 255);
		n = write(sock, " ", 255);	
		if (n<0) error("fscanf failed to write to socket");
	}	

	fclose(fp);
	system("rm dummyFile.txt");
   }
   //takes in a file name and if it's valid, returns the file to client
   else if (NULL != fopen(buffer, "r")){
	fp = fopen(buffer, "r");
	//checks that input is valid file
	if (fp != NULL){
		while(fscanf(fp, "%s ", b) != EOF){
			n = write(sock, b, 255);
			n = write(sock, " ", 255);
		}				
	}
      	fclose(fp);
   }
   //store function takes files from client and stores on server side
   else{
	fp = fopen("clientFile.txt", "w");
	int c;
	do{
		read(sock, buffer, 255);
		fputs(buffer, fp);
		fputs(" ", fp);
		c = fgetc(buffer);
	}
	while (c != EOF);
	fclose(fp);
   }
   //end Olivia code
}
