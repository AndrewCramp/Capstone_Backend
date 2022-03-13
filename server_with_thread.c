#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "plotter.h"

/* 
 * Function SocketCreate
 *---------------------------------------------------------
 * brief: Creates IPv4 TCP/IP host socket
 * 
 * return: hsocket (Object containing socket)
 */
short SocketCreate(void){
	short hSocket;
	printf("Creating the socket....\n");
	hSocket = socket(AF_INET, SOCK_STREAM, 0);
	return hSocket;
}


/*
 * Function CheckBuffer
 *---------------------------------------------------------
 * brief: Thread to execute code on plotter
 * 
 */

void * checkBuffer(void *vargp) {
    int count = 0;
    while(1){
        if(count < numberOfFiles){
            char * filename = malloc(20*sizeof(char));
            snprintf(filename, 20, "./contours%d.txt", count);
            ContourNode* linked_list = malloc(sizeof(ContourNode));
            importContours(linked_list, filename);
            printList(linked_list);
            drawImage(linked_list);
            count++;
            free(filename);
        }
    }
}

int receive_file(char * buffer, int size, char * name_buffer) {
    FILE *fp;
    fp = fopen(name_buffer, "w");
    if (fp == NULL){
        printf("Error");
        exit(1);
    }
    int i = 0;
    for (i = 0; i < size; i++) {
        if (buffer[i] == EOF) {
            fclose(fp);
            return 1;
        }
        fprintf(fp, "%c", buffer[i]);
        printf("%c", buffer[i]);
    }
    fclose(fp);
    return 0;
}

/*
 * Function SocketCreate
 *---------------------------------------------------------
 * brief: Creates IPv4 TCP/IP host socket
 * 
 * return: hsocket (Object containing socket)
 */

int main(){
//	char server_message[256] = "server says f is for friends";
	short socket_desc, new_socket;
	int c, status;
	struct sockaddr_in server, client;
	char ip_address[] = "192.168.0.123";
    char * data_buffer = malloc(sizeof(char)*100000);
	unsigned int port = 13000;
    char * filename = malloc(20*sizeof(char));
    numberOfFiles = 0;
    init();
    pthread_t thread_id;    //Creates variable to hold thread_id
	pthread_create(&thread_id,NULL,checkBuffer,NULL);    //Connects thread_id to sampleFunction
	socket_desc = SocketCreate();   // Create Host Socket
	if(socket_desc < 0){
		printf("Failed to create socket. Error: %s\n", strerror(errno));
		exit(0);
	}
	printf("Socket Created!\n");
	server.sin_addr.s_addr = inet_addr(ip_address);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
    // Bind socket to IP adress and Port
	if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) == -1){
		printf("Failed to bind socket. Error: %s\n",strerror(errno));
		exit(0);
	}
	printf("Socket bound to Adress: %s, Port: %d\n", ip_address, port);
    //Start Listening for new connections
    listen(socket_desc, 10);
    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
    //Accept Client Connection
	while(1){
        realloc(filename, 20*sizeof(char));
        int *status = realloc(data_buffer, sizeof(char)*100000);
    	new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
    	if(new_socket == -1){
            sleep(0.5);
    	    continue;
        }
   // 	send(new_socket,server_message,sizeof(server_message),0);
    	printf("Connection accepted\n");
        
        int bytes_read = 0;
        while(bytes_read < 5){
	        bytes_read += read(new_socket, data_buffer+bytes_read, sizeof(int)*100000);
        }
        snprintf(filename, 20, "./contours%d.txt", numberOfFiles);
        printf("%s\n", filename);
	    receive_file(data_buffer, bytes_read, filename);
        numberOfFiles++;
	//    char confirm_message[256] = "Message Received!";
	//    status = write(new_socket, confirm_message, sizeof(confirm_message));
    }
    free(filename);
	close(socket_desc);	
    return 0;
}

