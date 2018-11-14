//NAME: Lauren Fromm
//EMAIL: laurenfromm@gmail.com
//ID: 404751250

#include <stdio.h>
#include <termios.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <netdb.h>
#include "zlib.h"

#define BUFFER_SIZE 256

#define TIMEOUT 0


struct termios old, saved;


pid_t pid;

int port = 0;
int portNum;

int logFlag = 0;
char* logFile;
int logFilefd;
int compressFlag = 0;
int debug = 0;
int shell = 0;

struct pollfd fds[2]; //create pollfd struct      
struct sockaddr_in client;
int socketfd;


z_stream defstream;
z_stream infstream;


void sig_handler(int signo)
{
  if (signo == SIGINT && shell ==1){
    kill(pid,SIGINT);
  }
  if(signo ==SIGPIPE)
    exit(0);
}


void openConnection()
{
  if (debug)
    {
      fprintf(stderr, "opening connection with sockets");
    }
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0 )
    {
      fprintf(stderr, "Error opening socket");
      exit(1);
    }
  struct hostent* hostname;
  hostname = gethostbyname("localhost");
  if (hostname == NULL)
    {
      fprintf(stderr, "Error getting host name");
      exit(1);
    }
  memset((char*) &client, 0, sizeof(client));

  //client.sin_addr.s_addr = inet_addr("127.0.0.1"); //local host
  client.sin_family = AF_INET;
  memcpy((char *) &client.sin_addr.s_addr, (char*) hostname->h_addr, hostname->h_length);
  client.sin_port = htons(portNum);


  if(connect(socketfd, (struct sockaddr *) &client, sizeof(client)) == -1)
    {
      fprintf(stderr, "Error conecting to socket");
      exit(1);
    }
}

void setupCompression()
{
  if (debug)
    fprintf(stderr,"In SetupCompression");
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;


}
void compressFunc(char* a, char* b)
{
  if(debug)
    fprintf(stderr, "In compressFunc");
  defstream.avail_in = (uInt)strlen(a)+1; // size of input, string + terminator                                                                                                                             
  defstream.next_in = (Bytef *)a; // input char array                                                                                                                                                       
  defstream.avail_out = (uInt)sizeof(b); // size of output                                                                                                                                                  
  defstream.next_out = (Bytef *)b; // output char array                                                                                                                                                     
  deflateInit(&defstream, Z_BEST_COMPRESSION);
  deflate(&defstream, Z_FINISH);
  deflateEnd(&defstream);
}

void setupDecompression()
{
  if (debug)
    fprintf(stderr, "In setupDecompression");
  infstream.zalloc = Z_NULL;
  infstream.zfree = Z_NULL;
 infstream.opaque = Z_NULL;
}
void decompress(char* b, char* c)
{
  if(debug)
    fprintf(stderr, "In decompress");
  infstream.avail_in = (uInt)sizeof(b); // size of input                                                                                                                                                      
  infstream.next_in = (Bytef *)b; // input char array                                                                                                                                                         
  infstream.avail_out = (uInt)sizeof(c); // size of output                                                                                                                                                    
  infstream.next_out = (Bytef *)c; // output char array                                                                                                                                                       

  inflateInit(&infstream);
  inflate(&infstream, Z_NO_FLUSH);
  inflateEnd(&infstream);



}

void resetAttributes()
{
  tcsetattr(STDIN_FILENO, TCSANOW, &saved); //set the attributes with the updates      ~
}

void attributes()
{
  if(debug)
    fprintf(stderr, "In attributes() \n");
  tcgetattr(STDIN_FILENO, &old); //get the current attributes
  tcgetattr(STDIN_FILENO, &saved); //save current settings;
  atexit(resetAttributes);
  old.c_iflag = ISTRIP;/* only lower 7 bits*/
  old.c_oflag = 0;/* no processing*/
  old.c_lflag = 0;/* no processing*/

  if(tcsetattr(STDIN_FILENO, TCSANOW, &old) == -1) //set the attributes with the updates
    {
      fprintf(stderr, "Error setting attributes");
      exit(1);
    }
}



