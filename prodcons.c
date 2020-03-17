#ifndef PRODCONS_H
#define PRODCONS_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "prodcons.h"

// global variables
int n, consumersCounter=0, producersCounter=0, seed;
pthread_cond_t waitForProducerCondition, waitForMyTurnPro, waitForMyTurnCon;
pthread_mutex_t procoMutex;
pthread_t *consumer, *producer;
circular_buffer *cb;


//initialize circular buffer
//capacity: maximum number of elements in the buffer
//sz: size of each element 
void cb_init(circular_buffer *cb, size_t capacity, size_t sz)
{
    cb->buffer = malloc(capacity * sz);
    if(cb->buffer == NULL){
		printf("Could not allocate memory..Exiting! \n");
		exit(1);
		}
        // handle error
    cb->buffer_end = (char *)cb->buffer + capacity * sz;
    cb->capacity = capacity;
    cb->count = 0;
    cb->sz = sz;
    cb->head = cb->buffer;
    cb->tail = cb->buffer;
}

//destroy circular buffer
void cb_free(circular_buffer *cb)
{
    free(cb->buffer);
    // clear out other fields too, just to be safe
}

//add item to circular buffer
void cb_push_back(circular_buffer *cb, const void *item)
{
    if(cb->count == cb->capacity)
        {
			printf("Access violation. Buffer is full\n");
			exit(1);
		}
    memcpy(cb->head, item, cb->sz);
    cb->head = (char*)cb->head + cb->sz;
    if(cb->head == cb->buffer_end)
        cb->head = cb->buffer;
    cb->count++;
}

//remove first item from circular item
void cb_pop_front(circular_buffer *cb, void *item)
{
    if(cb->count == 0)
        {
			printf("Access violation. Buffer is empy\n");
			exit(1);
	}
    memcpy(item, cb->tail, cb->sz);
    cb->tail = (char*)cb->tail + cb->sz;
    if(cb->tail == cb->buffer_end)
        cb->tail = cb->buffer;
    cb->count--;
}


void * consumerMethod(void *t){
	int rc, popedValue; counter=0;
	char * fileName = "./cons_out.txt";
	int conThreadid = *(int*)t;
	int *values = malloc(10*sizeof(int));
	
	while((cb->count)=0){
		//wait the producer to start first.
		rc = pthread_cond_wait(&waitForProducerCondition, &procoMutex);
		if (rc != 0) {	
			printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
			pthread_exit(&rc);
		}
		
		while((cb->count)!=0){
			//mutex lock
			rc = pthread_mutex_lock(&procoMutex);
			if (rc != 0) {	
				printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
				pthread_exit(&rc);
			}	

			// save poped value to cons_out.txt and values array
			FILE *fco = fopen("./cons_out.txt", "w");
			if (fco == NULL){
				printf("Error opening file!\n");
				exit(1);
			}

			cb_pop_front(&cb, &popedValue);
			if(counter == sizeof(values)){
				int *temp = realloc(values, (sizeof(values)*2));
				if(!temp){
					printf("ERROR: reallocation failed");
					exit(1);
				}else{
					values=temp;
				}
			}
			values[counter]= popedValue;
			counter++;
			fprintf(fco, "Consumer: %d", popedValue);
			fprintf(fco, "\n");
			fclose(fco);

			// mutex unlock
			rc = pthread_mutex_unlock(&procoMutex);
			if (rc != 0) {	
				printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
				pthread_exit(&rc);
			}
		}
	}

	while(consumersCounter!=conThreadid){
		//wait for consumers before me to finish. 
		rc = pthread_cond_wait(&waitForMyTurnCon, &procoMutex);
		if (rc != 0) {	
			printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
			pthread_exit(&rc);
		}
	}
	// mutex lock	
	rc = pthread_mutex_lock(&procoMutex);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
		pthread_exit(&rc);
	}
	// consumers with id conThreadid output
	printf("Consumer %d: %s",conThreadid, arrayToString(values));
	printf("\n");
	// increase the counter to print results for the next consumer
	consumersCounter++;
	// mutex unlock
	rc = pthread_mutex_unlock(&procoMutex);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
		pthread_exit(&rc);
	}
	//free the pointer
	free(values);
	
}

