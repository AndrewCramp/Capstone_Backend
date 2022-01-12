#include "plotter.h"
#include "commands.h"
#include "dialog.h"

int serial_port;
struct termios tty;

char * findPlotter(){
	glob_t globbuff = {0};
	printf("%s", FindingBoard);
	glob("/dev/ttyACM*",0,NULL,&globbuff);
	int i = 0;
	FILE *fp;
	char *plotter_path = (char*) malloc(500*sizeof(char));
	for(i = 0; i < globbuff.gl_pathc; i++){
		char *command = (char*) malloc(500*sizeof(char));
		char *temp = (char*) malloc(500*sizeof(char));
		size_t len1 = strlen("udevadm info -q path -n ");
		memcpy(command, "udevadm info -q path -n ",len1);
		memcpy(command+len1,globbuff.gl_pathv[i],strlen(globbuff.gl_pathv[i])+1);
		fp = popen(command,"r");
		if(fp == NULL){
			printf("Failed to run command");
			exit(1);
		}
		command = (char*) malloc(500*sizeof(char));
		fgets(temp, 100, fp);
		len1 = strlen("udevadm info -a -p ");
		memcpy(command, "udevadm info -a -p ",len1);
		memcpy(command+len1, temp,strlen(temp));
		pclose(fp);
		fp  = popen(command,"r");
		if(fp == NULL){
			printf("Failed to run command");
			exit(1);
		}
		while(fgets(temp, 10000, fp)){
       			if(strstr(temp, "EiBotBoard")!=NULL){
				pclose(fp);
				memcpy(plotter_path,globbuff.gl_pathv[i],strlen(globbuff.gl_pathv[i])+1);
				globfree(&globbuff);
				printf("%sPath:%s\n", BoardFound,plotter_path);
				return plotter_path;
			}		
		}
		pclose(fp);
	}
	globfree(&globbuff);
	return "";
}

int init(){
	char* path  = (char *)malloc(50*sizeof(char));
	path = findPlotter();  
      	serial_port = open(path, O_RDWR);
	sleep(3);
	if(serial_port < 0){
		printf("ERROR %i from open: %s\n", errno, strerror(errno));
		return 1;
	}

	if(tcgetattr(serial_port, &tty) != 0){
		printf("ERROR %i from tcgetattr: %s\n", errno, strerror(errno));
		return 1;
	}
	tty.c_cflag &= ~PARENB;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;
	tty.c_cflag &= ~CRTSCTS;
	tty.c_cflag |= CREAD | CLOCAL;
	
	tty.c_lflag &= ~ECHO;
	tty.c_lflag &= ~ECHONL;
	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ISIG;
	tty.c_lflag &= ~IEXTEN;

	tty.c_iflag &= ~(IXON | IXOFF | IXANY);
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

	tty.c_oflag &= ~OPOST;
	tty.c_oflag &= ~ONLCR;

	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = 13;

	cfsetispeed(&tty, B115200);
	cfsetospeed(&tty, B115200);

	if(tcsetattr(serial_port, TCSANOW, &tty)!=0){
		printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
		return 1;
	}
	tcflush(serial_port, TCIFLUSH);
	return 0;
}

void serialWrite(char * msg){
	write(serial_port, msg, strlen(msg));
}

int serialRead(char* buffer){
	memset(buffer, '\0', sizeof(buffer));
	int n = read(serial_port, buffer, sizeof(buffer));
	return n;
}


int main(){
	printf("%s", Welcome);
	init();
	char buffer[15];
	char* message = malloc(100*sizeof(char));
	generateCommand(message,SET_PEN,"1");
	serialWrite(message);
	sleep(3);
	generateCommand(message,SET_PEN,"0");
	serialWrite(message);
	sleep(3);
	generateCommand(message, SET_PEN,"1");
	serialWrite(message);
	generateCommand(message, STEPPER_MOVE_MIXED,"700,0,5000");
	sleep(3);
	serialWrite(message);
}
