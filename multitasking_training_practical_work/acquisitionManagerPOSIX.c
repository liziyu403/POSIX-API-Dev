#define BUFFER_SIZE 4
#define ERROR_INIT 1
#define ERROR_SUCCESS 0
#define SEMAPHORE_INITIAL_VALUE 0
#define SEM_NAME1 "/preambule_sem1"
#define SEM_NAME2 "/preambule_sem2"
#define SEM_NAME3 "/preambule_sem3"
#define SEM_NAME4 "/preambule_sem4"
#define SEM_NAME5 "/preambule_sem5"
#define SEM_NAME6 "/preambule_sem6"

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
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER; // produceCount++; 
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER; // p = produceCount;

// atomic write semaphre
sem_t *semaphore1; 
sem_t *semaphore2; 
// atomic read semaphre
sem_t *semaphore3; 
sem_t *semaphore4; 
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
    	tabIndiceOcc[i]=i;
    	tabIndiceLib[i]=i;
    }
	// initialization of semaphore
	sem_unlink(SEM_NAME1);
    sem_unlink(SEM_NAME2);
    sem_unlink(SEM_NAME3);
    sem_unlink(SEM_NAME4);
    sem_unlink(SEM_NAME5);
    sem_unlink(SEM_NAME6);
    semaphore1 = sem_open(SEM_NAME1, O_CREAT, 0644, SEMAPHORE_INITIAL_VALUE);
    semaphore2 = sem_open(SEM_NAME2, O_CREAT, 0644, SEMAPHORE_INITIAL_VALUE);
    semaphore3 = sem_open(SEM_NAME3, O_CREAT, 0644, SEMAPHORE_INITIAL_VALUE);
    semaphore4 = sem_open(SEM_NAME4, O_CREAT, 0644, SEMAPHORE_INITIAL_VALUE);
    semaphore_Libre = sem_open(SEM_NAME5, O_CREAT, 0644, BUFFER_SIZE);
    semaphore_Occ = sem_open(SEM_NAME6, O_CREAT, 0644, SEMAPHORE_INITIAL_VALUE);
    
    if (((semaphore1 == SEM_FAILED) || (semaphore2 == SEM_FAILED)) || ((semaphore3 == SEM_FAILED)) || ((semaphore4 == SEM_FAILED)) || ((semaphore_Libre == SEM_FAILED) || (semaphore_Occ == SEM_FAILED))) 
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
	pthread_mutex_lock(&m1);
		printf("Owns the mutex for produceCount++\n");
		produceCount++; // Assigning the global variables with a lock to prevent other threads from modifying them.
	pthread_mutex_unlock(&m1);
}

unsigned int getProducedCount(void)
{
	unsigned int p = 0;
	//TODO
	pthread_mutex_lock(&m2);
		printf("Owns the mutex for produceCount get\n");
		p = produceCount; // Assigning the operation with a lock prevents other threads from repeating the assignment.
	pthread_mutex_unlock(&m2);
	return p;
}

MSG_BLOCK getMessage(void){
	//TODO
	int j_index;
	MSG_BLOCK* X;
	
	sem_wait(semaphore_Occ); 
	
		sem_wait(semaphore3);
			j_index = Table_IndicePlan[j_Plan];
			j_Plan = ((j_Plan + 1)%BUFFER_SIZE);
		sem_post(semaphore3);
						
		X = dataBuffer[j_index]; // get address of MSG_BLOCK by the index j_index
						
		sem_wait(semaphore4);
			Table_IndiceLib[j_Libre] = j_index;
			j_Libre = ((j_Libre + 1)%BUFFER_SIZE);
		sem_post(semaphore4);

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
	sem_destroy(semaphore1);
	sem_destroy(semaphore2);
	sem_destroy(semaphore3);
	sem_destroy(semaphore4);
	sem_destroy(semaphore_Occ);
	sem_destroy(semaphore_Libre);
	printf("[acquisitionManager]Semaphore cleaned\n");
}

void *produce(void* params)
{
	D(printf("[acquisitionManager]Producer created with id %d\n", gettid()));
	unsigned int i = 0;
	int i_index; // index for buffer
	while (i < PRODUCER_LOOP_LIMIT)
	{
		i++;
		sleep(PRODUCER_SLEEP_TIME+(rand() % 5));
		//TODO
		sem_wait(semaphore_Libre); 
			if(messageCheck(&temp)==1) // check pass
			{		
				sem_wait(semaphore1);
					i_index = tabIndiceLib[i_Libre];
					i_Libre = ((i_Libre + 1)%BUFFER_SIZE);
				sem_post(semaphore1);
				getInput(i,&X);
				dataBuffer[i_index] = &X; 
				incrementProducedCount(); // produce ++
				
				sem_wait(semaphore2);
					Table_IndicePlan[i_Plan]= i_index;
					i_Plan = ((i_Plan + 1)%BUFFER_SIZE);
				sem_post(semaphore2);
			}
        sem_post(semaphore_Occ); 	
	}
	printf("[acquisitionManager] %d termination\n", gettid());
	//TODO
	pthread_exit(NULL);	
}