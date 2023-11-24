#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "displayManager.h"
#include "iDisplay.h"
#include "iAcquisitionManager.h"
#include "iMessageAdder.h"
#include "msg.h"
#include "multitaskingAccumulator.h"
#include "debug.h"

// DisplayManager thread.
pthread_t displayThread;

/**
 * Display manager entry point.
 * */
static void *display( void *parameters );


void displayManagerInit(void){
	//TODO - check
	pthread_create(&displayThread, NULL, display, NULL); // 创建线程，调用 display() 接口
}

void displayManagerJoin(void){
	//TODO - check
	pthread_join(displayThread, NULL); // 线程返回
} 

static void *display( void *parameters )
{
	MSG_BLOCK msg
	D(printf("[displayManager]Thread created for display with id %d\n", gettid()));
	unsigned int diffCount = 0;
	while(diffCount < DISPLAY_LOOP_LIMIT){
		sleep(DISPLAY_SLEEP_TIME);
		//TODO - check
		msg = getCurrentSum(); // 调用 iMessagerAdder 接口
		messageDisplay(&msg); // 调用 iDisplay 接口
		diffCount = ((getProducedCount() - getConsumedCount()) <= 0) & DISPLAY_LOOP_LIMIT; // 防止消费大于生产产生错误，小于0是为了防止越界问题
		print(getProducedCount(), getConsumedCount()); // 按要求打印
	}
	printf("[displayManager] %d termination\n", gettid());
	//TODO - check
	pthread_exit(NULL);
	printf("Display Thread ended\n");
}
