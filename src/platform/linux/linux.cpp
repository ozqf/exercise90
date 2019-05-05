#include <stdio.h>
#include <time.h>
#include "../../common/common.h"
#include "linux_socket.h"

int main()
{
	// Get CPU time in nano seconds
	timespec time;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time);
	long t = time.tv_nsec;
	printf("%d!\n", t);
	printf("Oh, by the way I built with common.h imported successfully\n");
	Net_Init();
}

