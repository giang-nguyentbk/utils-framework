#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include <sys/prctl.h>
#include <sched.h>

#include "traceIf.h"

#define SIZE_OF_MSG		255
#define SIZE_OF_FILELINE	64
#define SIZE_OF_TIMESTAMP	50
#define SIZE_OF_THREADNAME	16 // System limitation, see prctl man page
#define SIZE_OF_CPUID		12

/*
	From high-resolution time measurement, it's showed that using LOG_MACRO() only costs 200% overhead compared to original printf().
	Using printf() consumes about 35-40 us while using LOG_MACRO(), it's 65-75 us. It's still in acceptable ranges, right? (-_-)

	By the way, using "inline" keyword for these LOG_MACRO_ZZ apparently has no effect! That's interesting as well!
*/

void getTraceParams(const char *file, int line, char *fileline, char *timestamp, char *threadname, char *cpuid);
void log_info(const char *file, int line, const char *msg);
void log_error(const char *file, int line, const char *msg);
void log_abn(const char *file, int line, const char *msg);
void log_debug(const char *file, int line, const char *msg);

void TPT_TRACE_ZZ(const char *file, int line, int level, const char *format, ...)
{
	va_list args;
	char msg[SIZE_OF_MSG];
	va_start(args, format);
	vsnprintf(msg, SIZE_OF_MSG, format, args);
	va_end(args);

	switch (level)
	{
	case TRACE_INFO:
		log_info(file, line, msg);
		break;

	case TRACE_ERROR:
		log_error(file, line, msg);
		break;

	case TRACE_ABN:
		log_abn(file, line, msg);
		break;

	case TRACE_DEBUG:
		log_debug(file, line, msg);
		break;
	
	default:
		snprintf(msg, SIZE_OF_MSG, "Invalid trace level = %d, use default ERROR trace to print this msg!", level);
		log_error(file, line, msg);
		break;
	}
}

void log_info(const char *file, int line, const char *msg)
{
	char fileline[SIZE_OF_FILELINE];
	char timestamp[SIZE_OF_TIMESTAMP];
	char threadname[SIZE_OF_THREADNAME];
	char cpuid_buffer[SIZE_OF_CPUID];

	getTraceParams(file, line, fileline, timestamp, threadname, cpuid_buffer);

	fprintf(stdout, "%-30s %-7s %-11s { \"%s\", %s, \"-\", \"%s\" }\n", timestamp, "INFO:", cpuid_buffer, threadname, fileline, msg);
	fflush(stdout);
}

void log_error(const char *file, int line, const char *msg)
{
	char fileline[SIZE_OF_FILELINE];
	char timestamp[SIZE_OF_TIMESTAMP];
	char threadname[SIZE_OF_THREADNAME];
	char cpuid_buffer[SIZE_OF_CPUID];

	getTraceParams(file, line, fileline, timestamp, threadname, cpuid_buffer);

	fprintf(stdout, "%-30s %-7s %-11s { \"%s\", %s, \"-\", \"%s\" }\n", timestamp, "ERROR:", cpuid_buffer, threadname, fileline, msg);
	fflush(stdout);
}

void log_abn(const char *file, int line, const char *msg)
{
	char fileline[SIZE_OF_FILELINE];
	char timestamp[SIZE_OF_TIMESTAMP];
	char threadname[SIZE_OF_THREADNAME];
	char cpuid_buffer[SIZE_OF_CPUID];

	getTraceParams(file, line, fileline, timestamp, threadname, cpuid_buffer);

	fprintf(stdout, "%-30s %-7s %-11s { \"%s\", %s, \"-\", \"%s\" }\n", timestamp, "ABN:", cpuid_buffer, threadname, fileline, msg);
	fflush(stdout);
}

void log_debug(const char *file, int line, const char *msg)
{
	char fileline[SIZE_OF_FILELINE];
	char timestamp[SIZE_OF_TIMESTAMP];
	char threadname[SIZE_OF_THREADNAME];
	char cpuid_buffer[SIZE_OF_CPUID];

	getTraceParams(file, line, fileline, timestamp, threadname, cpuid_buffer);

	fprintf(stdout, "%-30s %-7s %-11s { \"%s\", %s, \"-\", \"%s\" }\n", timestamp, "DEBUG:", cpuid_buffer, threadname, fileline, msg);
	fflush(stdout);
}

void getTraceParams(const char *file, int line, char *fileline, char *timestamp, char *threadname, char *cpuid)
{
	snprintf(fileline, SIZE_OF_FILELINE, "\"%s\", %d", file, line);

	timestamp[0] = '[';
	struct timespec tv;
	clock_gettime(CLOCK_REALTIME, &tv);
	struct tm *timePointerEnd = localtime(&tv.tv_sec);
	size_t nbytes = strftime(timestamp + 1, SIZE_OF_TIMESTAMP - 2, "%Y-%m-%d %H:%M:%S", timePointerEnd);
	snprintf(timestamp + 1 + nbytes, SIZE_OF_TIMESTAMP - 2 - nbytes, ".%.9ld", tv.tv_nsec);
	int ts_len = strlen(timestamp);
	timestamp[ts_len] = ']';
	timestamp[ts_len + 1] = '\0';

	prctl(PR_GET_NAME, threadname, SIZE_OF_THREADNAME);

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