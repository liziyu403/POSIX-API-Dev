#define BUFFER_SIZE 10
#define ERROR_INIT 1
#define ERROR_SUCCESS 0
#define SEMAPHORE_INITIAL_VALUE 1
#define SEM_NAME1 "/preambule_sem1"
#define SEM_NAME2 "/preambule_sem2"


#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include "acquisitionManager.h"
#include "msg.h"
#include "iSensor.h"
#include "multitaskingAccumulator.h"
#include "iAcquisitionManager.h"
#include "debug.h"


//producer count storage
volatile unsigned int produceCount = 0;
pthread_t producers[4];

// muti-write vs multi-read model
MSG_BLOCK* dataBuffer[BUFFER_SIZE] = {NULL};
int Table_IndicePlan[BUFFER_SIZE]; 
int Table_IndiceLib[BUFFER_SIZE];
int i_Libre = 0;
int i_Plan = 0;
int j_Libre = 0;
int j_Plan = 0;

static void *produce(void *params);

/**
* Semaphores and Mutex
*/
//TODO
// Mutex for produce counting operation
// atomic write mutex
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;  
// atomic read mutex
pthread_mutex_t m3 = PTHREAD_MUTEX_INITIALIZER;  
pthread_mutex_t m4 = PTHREAD_MUTEX_INITIALIZER;  

// global semaphre
sem_t *semaphore_Libre; // Sem Libre
sem_t *semaphore_Occ; // Sem Occupe

/*
* Creates the synchronization elements.
* @return ERROR_SUCCESS if the init is ok, ERROR_INIT otherwise
*/
static unsigned int createSynchronizationObjects(void);

/*
* Increments the produce count.
*/
static void incrementProducedCount(void);

static unsigned int createSynchronizationObjects(void)
{

	//TODO
	// initialization of table
	for(int i=0; i<BUFFER_SIZE; i++)
    {
    	Table_IndiceLib[i]=i;
    	Table_IndicePlan[i]=i;
    }
	// initialization of semaphore
    semaphore_Libre = sem_open(SEM_NAME1, O_CREAT, 0644, BUFFER_SIZE);
    semaphore_Occ = sem_open(SEM_NAME2, O_CREAT, 0644, 0);
    
    if ((semaphore_Libre == SEM_FAILED) || (semaphore_Occ == SEM_FAILED) ) 
    {
        perror("[sem_open]");
    	return ERROR_INIT;
    }
    
	printf("[acquisitionManager]Semaphore created\n");
	return ERROR_SUCCESS;
}

static void incrementProducedCount(void)
{
	//TODO
	produceCount++; 
}

unsigned int getProducedCount(void)
{
	unsigned int p = 0;
	//TODO
	p = produceCount; 
	return p;
}

MSG_BLOCK getMessage(void){
	//TODO
	int j_index;
	MSG_BLOCK* X;
	sem_wait(semaphore_Occ);

		pthread_mutex_lock(&m3);
			j_index = Table_IndicePlan[j_Plan];
			j_Plan = ((j_Plan + 1)%BUFFER_SIZE);
		pthread_mutex_unlock(&m3);
						
		X = dataBuffer[j_index]; // get address of MSG_BLOCK by the index j_index
						
		pthread_mutex_lock(&m4);
			Table_IndiceLib[j_Libre] = j_index;
			j_Libre = ((j_Libre + 1)%BUFFER_SIZE);
		pthread_mutex_unlock(&m4);
	sem_post(semaphore_Libre); 
	
	return *X;

}

//TODO create accessors to limit semaphore and mutex usage outside of this C module.

unsigned int acquisitionManagerInit(void)
{
	unsigned int i;
	printf("[acquisitionManager]Synchronization initialization in progress...\n");
	fflush( stdout );
	if (createSynchronizationObjects() == ERROR_INIT)
		return ERROR_INIT;
	
	printf("[acquisitionManager]Synchronization initialization done.\n");

	for (i = 0; i < PRODUCER_COUNT; i++)
	{
		//TODO
		pthread_create(&producers[i], NULL, produce, NULL); // 4 inputs correspond to 4 threads
	}

	return ERROR_SUCCESS;
}

void acquisitionManagerJoin(void)
{
	unsigned int i;
	for (i = 0; i < PRODUCER_COUNT; i++)
	{
		//TODO
		pthread_join(producers[i], NULL);
	}

	//TODO
	sem_destroy(semaphore_Occ);
	sem_destroy(semaphore_Libre);
	printf("[acquisitionManager]Semaphore cleaned\n");
}

void *produce(void* params)
{
	D(printf("[acquisitionManager]Producer created with id %d\n", gettid()));
	unsigned int i = 0;
	int i_index; // index for buffer
	MSG_BLOCK X;
	while (i < PRODUCER_LOOP_LIMIT)
	{
		i++;
		sleep(PRODUCER_SLEEP_TIME+(rand() % 5));
		//TODO
		sem_wait(semaphore_Libre); 
			if(messageCheck(&X)==1) // check pass
			{		
				pthread_mutex_lock(&m1);
					i_index = Table_IndiceLib[i_Libre];
					i_Libre = ((i_Libre + 1)%BUFFER_SIZE);
				pthread_mutex_unlock(&m1);
				getInput(i,&X);
				dataBuffer[i_index] = &X; 
				incrementProducedCount(); // produce ++
				
				pthread_mutex_lock(&m2);
					Table_IndicePlan[i_Plan]= i_index;
					i_Plan = ((i_Plan + 1)%BUFFER_SIZE);
				pthread_mutex_unlock(&m2);
			}
       		 sem_post(semaphore_Occ); 	
	}
	printf("[acquisitionManager] %d termination\n", gettid());
	//TODO
	pthread_exit(NULL);	
}