void doPolls()
{                                      
  if(debug)
    fprintf(stderr, "In doPolls");

  fds[0].fd = 0; //set first fd for stdin                                     
  fds[0].events = POLLIN | POLLHUP | POLLERR;
 
  fds[1].fd = socketfd; //read from shell                                  
  fds[1].events = POLLIN | POLLHUP | POLLERR;
}
void withShell()
{
  doPolls(); 
  if(debug)
    fprintf(stderr, "Polls created \n");
  
  char buf;
  while (1) //keep looping through                                                  
    {
      
      if( poll(fds, 2, 0) == -1) //wait for input sources               
	{
	  fprintf(stderr, "Error polling");
	  exit(1);
	}


      if(fds[0].revents & POLLIN) //if there is info coming in                     
	{
	  if(debug)
	    fprintf(stderr, "Input poll1 \n");
	  int sizeOfRead = read(0, &buf, sizeof(char));
	  if (debug)
	    fprintf(stderr, "sizeofRead = %d", sizeOfRead);
	  if(sizeOfRead ==0)
	    {
	      close(socketfd);
	      exit(0);
	    }
	  //if (sizeOfRead == -1)

	  // {
	  //   fprintf(stderr, "Error reading input");
	  //   exit(1);
	  // }
	  if( buf == '\r' || buf == '\n') //if there is <cr> or <lf>           
	    {
	      if(debug)
		fprintf(stderr, "<cr> or <lf> recieved \n");
	      char *temp="\r\n";
	      write(1, temp, 2*sizeof(char)); //write <cr><lf> to stdout               
	      if (compressFlag)
		{
		  compressFunc(&buf, &buf);
		}
	      if (logFlag)
		{
		  if(debug)
		    fprintf(stderr, "Logging sent");
		  char tempWriteOut[14] = "SENT 1 bytes: ";
		 
		  write(logFilefd, tempWriteOut, sizeof(tempWriteOut));
		  write(logFilefd, &buf, sizeof(char));
		  write(logFilefd, "\n", sizeof(char));
		  
		}
	      if(debug)
		fprintf(stderr, "got here 1");
	      write(socketfd, &buf, sizeof(char));
	      continue;
	    }
	  else
	    {
	      write(1, &buf, sizeof(char));
	      if(compressFlag)
		compressFunc( &buf, &buf);
	      if(logFlag)
		{
		  if(debug)
		    fprintf(stderr, "Logging sent");
		  char tempWriteOut[14] = "SENT 1 bytes: ";

                  write(logFilefd, tempWriteOut, sizeof(tempWriteOut));
                  write(logFilefd, &buf, sizeof(char));
                  write(logFilefd, "\n", sizeof(char));

		}
	      if(debug)
		fprintf(stderr, "got here");
		     
	      write(socketfd, &buf, sizeof(char));
	      
	    }
	  
	}


      if(fds[1].revents & POLLIN) //if the second pipe should input                 
	  {
	    if (debug)
	      fprintf(stderr, "Input poll2 \n");

	    int sizeOfRead =  read(socketfd, &buf, sizeof(char));
	    if (sizeOfRead == 0)
	      {
        	exit(0);
	      }
	    if (debug && buf == 0x03)
	      {
		fprintf(stderr, "Problem");
	      }
	    if (logFlag == 1 && buf != 0x03)
	      {
		if(debug)
		  fprintf(stderr, "Logging recieved bytes");
		char tempWriteOut[18] = "RECEIVED 1 bytes: ";
		write(logFilefd, tempWriteOut, sizeof(tempWriteOut));
		write(logFilefd, &buf, sizeof(char));
		write(logFilefd, "\n", sizeof(char));
	      }
	    if (compressFlag)
	      {
		decompress(&buf, &buf);
	      }
	    
	    if (buf == '\n' || buf == '\r') //if there is a <lf>
	      {
		char *temp="\r\n";
	        write(1, temp, 2* sizeof(char)); //write to std out <cr><lf>              
	      }
	    else
	      write(1, &buf, sizeof(char)); //otherwise write to stdout

	  }


      if(fds[1].revents & POLLHUP || fds[0].revents & POLLHUP)
	  {
	    if(debug)
	      fprintf(stderr, "POLLHUP&POLLERR");
	    exit(0);
	  }
      }

}





int main (int argc, char * argv[])
{
  struct option longopts[] = {
    { "port", required_argument, 0, 'p'},
    { "log", optional_argument, 0, 'l'},
    { "compress", no_argument, 0, 'c'},
    { "debug", no_argument, 0, 'd'},
    { 0,0,0,0}
  };
  int c;
  while((c=getopt_long(argc, argv, "", longopts, NULL)) != -1){
    switch(c){
    case 'p':
      port = 1;
      portNum = atoi(optarg);
      break;
    case 'l':
      logFlag = 1;
      logFile = optarg;
      if((logFilefd = creat(logFile, 0666)) == -1)
	{
	  fprintf(stderr,"Error creating log file");
	  exit(1);
	}
      break;
    case 'c':
      compressFlag = 1;
      setupCompression();
      setupDecompression();
      break;
    case 'd': 
      debug =1;
      break;
    default:
      fprintf(stderr, "Argument not recognized: %d \n", c);
      exit(1);
    }
  }

  if (debug && logFlag)
    fprintf(stderr, "Caught log flag");
  if (debug && compressFlag)
    fprintf(stderr, "Caught compress flag");
  if(debug && port)
    fprintf(stderr, "Caught port flag");
  if(!isatty(STDIN_FILENO)) //make sure we can read from stdin                           
    {
      fprintf(stderr, "Error reading from stdin \n");
      exit(1);
    }

  attributes();

  if (debug)
    fprintf(stderr, "Termios created \n");
  


  openConnection();


  withShell();
  


  if(debug)
    fprintf(stderr, "Ending proccess, resetting attributes \n");


  exit(0);
}
