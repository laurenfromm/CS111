//NAME: Lauren Fromm
//EMAIL: laurenfromm@gmail.com
//ID: 404751250
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <mraa/aio.h>
#include <math.h>
#include <getopt.h>
#include <time.h>
#include <poll.h>
#include <sys/stat.h>
#include <poll.h>
#include <netdb.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


const int B = 4275; // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
//const int pinTempSensor = A5;     // Grove - Temperature Sensor connect to A5
sig_atomic_t volatile run_flag = 1;
int celsius = 0;
int fahr = 1;
int stopFlag = 1;
int logFlag = 0;
FILE *logFile = NULL;
int logFilefd;
int offFlag = 0;
int period = 1;
int portNum = 0;
int id;
char* host = "";
time_t currTime;
struct tm *timeTime; 
struct pollfd fds[1]; //create pollfd struct    
int socketfd;
struct sockaddr_in server;
SSL_CTX *ctx;
SSL *ssl;



void do_when_interrupted( int sig)
{
	if(sig == SIGINT)
 		run_flag = 0;
}
	

float getTemp(float value)
{
	float R = 1023.0/value-1.0;
	R = R0*R;
	float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; 
	
	if(!celsius && fahr)
		temperature = temperature * 1.8 + 32;
	return temperature;
}

void doPolls()
{
   fds[0].fd = socketfd; //set first fd for stdin                                                                                                                                                      
   fds[0].events = POLLIN | POLLHUP | POLLERR;
}

void openConnection()
{
	if(SSL_library_init() < 0){
		fprintf(stderr, "Error using SSL library\n");
		exit(1);
	}
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
	
	ctx = SSL_CTX_new(SSLv23_client_method());
	if(ctx == NULL)
	{
		fprintf(stderr,"Error creating SSL ctx\n");
		exit(1);
	}
	ssl = SSL_new(ctx);


	//fprintf(stderr,"test1\n");
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0)
	{
		fprintf(stderr, "Error getting socket fd\n");
		exit(1);
	}
	struct hostent* hostname;
	//fprintf(stderr,"test2\n");
	hostname = gethostbyname(host);
	//fprintf(stderr,"test3\n");
	if (hostname ==NULL)
	{
		fprintf(stderr, "ERROR getting hostname\n");
		exit(1);
	}

	memset((char*) &server, 0, sizeof(server));
	server.sin_family = AF_INET;
	memcpy((char *) &server.sin_addr.s_addr, (char*) hostname->h_addr, hostname->h_length);
	server.sin_port = htons(portNum);
	//fprintf(stderr,"test4\n");
	if(connect(socketfd, (struct sockaddr *) &server, sizeof(server)) == -1)
    	{
        	fprintf(stderr, "Error conecting to socket");
	        exit(1);
	}
	//fprintf(stderr,"test5\n");

	if(SSL_set_fd(ssl, socketfd) == 0)
	{
		fprintf(stderr, "Error setting fd\n");
		exit(1);
	}
	if(SSL_connect(ssl) != 1)
	{
		fprintf(stderr,"Error connecting\n");
		exit(1);
	}


}



