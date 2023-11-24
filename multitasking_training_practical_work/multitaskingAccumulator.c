#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h> 
#include "multitaskingAccumulator.h"
#include "acquisitionManager.h"
#include "displayManager.h"
#include "messageAdder.h"
#include "displayManager.h"	
#include "debug.h"

/**

要求 1：多任务累加器（MultitaskingAccumulator）必须获取四个输入，并在获取一个输入后立即输出输入的累加结果。累加相当于生成一个 256 个整数的数组，其中第 i 个元素是输入数组中第 i 个元素的和。

要求 2：多任务累加器必须获取所有输入数据。

要求 3：多任务累加器必须保证输入数据在求和之前是正确的。=> messageCheck()

要求 4：多任务累加器必须尽快求和并产生输出，即一旦出现输入，无需等待。

要求 5：软件必须为每次累加操作生成诊断输出，显示有多少输入已被获取、有多少输入已被累加以及还有多少输入有待累加。

**/

//The entry point of the process
int main( void )
{
	printf("[multitaskingAccumulator]Software initialization in progress...\n");	
	acquisitionManagerInit();
	messageAdderInit();
	displayManagerInit();
	printf("[multitaskingAccumulator]Task initialization done.\n");
	
	printf("[multitaskingAccumulator]Scheduling in progress...\n");

	displayManagerJoin();
	messageAdderJoin();
	acquisitionManagerJoin();

	printf("[multitaskingAccumulator]Threads terminated\n");

	exit(EXIT_SUCCESS);

}
