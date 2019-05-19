#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <netinet/in.h> 
#include <fcntl.h>
#include <mysql/mysql.h>

#define SIZE 100
#define PORT 8080 
#define SERVERADDRES struct sockaddr 
/*
int authenticateUser(char* uandp)
{
	int hasUser = 0;
	hasUser = connectToDB(uandp);
	return hasUser;	
}

int connectToDB(char uandp[]) {
	char username[100];
	char password[100];
	char query[300];

	bzero(username, 100);
	bzero(password, 100);
	bzero(query, 300);

	sscanf(uandp, "%s %s", username, password);

	MYSQL *con = mysql_init(NULL);
  	if (con == NULL) 
  	{
      		printf("Failed to establish connection to the database!");
      		exit(1);
  	}

  	if (!(mysql_real_connect(con, "localhost", "root", "Legendary517.", 
          	"LoginInfo", 3306, NULL, 0))) 
  	{
      		printf("Failed to establish connection with given credentials!");
      		mysql_close(con);
      		exit(1);
  	}  	


  	if (mysql_query(con, "Select username, password from users")) 
  	{
      		printf("Wrong query!");
      		mysql_close(con);
      		exit(1);
  	}

	MYSQL_RES *result = mysql_store_result(con);
  
  	if (result == NULL) 
  	{
      		finish_with_error(con);
  	}

  	int num_fields = mysql_num_fields(result);

  	MYSQL_ROW row;

	int hasUser = 0;

  	while ((row = mysql_fetch_row(result))) 
  	{ 
      		 
          	if(strcmp(row[0], username) == 0 && strcmp(row[1], password) == 0)
		{
			hasUser = 1;
			break;
		}	 
  	}

  	mysql_close(con);
  	//exit(0);

	return hasUser;
}*/
   int serverSocket, connectionSocket;
   int size, i;
   int fd;

   char buff[SIZE], command[5], filename[20];

   struct sockaddr_in serveraddr, client;
   struct stat fileStatus;

int main() 
{

  connectSocket();	
/*
	char uandp[300];
	int hasUser = 0;

	for(;;) {
	  bzero(uandp, 300);
	  recv(connfd, &uandp, sizeof(uandp), 0);
	  hasUser = authenticateUser(uandp);
	  bzero(buff, 100);
	  if (hasUser == 1) {
		strcpy(buff, "Yes");
		send(connfd, &buff, sizeof(buff), 0);
		break;
		}
	   else {
	   strcpy(buff, "No");
	   send(connfd, &buff, sizeof(buff), 0);
	   } 
	}*/

   i = 1;
   for(;;)
   {
	recv(connectionSocket, buff, 100, 0);
	sscanf(buff, "%s", command);

	if(!strcmp(command, "get"))
	{
	   getFileFromServer();
	}

	else if (!strcmp(command, "put"))
	{
	   putFileOnServer();
	}

	else if(!strcmp(command, "ls"))
	{
	   listDirectoryContent();
	}

	else if(!strcmp(command, "cd"))
	{
	   changeDirectory();
	}

   }

}

void connectSocket()
{
   int length;

   serverSocket = socket(AF_INET, SOCK_STREAM, 0);
   if (serverSocket == -1)
   {
	printf("Creating a socket failed\n");
	exit(0);
   }

   else
   {
	printf("Socket created sucessfully\n");
   }

   bzero(&serveraddr, sizeof(serveraddr));

   serveraddr.sin_family = AF_INET;
   serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
   serveraddr.sin_port = htons(PORT);

   if ((bind(serverSocket, (SERVERADDRES*)&serveraddr, sizeof(serveraddr))) != 0)
   {
		printf("Binding failed\n");
		exit(0);
   }

   else
   {
	printf("Binding sucessfull\n");
   }

   if ((listen(serverSocket, 5)) != 0)
   {
	printf("Listening faild\n");
	exit(0);
   }

   else
   {
	printf("Listening\n");
   }
   length = sizeof(client);

   connectionSocket = accept(serverSocket, (SERVERADDRES*)&client, &length);
   if (connectionSocket < 0)
   {
	printf("Server fails to accept\n");
	exit(0);
   }

   else
   {
	printf("Server accepts connection\n");
   }

}

void getFileFromServer()
{
   sscanf(buff+strlen(command), "%s", filename);
   stat(filename, &fileStatus);
   fd = open(filename, O_RDONLY);
   size = fileStatus.st_size;

   if(fd == -1)
   {
	size = 0;
   }

   send(connectionSocket, &size, sizeof(int), 0);
   if(size)
   {
	sendfile(connectionSocket, fd, NULL, size);
   }

}

void putFileOnServer()
{
   int status = 0;
   char *f;

   sscanf(buff+strlen(command), "%s", filename);
   recv(connectionSocket, &size, sizeof(int), 0);

   i = 1;
   for(;;)
   {
	fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
	if(fd == -1)
	{
	   sprintf(filename + strlen(filename), "%d", i);
	}

	else
	{
	   break;
	}

   }
   f = malloc(size);
   recv(connectionSocket, f, size, 0);

   status = write(fd, f, size);
   close(fd);
   send(connectionSocket, &status, sizeof(int), 0);

}

//chagned from contents to content
void listDirectoryContent()
{
   system("ls >contents.txt");
   i = 0;
   stat("contents.txt",&fileStatus);
   size = fileStatus.st_size;
   send(connectionSocket, &size, sizeof(int), 0);

   fd = open("contents.txt", O_RDONLY);
   sendfile(connectionSocket, fd, NULL, size);
}

void changeDirectory()
{
   int status;

   if (chdir(buff + 3) == 0)
   {
	status = 1;
   }
   else
   {
	status = 0;
   }
   send(connectionSocket, &status, sizeof(int), 0);
}
