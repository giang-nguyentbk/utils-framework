#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include <sys/prctl.h>
#include <sched.h>

#include "traceIf.h"

#define SIZE_OF_MSG		255
#define SIZE_OF_LEVEL		7
#define SIZE_OF_FILELINE	64
#define SIZE_OF_TIMESTAMP	50
#define SIZE_OF_THREADNAME	16 // System limitation, see prctl man page
#define SIZE_OF_CPUID		12

/*
	From high-resolution time measurement, it's showed that using LOG_MACRO() only costs 200% overhead compared to original printf().
	Using printf() consumes about 35-40 us while using LOG_MACRO(), it's 65-75 us. It's still in acceptable ranges, right? (-_-)

	By the way, using "inline" keyword for these LOG_MACRO_ZZ apparently has no effect! That's interesting as well!
*/

void get_loglevel(int level, char *loglevel, char *msg);
void get_fileline(const char *file, int line, char *fileline);
void get_timestamp(char *timestamp);
void get_threadname(char *threadname);
void get_cpuid(char *cpuid);

void tracepoint(const char *file, int line, int level, const char *format, ...)
{
	va_list args;
	char msg[SIZE_OF_MSG];
	va_start(args, format);
	vsnprintf(msg, SIZE_OF_MSG, format, args);
	va_end(args);

	char loglevel[SIZE_OF_LEVEL];
	get_loglevel(level, loglevel, msg);

	char fileline[SIZE_OF_FILELINE];
	get_fileline(file, line, fileline);

	char timestamp[SIZE_OF_TIMESTAMP];
	get_timestamp(timestamp);

	char threadname[SIZE_OF_THREADNAME];
	get_threadname(threadname);

	char cpuid[SIZE_OF_CPUID];
	get_cpuid(cpuid);

	fprintf(stdout, "%-30s %-7s %-11s { \"%s\", %s, \"-\", \"%s\" }\n", timestamp, loglevel, cpuid, threadname, fileline, msg);
	fflush(stdout);
}

void get_loglevel(int level, char *loglevel, char *msg)
{
	switch (level)
	{
	case TRACE_INFO:
		strcpy(loglevel, "INFO:");
		break;

	case TRACE_ERROR:
		strcpy(loglevel, "ERROR:");
		break;

	case TRACE_ABN:
		strcpy(loglevel, "ABN:");
		break;

	case TRACE_DEBUG:
		strcpy(loglevel, "DEBUG:");
		break;
	
	default:
		strcpy(loglevel, "ERROR:");
		snprintf(msg, SIZE_OF_MSG, "Invalid trace level = %d, use default ERROR trace to print this msg!", level);
		break;
	}
}

void get_fileline(const char *file, int line, char *fileline)
{
	snprintf(fileline, SIZE_OF_FILELINE, "\"%s\", %d", file, line);
}

void get_timestamp(char *timestamp)
{
	timestamp[0] = '[';
	struct timespec tv;
	clock_gettime(CLOCK_REALTIME, &tv);
	struct tm *timePointerEnd = localtime(&tv.tv_sec);
	size_t nbytes = strftime(timestamp + 1, SIZE_OF_TIMESTAMP - 2, "%Y-%m-%d %H:%M:%S", timePointerEnd);
	snprintf(timestamp + 1 + nbytes, SIZE_OF_TIMESTAMP - 2 - nbytes, ".%.9ld", tv.tv_nsec);
	int ts_len = strlen(timestamp);
	timestamp[ts_len] = ']';
	timestamp[ts_len + 1] = '\0';
}

void get_threadname(char *threadname)
{
	prctl(PR_GET_NAME, threadname, SIZE_OF_THREADNAME);
}

void get_cpuid(char *cpuid)
{
	unsigned int cpu = 999;
	getcpu(&cpu, NULL);
	if(cpu != 999)
	{
		if(cpu < 10)
		{
			sprintf(cpuid, "{ cpu  %d }", cpu);
		} else
		{
			sprintf(cpuid, "{ cpu %d }", cpu);
		}
	} else
	{
		strcpy(cpuid, "{ - }");
	}
}