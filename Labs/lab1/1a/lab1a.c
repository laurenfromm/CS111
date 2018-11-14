
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


#define BUFFER_SIZE 256

#define TIMEOUT 0

int shell = 0; // set a flag to catch if --shell is used
int debug = 0; //set a flag to catch if --debug is used   

struct termios old, saved;

int pipein[2]; //create a pipe going into shell                                                                                                                                                           
int pipeout[2]; //create a pipe leaving shell      

pid_t pid;
struct pollfd fds[2]; //create pollfd struct      


void sig_handler(int signo)
{
  if (signo == SIGINT && shell ==1){
    kill(pid,SIGINT);
  }
  if(signo ==SIGPIPE)
    exit(0);
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

void resetAttributes()
{
  tcsetattr(STDIN_FILENO, TCSANOW, &saved); //set the attributes with the updates      ~
  if(shell == 1 )
    {
      int temp;
      if (waitpid(pid, &temp, 0) == -1)
	{
	  fprintf(stderr, "Error waiting");
	  exit(1);
	}
      else
	{
	  fprintf(stderr, "Exit with STATUS=%d\n", WEXITSTATUS(temp));
	  exit(0);

	}
    }
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


void setPipes()
{
  close(pipein[1]); //close writing in fd                                                                                                                                                               
  close(pipeout[0]); //close reading out fd                                                                                                                                                             
  dup2(pipein[0], 0); //move reading in fd to lowest fd                                                                                                                      
  dup2(pipeout[1], 1); //move writing out fd to second lowest fd                                                                                                                    
  close(pipein[0]); // close reading in fd                                                                                                                                                              
  close(pipeout[1]); //close writing out fd    
}

void doPolls()
{                                                                                                                                                        
  fds[0].fd = 0; //set first fd for stdin                                                                                                                                                      
  fds[0].events = POLLIN | POLLHUP | POLLERR;

  fds[1].fd = pipeout[0]; //read from shell                                                                                                                                                    
  fds[1].events = POLLIN | POLLHUP | POLLERR;
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
      if (execl("/bin/bash",NULL))
	{
	  fprintf(stderr, "Error with exec");
	  exit(1);
	}//exec a shell               
      if(debug)
	fprintf(stderr, "Shell was exec'd \n");
    }
  else{ //if it is the parent process                                                                                                                                                                   
    if(debug)
      fprintf(stderr, "Parent Process active \n");
    close(pipein[0]);
    close(pipeout[1]);

    doPolls(); 
    if(debug)
      fprintf(stderr, "Polls created \n");

    char buf;
    while (1) //keep looping through                                                                                                                                                                    
      {

	if( poll(fds, 2, TIMEOUT) == -1) //wait for input sources                                                                                                                                 
	  {
	    fprintf(stderr, "Error polling");
	    exit(1);
	  }
	if(fds[0].revents & POLLIN) //if there is info coming in                                                                                                                                        
	  {
	    if(debug)
	      fprintf(stderr, "Input poll1 \n");
	    read(0, &buf, sizeof(char)); //read from the input                                                                                                                                  
	    if (buf == 0x04) //if there is a ^D                                                                                                                                                         
	      {
		if(debug)
		  fprintf(stderr, "^D recieved \n");
		close(pipein[1]); //close pipe to shell but continuing processing info from shell                                                                                                       
             }
	    else if (buf == 0x03) //if there is a ^C                                                                                                                                                    
	      {
		if(debug)
		  fprintf(stderr, "^C recieved \n");
		kill(pid, SIGINT); //use kill to send SIGINT to the shell process                                                                                                                       
		write(pipein[1], &buf, sizeof(char));
	      }
	    else if( buf == '\r' || buf == '\n') //if there is <cr> or <lf>                                                                                                                             
	      {
		if(debug)
		  fprintf(stderr, "<cr> or <lf> recieved \n");
		char *temp="\r\n";
		write(1, temp, 2*sizeof(char)); //write <cr><lf> to stdout                                                                                                                                 
		char *temp2 = "\n";
		write(pipein[1], temp2 , sizeof(char)); //write <lf> to shell       
	      }
	    else
	      {
		write(1, &buf, sizeof(char)); //write to stdout what we read                                                                                                                             
		write(pipein[1], &buf, sizeof(char)); //write to shell what we read                                                                                                                      
	      }
	    

	  }
	if(fds[1].revents & POLLIN) //if the second pipe should input                                                                                                                                   
	  {
	    if (debug)
	      fprintf(stderr, "Input poll2 \n");
	    read(pipeout[0], &buf, sizeof(char)); //read                                                                                                                                        
	    if (buf == '\n') //if there is a <lf>                                                                                                                                                       
	      {
		char *temp="\r\n";
		write(1, temp,2* sizeof(char)); //write to std out <cr><lf>                                                                                                                                

	      }
	    else
	      write(1, &buf, sizeof(char)); //otherwise write to stdout                                                                                                                                  
	  }
	if(fds[1].revents & (POLLHUP | POLLERR))
	  {
	    if(debug)
	      fprintf(stderr, "POLLHUP&POLLERR");
	    exit(0);
	  }
      }

  }


}

void withoutShell()
{
  char* buf2;
  buf2 =  (char*) malloc(sizeof(char));

  
  while(1)
    {
      read(0, buf2, BUFFER_SIZE);
      if ( *buf2 == 0x04) //if there is a ^D                                                                                                                                                              
	{
	  if(debug)
	    fprintf(stderr, "^D recieved \n");
	  free(buf2);
	  exit(0);
	}
      else if ( *buf2 == '\r' || *buf2 == '\n') //if there is a <cr> or <lf>                                                                                                                               
	{
	  if(debug)
	    fprintf(stderr, "<cr> or <lf> recieved \n");
	  char temp[] = "\r\n";
	  write(1, temp, 2*sizeof(char)); //write to stdout <cr><lf>                                                                                                                                       
	}
      else
	{
	  char *temp2 = buf2;
	  if( write(1, temp2 , 1) == -1) //otherwise write to stdout
	    {
	      fprintf(stderr, "Error writing to output");
	      free(buf2);
	      exit(1);
	    }
	}
    }
  free(buf2);

}
int main (int argc, char * argv[])
{
  struct option longopts[] = {       //define a struct for arguments
    {"shell", no_argument, 0, 's'},
    {"debug", no_argument, 0, 'd'},
    { 0, 0, 0, 0,}
  };
  int c;
  while ((c = getopt_long(argc, argv, "", longopts, NULL)) != -1){
    switch(c)
      {
      case 's': //if flag is used, set shell to one
	signal(SIGINT, sig_handler);
	signal(SIGPIPE, sig_handler);
	shell = 1;
	break;
      case 'd': //if degbuf flag is used;
	debug = 1;
	fprintf(stderr, "Debugging mode activiated \n");
	break;
      default: //if any other argument is used, exit
	fprintf(stderr, "Error: Argument not recognized: %d \n", c);
	exit(1);
      }
  }
  if(!isatty(STDIN_FILENO)) //make sure we can read from stdin
    {
      fprintf(stderr, "Error reading from stdin \n");
      exit(1);
    }

  attributes();

  if (debug)
    fprintf(stderr, "Termios created \n");
  
  piper();

  if(debug)
    fprintf(stderr, "Pipes created \n");

  if (shell == 1) //if the shell flag is set
    {
      if (debug)
	fprintf(stderr, "Shell flag caught \n");
      withShell();

    } 
  else //if shell flag is not used
    {
      if(debug)
	fprintf(stderr, "Shell flag not used \n");
      withoutShell();
    }


  if(debug)
    fprintf(stderr, "Ending proccess, resetting attributes \n");
  exit(0);
}
