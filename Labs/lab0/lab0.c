//NAME: Lauren Fromm
//EMAIL: laurenfromm@gmail.com
//ID: 404751250
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>

void sig_handler(int signo)
{
  fprintf(stderr, "Segmentation fault error caugt with SIGSEGV handler.", strerror(errno));
  exit(4);
}

int main(int argc, char * const argv[])
{
  bool catch; //bool to see if catch flag is used
  catch = false; //set flag originally to false
  char* inFile = ""; //strings to hold the input and output file names
  char* outFile = "";
  struct option longopts[] = {
    { "input", required_argument, 0, 'i'},
    { "output", required_argument, 0, 'o'},
    { "segfault", no_argument, 0, 's'},
    { "catch", no_argument, 0, 'c'},
    { 0, 0, 0, 0,}
  };  //longopts holds all of the possible flags
  int c; // c holds the result of getopt
  char *null = NULL; //declare a pointer to null to use for the segmentation fault
  while ( (c = getopt_long(argc, argv, "", longopts, NULL)) != -1)
    {
      switch(c)
	{
	case 'i':  //set the input string file to the given argument
	   inFile = optarg;
	   break;
        case 'o': //set the output string file to the given argument
	  outFile = optarg;
	  break;
        case 's':;
	  *null = 't'; //force a segmentation fault 
	  break;
	case 'c':
      	  catch = true; //if the catch flag is  used, set the boolean to true
	  break;
	default:
	  fprintf(stderr, "Argument not recognized: %d", c); //if any other flag is used, report error and exit
	  exit(1);
	}
      if (catch == true) //registers a SIGSEGV handler that catches the segfault
	{
	  signal(SIGSEGV,sig_handler);
	 
	}
    }
  int ifd = open(inFile,  O_RDONLY); //open the input file
  if(ifd == -1 && strcmp(inFile, ""))
  {
    fprintf(stderr, "Input file does not exist.", strerror(errno));
    exit(2);
  }
  else if (ifd >= 0) {
    close(0); //close the file descriptor
    if (dup(ifd) == -1) //duplicate the input file to the new file descriptor, if this fails, report error
      {
	fprintf(stderr, "Error creating another reference to the open file. ", strerror(errno));
	exit(2);
      }
    close(ifd); //close the redundant file descriptor 
  }


  int ofd = creat(outFile, 0666); //opens the output file
  if (ofd == -1 && strcmp(outFile, ""))
    {
      fprintf(stderr, "Can not write to output file. ", strerror(errno) );
      exit(3);
    }
  else if (ofd >= 0){
    close(1); //close the file descriptor to be replaced
    if (dup(ofd) == -1) //duplicate the the new output file to the newly emptied file descriptor, if this fails, report error
      {
	fprintf(stderr, "Error creating another reference to the open file. ", strerror(errno));
	exit(3);
      }
    close(ofd); //close the redundant file descriptor
  }
    
  char* reader; //create a string to read the input
  reader = (char*) malloc(sizeof(char));
  int r;


    r = read(0,reader, 1);
  if( r == -1)
    {
      fprintf(stderr, "Error reading input file. ", strerror(errno));
      free(reader);
      exit(-1);
    }
  while(r != 0) //read from the file descriptor for the input
      {
	if(write(1, reader, 1) ==-1) //write to the file descriptor for the output
	  {
	    fprintf(stderr, "Error writing to output. ", strerror(errno));
	    free(reader);
	    exit(-1);
	  }

	 r = read(0,reader, 1);
       }
    free(reader);
  exit(0);

}
