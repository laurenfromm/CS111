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
time_t currTime;
struct tm *timeTime; 
struct pollfd fds[1]; //create pollfd struct    


void do_when_interrupted( int sig)
{
	if(sig == SIGINT)
  	 run_flag = 0;
}

void do_when_interrupted1()
{
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
   fds[0].fd = 0; //set first fd for stdin                                                                                                                                                      
   fds[0].events = POLLIN | POLLHUP | POLLERR;
}

int main(int argc, char* argv[])
{
	struct option longopts[] = {
	{"scale", required_argument, 0, 's'},
	{"period", required_argument, 0, 'p'},
	{"log", required_argument, 0, 'l'},
	};
	int c = 0;
	while((c = getopt_long(argc, argv, "", longopts, 0)) != -1)
	{
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
		default:
			fprintf(stderr, "Error: Unrecognized Argument\n");
			exit(1);
		}

	}



  float value;

  mraa_aio_context temp;
  mraa_gpio_context button;
  button = mraa_gpio_init(62);
  temp = mraa_aio_init(1);

  mraa_gpio_dir(button, MRAA_GPIO_IN);
  mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &do_when_interrupted1, NULL);
  signal(SIGINT, do_when_interrupted);
  doPolls();


  while(run_flag)
  {	
  	mraa_gpio_write(button, 1);
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
	if( poll(fds,1,0) == -1)
		exit(1);
	
	if(fds[0].revents & POLLIN)
	{
//		char c;
//		int j = 0;
		char buff[128];
		if(fgets(buff, 128, stdin)!=NULL)
			{

//	fprintf(stderr,"buff measured: %s\n", buff);
//		int sizeofbuff = sizeof(buff);
//		fprintf(stderr, "sizeofbuff: %d\n", sizeofbuff);
		//while (c != '\n')
		//{
	//		read(0, &c, 1);
//			buff[j] = c;
//			j++;
//		}
		//buff[j]='\0';
	//char buff[128];
		//scanf("%s", buff);
	int k = 0;
	int offset = 0;
	for(; k < 128; k++){
	
		if(buff[k] == '\n'){
			buff[k] = 0;

			if(strcmp(buff+offset, "OFF")==0)
			{
				if(logFlag)
				{
					fprintf(logFile, "OFF\n");
					fprintf(logFile, "%s SHUTDOWN", buffer);
				}
				else{
					fprintf(stdout, "OFF\n");
					fprintf(stdout, "%s SHUTDOWN", buffer);
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
	mraa_gpio_write(button, 0);
//	sleep(1);
  }
 
  mraa_gpio_write(button, 0);
  mraa_aio_close(temp);
  mraa_gpio_close(button);
 currTime = time(NULL);
 timeTime = localtime( &currTime);
 char buffer[10];
 strftime(buffer, 10, "%H:%M:%S", timeTime);
 if(logFlag)
    {       

	fprintf(logFile, "%s SHUTDOWN", buffer);
  	fclose(logFile);
  }
 else{
	
	fprintf(stdout, "%s SHUTDOWN", buffer);
 }
 
 return 0;
}
							     

