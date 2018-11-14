//NAME: Lauren Fromm
//EMAIL: laurenfromm@gmail.com
//ID: 404751250

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <strings.h>
#include <string.h>
#include <netdb.h>
#include "zlib.h"
#define BUFFER_SIZE 256

#define TIMEOUT 0


int pipein[2]; //create a pipe going into shell                                                
int pipeout[2]; //create a pipe leaving shell    

pid_t pid;

int port = 0;
int portNum;

int logFlag = 0;
char* logFile;
int compressFlag = 0;
int debug = 0;
int shell = 0;

struct pollfd fds[2]; //create pollfd struct      

int listenfd;
int commfd;

struct sockaddr_in server, cli_addr;

z_stream defstream;
z_stream infstream;



void openConnection()
{
  if(debug)
    fprintf(stderr, "In openConnection()");
  
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if(listenfd == -1)
    {
      fprintf(stderr, "Error opening socket");
      exit(1);
    }
  memset((char*) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY; //use static addressing instead
  server.sin_port = htons(portNum); // same as client -choose a random number greater than 10224

  if(bind(listenfd, (struct sockaddr *) &server, sizeof(server)) == -1)
    {
      fprintf(stderr, "Error binding");
      exit(1);
    }
  listen(listenfd, 5);
  int clilen = sizeof(cli_addr);
  commfd = accept(listenfd, (struct sockaddr *) &cli_addr, &clilen);
  if(commfd == -1)
    {
      fprintf(stderr, "Error opening comm socket");
      exit(1);
    }
}
void setupCompression()
{
  if(debug)
    fprintf(stderr, "In setupCompression ");
  defstream.zalloc = Z_NULL;
  defstream.zfree = Z_NULL;
  defstream.opaque = Z_NULL;

  
}
void compressFunc(char* a, char* b)
{
  if (debug)
    fprintf(stderr, "in compressFunc ");
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
  if(debug)
    fprintf(stderr, "In setupDecompression ");
  infstream.zalloc = Z_NULL;
  infstream.zfree = Z_NULL;
  infstream.opaque = Z_NULL;
}
void decompress(char* b, char* c)
{
  if(debug)
    fprintf(stderr, "In decompress");
  infstream.avail_in = (uInt)((char*)defstream.next_out - b);
  infstream.next_in = (Bytef *)b; // input char array
  infstream.avail_out = (uInt)sizeof(c); // size of output
  infstream.next_out = (Bytef *)c; // output char array

  inflateInit(&infstream);
  inflate(&infstream, Z_NO_FLUSH);
  inflateEnd(&infstream);


  
}
void sig_handler(int signo)
{
  //  fprintf(stderr, "SIGPIPE");
}

void beforeExit()
{
  close(pipeout[0]);
  close(listenfd);
  close(commfd);
  close(pipein[1]);
  int status;
  if (waitpid(pid, &status, 0) == -1)
    {
      fprintf(stderr, "Error waiting for pid");
      exit(1);
    }
  else
    {
      fprintf(stderr, "Exit with STATUS=%d\n", WEXITSTATUS(status));
      //    exit(0);
    }
}
void piper()
{
  if(debug)
    fprintf(stderr, "In piper() \n");
  if(pipe(pipein) == -1)
    {
      fprintf(stderr, "Error creating the first pipe");
      exit(1);
    }
  if(pipe(pipeout) == -1)
    {
      fprintf(stderr, "Error creating the second pipe");
      exit(1);
    }

}


void setPipes()
{
  close(pipein[1]); //close writing in fd                                              
  close(pipeout[0]); //close reading out fd                                            
  dup2(pipein[0], 0); //move reading in fd to lowest fd                      
  dup2(pipeout[1], 1); //move writing out fd to second lowest fd
  dup2(pipeout[1], 2); // for errors
  close(pipein[0]); // close reading in fd                                             
  close(pipeout[1]); //close writing out fd    
}

void doPolls()
{                                                       
  fds[0].fd = commfd; //set first fd for stdin                                             
  fds[0].events = POLLIN | POLLHUP | POLLERR;
  fds[0].revents = 0;
  fds[1].fd = pipeout[0]; //read from shell                                                    
  fds[1].events = POLLIN | POLLHUP | POLLERR  ;
  fds[1].revents = 0;
}


void readAndWrite()
{
  if(debug)
    fprintf(stderr, "In readAndWrite");
  doPolls();
  char buf;  
  char test[10];
  while(1)
    {
      // if(numtimes >= 100)
	//exit(0);
      int pollRet = poll(fds,2, 0);
       if (pollRet== -1)
	{
	  fprintf(stderr, "Error polling");
	  exit(1);
	}
      if (fds[0].revents & POLLIN) {
	if(debug)
	  fprintf(stderr, "reading");
          int sizeOfRead = read(commfd, &buf, sizeof(char));
	  /*	  fprintf(stderr, "buf : %s \n", &buf);
	  if (strcmp(&buf,"e")== 0)
	    {
	      test[0] = 'e';
	      fprintf(stderr, "test[0] = %s \n", test[0]);
	    }
	  if(&buf =="x" && test[0] == 'e')
	    test[1] = 'x';
	  else
	    test[0] = ' ';
	  if(&buf == "i" && test[0]=='e' && test[1] =='x')
	    test[2] = 'i';
	  else 
	    {
	      test[0] = ' ';
	      test[1] = ' ';
	    }
	  if(&buf=="t" && test[0]=='e' && test[1] =='x' && test[2] =='i')
	    test[3] = 't';
	  else
	    {
	      test[0] = ' ';
	      test[1] =' ';
	      test[2] =' ';
	    }
	  if(buf == '\n' &&  test[0]=='e' && test[1] =='x'&& test[2] =='i' && test[3] == 't')
	    exit(0);
	  else if (buf == ' ' &&  test[0]=='e' && test[1] =='x'&& test[2] =='i' && test[3] == 't')
	    test[4] = ' ';
	  else
	    {
	      test[0] =' ';
              test[1] =' ';
              test[2] =' ';
	      test[3]=' ';
	    }

	  if ( isdigit(buf) && test[0]=='e' && test[1] =='x'&& test[2] =='i' && test[3] == 't' && test[4] == ' ')
	    exit(buf);
	  else
	    {
	      test[0] =' ';
              test[1] =' ';
              test[2] =' ';
              test[3]=' ';
	      test[4] =' ';
	    } 
	  */
	  //	    fprintf(stderr, "test: %s", test[0]);
	  if ( buf == 0x04 )
	    {
	      close(pipein[1]);
	      //close(pipeout[0]);
	      //close(commfd);
	      //close(listenfd);
	      if(debug)
		fprintf(stderr, "^D caught!");
	      //exit(0);
	      return; //was exit^
	    }
	  else if (buf == 0x03)
	    {
	      kill(pid, SIGINT);
	      //close(pipein[1]);
	      //close(pipeout[0]);
	      //close(commfd);
	      //close(listenfd);

	      if(debug)
		fprintf(stderr, "^C caught");
	      break;
	    }
	  else if (buf == '\n' || buf == '\r')
	  {
	    //char *temp="\r\n";
	    //write(1, temp, 2*sizeof(char)); //write <cr><lf> to stdout           
	    char *temp2 = "\n";
	    write(pipein[1], temp2 , sizeof(char)); //write <lf> to shell     
	  }
	  else{
	    if (compressFlag)
	      compressFunc(&buf, &buf);
	    
	    write(pipein[1], &buf, sizeof(char));
	  }
      }

	  
      if((fds[1].revents & POLLIN) == POLLIN ){
        if(debug)
	  fprintf(stderr, "coming out of my cage");
	int sizeOfRead = read(pipeout[0], &buf, sizeof(char));
	if(sizeOfRead == -1)
	 {
	   fprintf(stderr, "Read not returning correctly");
	   exit(1);
	 }
	if(sizeOfRead == 0)
	  {
	    close(pipeout[0]);
	    exit(0);
	  }
	if(compressFlag)
	  decompress(&buf, &buf);
        write(commfd, &buf, sizeof(char));
      }
      if(fds[1].revents &POLLNVAL)
	{
	  exit(0);
	}
      if(fds[1].revents & POLLHUP)
	{ //close(pipeout[0]);
	  if(debug)
	    fprintf(stderr,"pollhup and pollerr ");
	  exit(0);
	}      
      if(fds[0].revents & POLLHUP)
	{
	  exit(0);
	}
      if(fds[0].revents & POLLERR)
	{
	  exit(1);
	}
      if(fds[1].revents & POLLERR)
	{
	  //	 close(pipeout[0]);
	  exit(1);
	      
	}
     
    }
      
}


void withShell()
{

  pid = fork();
  if(pid  == -1)//create a child process                                                      
    {
     
      fprintf(stderr, "Error forking");
      exit(1);
    }
  if (pid == 0) //if it is the child process
    {
      if (debug)
	fprintf(stderr,"Child process active \n");                                         
      setPipes();
      if (execl("/bin/bash",NULL) == -1)
	{
	  fprintf(stderr, "Error with exec");
	  exit(1);
	}//exec a shell               
      if(debug)
	fprintf(stderr, "Shell was exec'd \n");
    }
  else{ //if it is the parent process                                                      
    readAndWrite();  
  }

}



int main (int argc, char * argv[])
{
  struct option longopts[]= {
    {"port", required_argument, 0, 'p'},
    {"compress", no_argument, 0, 'c'},
    {"debug", no_argument, 0, 'd'},
    {0,0,0,0,}};
  int c;
  while((c=getopt_long(argc, argv, "", longopts, NULL)) != -1){
    switch(c){
    case 'p':
      port = 1;
      portNum = atoi(optarg);
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
  if(!port)
    {fprintf(stderr, "Error port argument is required");
      exit(1);
    }
  atexit(beforeExit);
  signal(SIGPIPE, sig_handler);

  if (debug && compressFlag)
    fprintf(stderr, "Compress flag caught");

  openConnection();

  piper();

  if(debug)
    fprintf(stderr, "Pipes created \n");


  withShell();
      

  if(debug)
    fprintf(stderr, "Ending proccess, resetting attributes \n");


  exit(0);
}

