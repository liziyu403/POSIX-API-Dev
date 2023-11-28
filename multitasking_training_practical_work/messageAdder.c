#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h> 
#include <unistd.h>
#include <pthread.h>
#include "messageAdder.h"
#include "msg.h"
#include "iMessageAdder.h"
#include "multitaskingAccumulator.h"
#include "iAcquisitionManager.h"
#include "debug.h"

// muutex
pthread_mutex_t consumeCountMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t outMutex = PTHREAD_MUTEX_INITIALIZER;

//consumer thread
pthread_t consumer;
//Message computed
volatile MSG_BLOCK out;
//Consumer count storage
volatile unsigned int consumeCount = 0;
unsigned int consume_tmp;
MSG_BLOCK block_tmp;

/**
 * Increments the consume count.
 */
static void incrementConsumeCount(void);

/**
 * Consumer entry point.
 */
static void *sum( void *parameters );

static void incrementConsumeCount(void) {
    pthread_mutex_lock(&consumeCountMutex);
    consumeCount++;
    pthread_mutex_unlock(&consumeCountMutex);
}

MSG_BLOCK getCurrentSum(){
	//TODO
	pthread_mutex_lock(&outMutex);
	block_tmp = out;
	pthread_mutex_unlock(&outMutex);
	return block_tmp;

}

unsigned int getConsumedCount(){
	//TODO
	pthread_mutex_lock(&consumeCountMutex);
	consume_tmp = consumeCount;
	pthread_mutex_unlock(&consumeCountMutex);
	return consume_tmp;
}

void messageAdderInit(void){
	out.checksum = 0;
	for (size_t i = 0; i < DATA_SIZE; i++)
	{
		out.mData[i] = 0;
	}
	//TODO
	pthread_create(&consumer, NULL, sum, NULL);
}

void messageAdderJoin(void){
	//TODO
	pthread_join(consumer, NULL);
}

static void *sum( void *parameters )
{
	D(printf("[messageAdder]Thread created for sum with id %d\n", gettid()));
	unsigned int i = 0;
	MSG_BLOCK msg;
	while(i<ADDER_LOOP_LIMIT){
		i++;
		sleep(ADDER_SLEEP_TIME);
		//TODO
		pthread_mutex_lock(&outMutex);
		msg = getMessage();
		messageAdd(&out, &msg); // void messageAdd(volatile MSG_BLOCK* src, volatile MSG_BLOCK* add)
		incrementConsumeCount();
		
		// consumeCount++;
		pthread_mutex_unlock(&outMutex);
	}
	printf("[messageAdder] %d termination\n", gettid());
	//TODO
	pthread_exit(NULL);
}


