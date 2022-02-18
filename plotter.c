#include "plotter.h"
#include "commands.h"
#include "dialog.h"

int serial_port;
struct termios tty;
int motor_status = 0;

void *readThread(void * buffer){
	while(1){
        	sleep(0.25);
        	serialRead(buffer);
        	printf("buffer: %s", buffer);
    	}
}

/*
 * Function findPlotter
 * --------------------------------------------------------
 * brief: Searches /dev/ttyACM* for plotter.
 *
 * return: plotter_path (Path to plotter file)
 */

char * findPlotter(){
	glob_t globbuff = {0};
	printf("%s", FindingBoard);
	glob("/dev/ttyACM*",0,NULL,&globbuff);
	int i = 0;
	FILE *fp;
	char *plotter_path = (char*) malloc(20*sizeof(char));
	for(i = 0; i < globbuff.gl_pathc; i++){
		char *command = (char*) malloc(100*sizeof(char));
		char *temp = (char*) malloc(100*sizeof(char));
		size_t len1 = strlen("udevadm info -q path -n ");
		memcpy(command, "udevadm info -q path -n ",len1);
		memcpy(command+len1,globbuff.gl_pathv[i],strlen(globbuff.gl_pathv[i])+1);
		fp = popen(command,"r");
		if(fp == NULL){
			printf("Failed to run command");
			exit(1);
		}
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


/*
 * Function init
 * --------------------------------------------------------
 * brief: Initiallizes serial connection with plotter.
 *
 */

int init(){
	char* path = findPlotter();  
    serial_port = open(path, O_RDWR);
	char* buffer = malloc(50*sizeof(char));
    pthread_t thread_id;
    pthread_create(&thread_id,NULL,readThread,buffer);
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


int importContours(ContourNode* contour){
    FILE *FP;
    char buffer[255];
    int * xpos = calloc(5000,sizeof(int));
    int * ypos = calloc(5000,sizeof(int));
    FP = fopen("./contours.txt","r");
    int points = 0;
    int contour_count = 0;
    int width = strtol(fgets(buffer,sizeof(buffer), FP),(char**)NULL,10);
    int height = strtol(fgets(buffer,sizeof(buffer), FP),(char**)NULL,10);
    while(fgets(buffer, sizeof(buffer), FP)){
        if(strstr(buffer, "---")){
            contour->height = height;
            contour->width = width;
            contour->x_positions = xpos; 
            contour->y_positions = ypos;
            contour->size = points;
            contour->next = (ContourNode *) malloc(sizeof(ContourNode));
            contour->end = 0;
            contour = (ContourNode *) contour->next;
            points = 0;
            contour->end = 1;
            xpos = calloc(5000,sizeof(int));
            ypos = calloc(5000,sizeof(int));
            printf("%d\n", contour_count++);
            continue;
        }
        char *token = strtok(buffer, " ");
        int count = 0;
        while(token != NULL){
            if (count == 0){
                ypos[points] = strtol(token, (char **)NULL, 10);
            }
            if (count == 1){
                xpos[points] = strtol(token, (char **)NULL, 10);
            }
            count++;
            token = strtok(NULL," ");
        }
        points++;
    }
}

int printList(ContourNode * HEAD){
    ContourNode * temp = HEAD;
    int count = 0;
    do{
        printf("%d\n", count++);
        printf("-------\n");
        int i = 0;
        for(i = 0; i < temp->size; i++){
            printf("x[1]: %d, ", temp->x_positions[i]);
            printf("y[1]: %d\n", temp->y_positions[i]);
        }
        printf("Points: %d\n", temp->size);
        printf("Points: %d\n", temp->end);
        printf("-------\n\n");
        temp = (ContourNode *)temp->next;
    }while(!temp->end);
}

/*
 * Function lowerPen
 * --------------------------------------------------------
 * brief: Lowers pen to page using servo.
 *
 */

int lowerPen(){
	char* message = malloc(50*sizeof(char));
	generateCommand(message,SET_PEN,"0");
	serialWrite(message);
	sleep(0.1);
}

int pixelToStep(int height, int width, int* x, int* y){
    int ratioX = (int) MAX_X/height;
    int ratioY = (int) MAX_Y/width;
    *x = *x * ratioX;
    *y = *y * ratioY;
    return 0;
}

int drawImage(ContourNode* HEAD){
    ContourNode * temp = HEAD;
    int count = 0;
    do{
        printf("Points: %d\n", temp->size);
        int i = 0; 
       for(i = 0; i < temp->size;i++){
            pixelToStep(360,360,&temp->x_positions[i],&temp->y_positions[i]);
        }
        drawContour(temp->x_positions,temp->y_positions,temp->size);
        temp = temp->next;
    }while(!temp->end);
    movePen(0,0,0);
    return 0;
}

/*
 * Function raisePen
 * --------------------------------------------------------
 * brief: Raises pen off of page using servo.
 *
 */

int raisePen(){
	char* message = malloc(50*sizeof(char));
	generateCommand(message,SET_PEN,"1");
	serialWrite(message);
	sleep(0.1);
}

/*
 * Function drawContour
 * --------------------------------------------------------
 * brief: Moves writing head to specified position.
 *
 * x: Pointer to array of X positions.
 *
 * y: Pointer to array of y positions. Must be same length 
 *    as X array.
 *
 * points: number of points to be connected
 *
 */

int drawContour(int* x, int* y, int points){
	movePen(x[0], y[0],0);
	sleep(1.5);
	lowerPen();
	sleep(1.5);
	int i;
	printf("points: %d\n",points);
	for(i = 0; i < points; i++){
		movePen(x[i], y[i], 0);	
	}
	sleep(1.5);
	raisePen();
	sleep(1.5);
	return 0;
}

/*
 * Function movePen
 * --------------------------------------------------------
 * brief: Moves writing head /o specified position.
 *
 * x: X position in steps to be moved to.
 *
 * y: Y position in steps to be moved to.
 *
 * strictMode: If strict mode is enabled (1) the function 
 *             will check if the motors are moving before 
 *             and after each movment,
 *
 */

int movePen(int x, int y, int strictMode){
	char* move_message = malloc(50*sizeof(char));
	int motorStatus = 0;
	if(x < 0 || y < 0){
		printf("%s", BoundsError);
		printf("Desired X: %d, Desired Y: %d\n", x, y);
		printf("Min X: %d, Min Y: %d\n", 0, 0);
		free(move_message);
		return 1;
	}
	if(x > MAX_X || y > MAX_Y){
		printf("%s", BoundsError);
		printf("Desired X: %d, Desired Y: %d\n", x, y);
		printf("Max X: %d, Max Y: %d\n", MAX_X, MAX_Y);
		free(move_message);
		return 1;
	}

	if (strictMode){
        int ms;
	    while(ms = getMotorStatus())  sleep(0.25);
    }   
	int xSteps = x-plotterXPos;
	int ySteps = y-plotterYPos;
	int moveTime = sqrt(xSteps*xSteps + ySteps*ySteps)*0.55;
    char *params;
	size_t sz;
	sz = snprintf(NULL, 0, "%d,%d,%d", moveTime, xSteps,ySteps);
	params = (char *) malloc(sz+1);
	sz = snprintf(params, sz+1, "%d,%d,%d", moveTime, xSteps,ySteps);
	generateCommand(move_message,STEPPER_MOVE_MIXED,params);
    printf("command: %s", move_message);
	serialWrite(move_message);
    plotterXPos = x;
	plotterYPos = y;
    sleep((double)moveTime/1000.0);
}


/*
 * Function getMotorStatus
 * --------------------------------------------------------
 * brief: Checks if motors are currently moving.
 *        
 * return: status (0) if motors not moving (1) if motors 
 *         moving 
 *
 */

int getMotorStatus(){
	char* motor_buffer= malloc(50*sizeof(char));
	char* message = malloc(50*sizeof(char));
	int status = 0;
	generateCommand(message,QUERY_MOTORS,"");
    int count = 0;
    do{
	    serialWrite(message);
	    sleep(0.5);
	    count = serialRead(motor_buffer);
    }while(count < 11);
	const char delim[2] = ",";
	char *substring;
	substring = strtok(motor_buffer, delim);
	if(!strstr(substring, "0") && !strstr(substring, "Q"))
		status = 1;
	while(substring != NULL){
		if(!strstr(substring, "0") && !strstr(substring, "Q"))
			status = 1;
		substring = strtok(NULL, delim);	
	}
	free(motor_buffer);
	free(message);
	return status;
}


/*
 * Function serialWrite
 * --------------------------------------------------------
 * brief: Writes data to connected plotter over serial.
 *        data is written from string pointed to by msg.
 *        Data is written until string terminator.
 *
 * msg: String pointer pointing to data to be written. 
 *
*/

int serialWrite(char * msg){
	write(serial_port, msg, strlen(msg));
	return 0;
}

/*
 * Function serialRead
 * --------------------------------------------------------
 * brief: Reads serial buffer from connected plotter until 
 *        newline character, data is placed in buffer array.
 *
 * buffer: chaaracter array pointer pointing to array to 
 *         hold data from the serial port.
 *
*/

int serialRead(char* buffer){
	memset(buffer, '\0', sizeof(buffer));
	char temp;
	int n = 0;
    long timeout = (long)time(NULL)+3;
	while(temp != '\n' && (long)time(NULL) < timeout){
        sleep(0.05); 
        int size_read = 0;
		 n += read(serial_port, &temp, 1);
         if (n > 0) {
		 buffer[n-1] = temp;
         }
	}
	return n;
}
