// Write CPP code here 
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <fcntl.h>
#define SIZE 100 
#define PORT 8080 
#define SERVERADDRESS struct sockaddr 
/*
int userLogin(int sockfd) {
	char username[100];
	char password[100];
	char buff[100];
	char uandp[300];
	int hasUser = 0;
	for(int i = 0; i < 3; i++)
	{
		printf("Enter username: ");
		scanf("%s", username);
		printf("Enter password: ");
		scanf("%s", password);
		
		bzero(uandp, 300);

		strcat(uandp, username);
		strcat(uandp, " ");
		strcat(uandp, password);
		
		write(sockfd, uandp, sizeof(uandp));

		bzero(buff, 100);
		read(sockfd, buff, sizeof(buff));
		
		if(strcmp(buff, "Yes") == 0)
		{
			hasUser = 1;
			break;
		}
	 }
return hasUser;

}
/*
void listFiles(int sockfd) {
	char buff[MAX];
	
}*/

   int serverSocket;
   int fd;
   int size;

   char command[5], filename[20];
   char buff[SIZE];

   struct sockaddr_in serveraddr;
   struct stat fileStatus; 

int main() 
{ 
	
   int status;
   int choice;
	
   socketConnect();

   //if (userLogin(serverSocket) == 1) {
	for(;;)
	{
	printf("\n");
	   printf(" 1: Get file from server\n 2: Put file on server\n 3: List current directory content\n 4: Change directory\n 5: Exit\n");
	   printf("\n");
	   printf("Choose an option: ");

	   scanf("%d",&choice);
	   switch(choice)
	   {
		case 1:
		   getFileFromServer();
		   break;

		case 2:
		   putFileOnServer();
		   break;

		case 3:
		   listFiles();
		   break;

		case 4:
		   changeDirectory();
	  	   break;

		case 5:
		   close(serverSocket);
		   printf("Exiting...\n");
		   exit(0);
	   }
}
   //}
	/*else {
	close(serverSocket);
	printf("Maximum attempts for login reached!\n");
	printf("Closing....\n");
	exit(1);
	}*/
 } 

void socketConnect()
{

   serverSocket = socket(AF_INET, SOCK_STREAM, 0);
   if (serverSocket == -1)
   {
	printf("Failed to create a socket\n");
	exit(0);
   }

   else
   {
	printf("Socket creation sucessfull\n");
   }

   bzero(&serveraddr, sizeof(serveraddr));

   serveraddr.sin_family = AF_INET;
   serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
   serveraddr.sin_port = htons(PORT);

   if (connect(serverSocket, (SERVERADDRESS*)&serveraddr, sizeof(serveraddr)) != 0)
   {
	printf("Connection failed\n");
	exit(0);
   }

   else
   {
	printf("Connected\n");
   }

}

void getFileFromServer()
{
   char *f;
   int i = 1;

   printf("\nEnter filename to get: ");
   scanf("%s", filename);
   strcpy(buff, "get ");
   strcat(buff, filename);
   send(serverSocket, buff, 100, 0);

   recv(serverSocket, &size, sizeof(int), 0);
   if(!size)
   {
	printf("No file found\n\n");	   
   }

   f = malloc(size);
   recv(serverSocket, f, size, 0);

   for(;;)
   {
	fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
	if(fd == -1)
	{
	   sprintf(filename + strlen(filename), "%d", i);
	}
	else
	{
	   printf("\nFile transfer sucessfull");
	   break;
	}
   }

  /* write(fd, f, size, 0);
   close(fd);
   strcpy(buff, "cat ");
   strcat(buff, filename);
   system(buff);*/

}

void putFileOnServer()
{
   int status;

   printf("\nEnter file you wish to put on server: ");
   scanf("%s", filename);

   fd = open(filename, O_RDONLY);
   if(fd == -1)
   {
	printf("No file found\n\n");      
   }

   strcpy(buff, "put ");
   strcat(buff, filename);
   send(serverSocket, buff, 100, 0);

   stat(filename, &fileStatus);
   size = fileStatus.st_size;
   send(serverSocket, &size, sizeof(int), 0);
   sendfile(serverSocket, fd, NULL, size);

   recv(serverSocket, &status, sizeof(int), 0);
   if(status)
   {
	printf("File stored successfully\n");
   }

   else
   {
	printf("Failed to store file onto server\n");
   }

}

void listFiles()
{
   char *f;

   strcpy(buff, "ls");
   send(serverSocket, buff, 100, 0);

   recv(serverSocket, &size, sizeof(int), 0);
   f = malloc(size);

   recv(serverSocket, f, size, 0);
   fd = creat("content.txt", O_WRONLY);
   write(fd, f, size);

   close(fd);

   printf("\n");
   printf("Directory content:\n");

   system("cat content.txt");

}

void changeDirectory()
{
   int status;

   strcpy(buff, "cd");
   printf("\nEnter the name of the directory you wish to change: ");
   scanf("%s", buff + 3);
   send(serverSocket, buff, 100, 0);

   recv(serverSocket, &status, sizeof(int), 0);
   if (status)
   {
	printf("Directory changed\n");
	printf("Currently in %s\n", buff + 3);
   }

   else
   {
	printf("Failed to change directory\n");
   }
}
