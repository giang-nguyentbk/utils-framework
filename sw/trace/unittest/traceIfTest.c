#include <stdio.h>
#include "traceIf.h"
#include <pthread.h>
#include <sys/prctl.h>
#include <unistd.h>

// unsigned long int calc_time_diff(struct timespec t_start, struct timespec t_end)
// {
// 	unsigned long int diff = 0;

// 	if(t_end.tv_nsec < t_start.tv_nsec)
// 	{
// 		diff = (t_end.tv_sec - t_start.tv_sec)*1000000000 - (t_start.tv_nsec - t_end.tv_nsec);
// 	} else
// 	{
// 		diff = (t_end.tv_sec - t_start.tv_sec)*1000000000 + (t_end.tv_nsec - t_start.tv_nsec);
// 	}

// 	return diff; 
// }

void * test_thread(void *data)
{
	(void)data;

	// Keep in mind, thread name is never longer than 16 bytes (including NULL-terminated character), this is a kernel limitation.
	prctl(PR_SET_NAME, "test_thread");
	TPT_TRACE(TRACE_INFO, "Frequency gain = %.3f", 2.145);
	TPT_TRACE(TRACE_ERROR, "MeasGroup %d not found!", 1);

	sleep(2);

	TPT_TRACE(TRACE_ABN, "Unknown direction %s!", "DL_1");
	TPT_TRACE(TRACE_DEBUG, "Debugging mode is enabled!");

	return NULL;
}

int main(void)
{
	/* Used for LOG_MACRO time consuming measurement */
	// struct timespec t_start;
	// struct timespec t_end;

	// clock_gettime(CLOCK_REALTIME, &t_start);
	TPT_TRACE(TRACE_INFO, "New cycle was started!");
	// printf("New cycle was started!\n");
	// clock_gettime(CLOCK_REALTIME, &t_end);

	// unsigned long int difftime = calc_time_diff(t_start, t_end);
	// TPT_TRACE(TPT_INFO, "Time needed for LOG_INFO = %lu (ns) -> %lu (ms)!", difftime, difftime/1000000);

	TPT_TRACE(TRACE_ERROR, "Failed to get power boost on port %d", 3);

	pthread_t threadid;
	pthread_create(&threadid, NULL, &test_thread, NULL);

	sleep(1);

	TPT_TRACE(TRACE_ABN, "DB key /abc/def/ not found, use default value = %d!", 22);
	TPT_TRACE(TRACE_DEBUG, "This trace used for debugging!");
	
	TPT_TRACE(-1, "This trace used for debugging!");

	pthread_join(threadid, NULL);

	return 0;
}