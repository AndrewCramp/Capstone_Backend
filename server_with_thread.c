#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

short SocketCreate(void){
	short hSocket;
	printf("Creating the socket....\n");
	hSocket = socket(AF_INET, SOCK_STREAM, 0);
	return hSocket;
}

void *sampleFunction(void *vargp)
{
	printf("second thread created\n");
	//run xy plotter code
	return NULL;
}

int main(){
	char server_message[256] = "server says f is for friends";
	short socket_desc, new_socket;
	int c, status;
	struct sockaddr_in server, client;
	char ip_address[] = "192.168.2.123";
	unsigned int port = 13000;
	socket_desc = SocketCreate();
	if(socket_desc < 0){
		printf("Failed to create socket. Error: %s\n", strerror(errno));
		exit(0);
	}
	printf("Socket Created!\n");
	server.sin_addr.s_addr = inet_addr(ip_address);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) == -1){
		printf("Failed to bind socket. Error: %s\n",strerror(errno));
		exit(0);
	}
	printf("Socket bound to Adress: %s, Port: %d\n", ip_address, port);
	listen(socket_desc, 3);
	printf("Waiting for incoming connections...\n");
	c = sizeof(struct sockaddr_in);
	new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
	if(new_socket == -1){
		printf("Failed to accept client %d", errno);
	}
	send(new_socket,server_message,sizeof(server_message),0);
	printf("Connection accepted\n");
	while(1){
	char * buffer = (char*)calloc(10,sizeof(char));
	status = read(new_socket, buffer, sizeof(char)*10);
	if(status != -1&& (*buffer) !=NULL){	
		printf("Received: %s\n", buffer);
	}
	char confirm_message[256] = "Message Received!";
	status = write(new_socket, confirm_message, sizeof(confirm_message));
	if(buffer=="close")break;
	if(buffer=="second"){
		pthread_t thread_id;
		pthread_create(&thread_id,NULL,sampleFunction,NULL);
	}
	}
	pthread_join(thread_id,NULL);
	close(socket_desc);	

	return 0;

}

