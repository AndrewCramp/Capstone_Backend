#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void subfunction(void){
	printf("subfunction!\n");
	return NULL;
}


void *secondThread(void *vargp)
{	
	printf("second thread from within\n");
	return NULL;
}	

void *sampleFunction(void *vargp)
{
	printf("thread created\n");
	subfunction(); //calling other funct?ions within a thread
	pthread_t thread_id2; //creating other threads within a thread
	pthread_create(&thread_id2,NULL, secondThread, NULL);
	pthread_join(thread_id2,NULL);//waits for thread to finish
	return NULL;
}

int main()
{
	pthread_t thread_id;
	printf("Before Thread\n");
	pthread_create(&thread_id, NULL, sampleFunction, NULL);
	pthread_join(thread_id, NULL);
	printf("After Thread\n");
	exit(0);
}