int main(int argc, char* argv[])
{
	struct option longopts[] = {
	{"scale", required_argument, 0, 's'},
	{"period", required_argument, 0, 'p'},
	{"log", required_argument, 0, 'l'},
	{"id", required_argument, 0, 'i'},
	{"host", required_argument, 0, 'h'},
	{0,0,0,0}
	};
	int c = 0;
	while(optind < argc){
	  c = getopt_long(argc, argv, "", longopts, 0);
	  if(c != -1)
	   {
	   //fprintf(stderr,"test\n");
	    switch(c)
	      {
	      case 's':
		if (optarg[0] == 'C')
		  celsius = 1;
		else if(optarg[0] == 'F')
		  fahr = 1;
		else 
		  {
		    fprintf(stderr, "Unrecognized scale argument\n");
		    exit(1);
		  }
		break;
	      case 'p':
		period = atoi(optarg);
		break;
	      case 'l':
		logFile = fopen(optarg, "w");
		logFlag = 1;
		break;
	      case 'i':
		id = atoi(optarg);
		break;
	      case 'h':
		host = optarg;
		break;
	      default:
		fprintf(stderr, "Error: Unrecognized Argument\n");
		exit(1);
	      }
	   }
	else
      	{
      		if(optind < argc)
	  	{
    		    portNum = atoi(argv[optind]);
		    optind++;
		//	    fprintf(stderr, "uh ya %d\n", portNum);
		 }

	} 
	
     }
  //fprintf(stderr, "tester\n");
  openConnection();


  if (logFlag)
  	fprintf(logFile, "ID=%d\n", id);
  else
  	fprintf(stdout, "ID=%d\n",id);
  char idStr[20];
  sprintf(idStr, "ID=%d\n", id);
  SSL_write(ssl,idStr, strlen(idStr));



  float value;

  mraa_aio_context temp;
 // mraa_gpio_context button;
 // button = mraa_gpio_init(62);
  temp = mraa_aio_init(1);

 // mraa_gpio_dir(button, MRAA_GPIO_IN);
 // mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &do_when_interrupted1, NULL);
  signal(SIGINT, do_when_interrupted);
  doPolls();





  while(run_flag)
  {	
//  	mraa_gpio_write(button, 1);
//	sleep(1);
	
	value = mraa_aio_read(temp);
	float real = getTemp(value);
	//float real = floor(real1 * 10) / 10;
	currTime = time(NULL);
	timeTime = localtime( &currTime);	
	char buffer[10];
 	strftime(buffer, 10, "%H:%M:%S", timeTime);
	if(stopFlag)
		fprintf(stdout, "%s %.01f\n",buffer,real);
	if(logFlag && stopFlag){
		fprintf(logFile, "%s %.01f\n", buffer, real);
//		fprintf(stderr, "%d", sizeof(real));
//		float *temp = &real;
//		write(logFilefd, &temp, sizeof(real));
//		time_t timeTemp = *asctime(localtime(&currTime));		
//		write(logFilefd, &timeTemp, sizeof(timeTemp));
//		write(logFilefd, "\n", sizeof(char));
	}
	char tempBuf[20];
	sprintf(tempBuf, "%s %.01f\n", buffer, real);
	SSL_write(ssl, tempBuf, strlen(tempBuf));
	if( poll(fds,1,0) == -1)
	{
		fprintf(stderr, "Error polling\n");
		exit(1);
	}	

	if(fds[0].revents & POLLIN)
	{
//		char c;
//		int j = 0;
//	FILE *input = fdopen(socketfd, "r");
		char buff[128];
		if(SSL_read(ssl, buff, 128) > 0)
			{




			int k = 0;
			int offset = 0;
			for(; k < 128; k++){
		
			if(buff[k] == '\n'){
				buff[k] = 0;

			if(strcmp(buff+offset, "OFF")==0)
			{
				char tempShut[25];
				 sprintf(tempShut, "%s SHUTDOWN\n", buffer);
				  SSL_write(ssl, tempShut, strlen(tempShut));
				if(logFlag)
				{
					fprintf(logFile, "OFF\n");
					fprintf(logFile, "%s SHUTDOWN\n", buffer);
				}
				else{
					fprintf(stdout, "OFF\n");
					fprintf(stdout, "%s SHUTDOWN\n", buffer);
				}
				exit(0);
	
			}
			else if (strcmp(buff+offset, "STOP")==0)
				{
				
				stopFlag = 0;
				if(logFlag)
					fprintf(logFile, "STOP\n");
				else
					fprintf(stdout, "STOP\n");
			
			}
			else if(strcmp(buff+offset, "START") == 0)
			{
				stopFlag = 1;
				if(logFlag)
			  	{       
					fprintf(logFile, "START\n");
				}
				else{
					fprintf(stdout, "START\n");
				}
			}
			else if (strcmp(buff+offset, "SCALE=C") == 0)
			{
				celsius = 1;
				fahr = 0;
				if(logFlag)
					fprintf(logFile, "SCALE=C\n");
				else
					fprintf(stdout, "SCALE=C\n");
			}	
			else if (strcmp(buff+offset, "SCALE=F") == 0)
			{
				celsius = 0;
				fahr = 1;
				if(logFlag)
					fprintf(logFile, "SCALE=F\n");
				else
					fprintf(stdout, "SCALE=F\n");
			}	
			else {
			
				char tempTest[7] ="PERIOD=";
				int i = 0;
				int flag = 0;
				for(;i != 7; i++)
				{
					if(tempTest[i] != buff[i])
					{
					flag = 1;		
					//if(logFlag)
					//		fprintf(logFile, buff);
					//	else
					//		fprintf(stdout, buff);
				
					//	fprintf(stderr, "Error unrecognized command");
					//exit(1);
					}
				}
				if(flag)
				{
					if(logFlag)
					{	
						fprintf(logFile, "%s\n",buff);
					//	fprintf(logFile, "\n");
					}
					else
					{
						fprintf(stdout,"%s\n", buff);
					//	fprintf(stdout, "\n");
					}
				continue;
			
				}
				period = atoi(&buff[7]);
				if(logFlag)
					fprintf(logFile, buff);
				else
					fprintf(stdout, buff);
			}
			offset = k + 1;
			}
		}
	}		


	}	
	sleep(period);
//	mraa_gpio_write(button, 0);
//	sleep(1);
  }
 
// mraa_gpio_write(button, 0);
  mraa_aio_close(temp);
//  mraa_gpio_close(button);
 currTime = time(NULL);
 timeTime = localtime( &currTime);
 char buffer[10];
 strftime(buffer, 10, "%H:%M:%S", timeTime);
 char tempShut[25];
 sprintf(tempShut, "%s SHUTDOWN\n", buffer);
 SSL_write(ssl, tempShut, strlen(tempShut));
 if(logFlag)
    {       

	fprintf(logFile, "%s SHUTDOWN\n", buffer);
  	fclose(logFile);
  }
 else{
	
	fprintf(stdout, "%s SHUTDOWN\n", buffer);
 }
 
 return 0;
}
							     