void * producerMethod(void *t){
	int rc, j, seedp, counter=0, randomNumber=0;
	char* fileName = "./prod_in.txt";
	int size = sizeof(t)/sizeof(int);
 	int arguments[size];
	for(j=0; j<size; j++){
		arguments[j] = *(int*)(t+j);
	}
	int proThreadid = arguments[j];
	int* values = malloc(arguments[0]*sizeof(int));
	seedp = seed*proThreadid;
	
	//let the consumer to start
	rc= pthread_cond_signal(&waitForProducerCondition);
	if (rc != 0) {	
			printf("ERROR: return code from pthread_cond_signal() is %d\n", rc);
			pthread_exit(&rc);
	}

	while((cb->count)<= size && counter!=arguments[0]-1){
		// if the buffer is full wait for consumer to consume! 		
		if((cb->count)=(cb->capacity)){
			//let the consumer to start
			rc= pthread_cond_signal(&waitForProducerCondition);
			if (rc != 0) {	
				printf("ERROR: return code from pthread_cond_signal() is %d\n", rc);
				pthread_exit(&rc);
			}
		}

		// mutex lock
		rc = pthread_mutex_lock(&procoMutex);
		if (rc != 0) {	
			printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
			pthread_exit(&rc);
		}
		
		//save produced values to the buffer and values array
		randomNumber = rand_r((int*)seedp);
		values[counter] = randomNumber;
		counter++;
		if(randomNumber == 0){
			printf("ERRO: random generation failed!");
		}
		FILE *fpro = fopen("./prod_in.txt", "w");
		if (fpro == NULL){
			printf("Error opening file!\n");
			exit(1);
		}
		cb_push_back(&cb, &randomNumber);
		fprintf(fpro, "Producer: %d", randomNumber);
		fprintf(fpro, "\n");
		fclose(fpro);

		// mutex unlock
		rc = pthread_mutex_unlock(&procoMutex);
		if (rc != 0) {	
			printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
			pthread_exit(&rc);
		}
		
	}

	while(producersCounter != proThreadid){
	//wait for producers before me to finish. 
		rc = pthread_cond_wait(&waitForMyTurnPro, &procoMutex);
		if (rc != 0) {	
			printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
			pthread_exit(&rc);
		}
	}

	// mutex lock
	rc = pthread_mutex_lock(&procoMutex);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
		pthread_exit(&rc);
	}
	// producer with id proThreadid output
	printf("Producer %d: %s",proThreadid, arrayToString(values));
	printf("\n");
	// increase the counter to print results for the next producer
	producersCounter++;
	// mutex unlock
	rc = pthread_mutex_unlock(&procoMutex);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
		pthread_exit(&rc);
	}
	//free the pointer
	free(values);	
	
}

int main(int argc, char *argv[]) {
	int rc, i;
	pthread_t *consumer, *producer;
	int producers, consumers, numberCapacity, numbers, seed, id;
	int* producerArgs = malloc(2*sizeof(int));	
	
	if (argc<1){
		printf("No arguments were inserted");
		exit(1);
	}

	if(argc >= 4){
		producers = atoi(argv[1]); //number of producers
		consumers = atoi(argv[2]); //number of consumers
		numberCapacity = atoi(argv[3]); //capacity of buffer
		numbers = atoi(argv[4]); 
		seed = atoi(argv[5]); 
	
		consumer = malloc(consumers*sizeof(int));
		producer = malloc(producers*sizeof(int));
		cb_init(&cb, sizeof(numbers), sizeof(int));
	}
	
	
	//mutex initialization.
	rc = pthread_mutex_init(&procoMutex, NULL);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
		exit(-1);
	}
	
	//Condition initialization.
	rc = pthread_cond_init(&waitForProducerCondition, NULL);
	if (rc != 0) {
    	printf("ERROR: return code from pthread_cond_init() is %d\n", rc);
       	exit(-1);
	}
	
	//Condition initialization.
	rc = pthread_cond_init(&waitForMyTurnPro, NULL);
	if (rc != 0) {
    	printf("ERROR: return code from pthread_cond_init() is %d\n", rc);
       	exit(-1);
	}
	
	//Condition initialization.
	rc = pthread_cond_init(&waitForMyTurnCon, NULL);
	if (rc != 0) {
    	printf("ERROR: return code from pthread_cond_init() is %d\n", rc);
       	exit(-1);
	}

	//producer thread creation.
	for(i=0; i<producers-1; i++){
		producerArgs[0]=numbers;
		producerArgs[1]=i+1; // threads id
	    	rc = pthread_create(&producer[i], NULL, (void*)producerMethod, &producerArgs);
		//check if pthread_create is right.
	    	if (rc != 0) {
	    		printf("ERROR: return code from pthread_create() is %d\n", rc);
	       		exit(-1);
	    	}
	}

	//consumer thread creation.
	for(i=0; i<consumers-1; i++){
		//i for consumers pthread id
		id = i+1;
	    	rc = pthread_create(&consumer[i], NULL,  (void*)consumerMethod, &id);
		//check if pthread_create is right.
	    	if (rc != 0) {
	    		printf("ERROR: return code from pthread_create() is %d\n", rc);
	       		exit(-1);
	    	}
	}

	void *status;
	//wait for producer thread to finish.
	for(i=0; i<producers-1; i++){
		rc = pthread_join(producer[i], &status);
		
			if (rc != 0) {
				printf("ERROR: return code from pthread_join() is %d\n", rc);
				exit(-1);		
			}
	}

	//wait for consumer thread to finish.
	for(i=0; i<consumers-1; i++){
		rc = pthread_join(consumer[i], &status);
		
			if (rc != 0) {
				printf("ERROR: return code from pthread_join() is %d\n", rc);
				exit(-1);		
			}
	}
		
	//mutex destruction.
	rc = pthread_mutex_destroy(&procoMutex);
	if (rc != 0) {
   		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
      		exit(-1);
   	}
	
	//condition destruction.
	rc = pthread_cond_destroy(&waitForProducerCondition);
	if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1);		
	}

	//condition destruction.
	rc = pthread_cond_destroy(&waitForMyTurnPro);
	if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1);		
	}

	//condition destruction.
	rc = pthread_cond_destroy(&waitForMyTurnCon);
	if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1);		
	}

	//circular buffer destruction.
	cb_free(&cb);
	// free the pointer
	free(producerArgs);

 }
#endif //PRODCONS_H
