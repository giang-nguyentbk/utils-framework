#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include <sys/prctl.h>
#include <sched.h>

#include "traceIf.h"

/*
	From high-resolution time measurement, it's showed that using LOG_MACRO() only costs 200% overhead compared to original printf().
	Using printf() consumes about 35-40 us while using LOG_MACRO(), it's 65-75 us. It's still in acceptable ranges, right? (-_-)

	By the way, using "inline" keyword for these LOG_MACRO_ZZ apparently has no effect! That's interesting as well!
*/

void LOG_INFO_ZZ(const char *file, int line, const char *format, ...)
{
	va_list args;
	char buffer[256];

	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	char timestamp[50];
	struct timespec tv;
	clock_gettime(CLOCK_REALTIME, &tv);
	struct tm *timePointerEnd = localtime(&tv.tv_sec);
	size_t nbytes = strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timePointerEnd);
	snprintf(timestamp + nbytes, sizeof(timestamp) - nbytes, ".%.9ld", tv.tv_nsec);

	char threadname[30];
	threadname[0] = '\"';
	prctl(PR_GET_NAME, &threadname[1], (30 - 2));
	int len = strlen(threadname);
	threadname[len] = '\"';
	threadname[len + 1] = '\0';

	char fileline[64];
	snprintf(fileline, 64, "%s:%d", file, line);

	char cpuid_buffer[5];
	unsigned int cpuid = 999;
	
	getcpu(&cpuid, NULL);
	if(cpuid != 999)
	{
		sprintf(cpuid_buffer, "%d", cpuid);
	} else
	{
		cpuid_buffer[0] = '-';
		cpuid_buffer[1] = '\0';
	}

	fprintf(stdout, "%-30s %-10s cpu=%-5s %-20s %-25s msg: %-s", timestamp, "INFO", cpuid_buffer, threadname, fileline, buffer);
	fflush(stdout);
}

void LOG_ERROR_ZZ(const char *file, int line, const char *format, ...)
{
	va_list args;
	char buffer[256];

	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	char timestamp[50];
	struct timespec tv;
	clock_gettime(CLOCK_REALTIME, &tv);
	struct tm *timePointerEnd = localtime(&tv.tv_sec);
	size_t nbytes = strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timePointerEnd);
	snprintf(timestamp + nbytes, sizeof(timestamp) - nbytes, ".%.9ld", tv.tv_nsec);

	char threadname[30];
	threadname[0] = '\"';
	prctl(PR_GET_NAME, &threadname[1], (30 - 2));
	int len = strlen(threadname);
	threadname[len] = '\"';
	threadname[len + 1] = '\0';

	char fileline[64];
	snprintf(fileline, 64, "%s:%d", file, line);

	char cpuid_buffer[5];
	unsigned int cpuid = 999;
	getcpu(&cpuid, NULL);
	if(cpuid != 999)
	{
		sprintf(cpuid_buffer, "%d", cpuid);
	} else
	{
		cpuid_buffer[0] = '-';
		cpuid_buffer[1] = '\0';
	}

	fprintf(stdout, "%-30s %-10s cpu=%-5s %-20s %-25s msg: %-s", timestamp, "ERROR", cpuid_buffer, threadname, fileline, buffer);
	fflush(stdout);
}

void LOG_ABN_ZZ(const char *file, int line, const char *format, ...)
{
	va_list args;
	char buffer[256];

	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	char timestamp[50];
	struct timespec tv;
	clock_gettime(CLOCK_REALTIME, &tv);
	struct tm *timePointerEnd = localtime(&tv.tv_sec);
	size_t nbytes = strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timePointerEnd);
	snprintf(timestamp + nbytes, sizeof(timestamp) - nbytes, ".%.9ld", tv.tv_nsec);

	char threadname[30];
	threadname[0] = '\"';
	prctl(PR_GET_NAME, &threadname[1], (30 - 2));
	int len = strlen(threadname);
	threadname[len] = '\"';
	threadname[len + 1] = '\0';

	char fileline[64];
	snprintf(fileline, 64, "%s:%d", file, line);

	char cpuid_buffer[5];
	unsigned int cpuid = 999;
	getcpu(&cpuid, NULL);
	if(cpuid != 999)
	{
		sprintf(cpuid_buffer, "%d", cpuid);
	} else
	{
		cpuid_buffer[0] = '-';
		cpuid_buffer[1] = '\0';
	}

	fprintf(stdout, "%-30s %-10s cpu=%-5s %-20s %-25s msg: %-s", timestamp, "ABN", cpuid_buffer, threadname, fileline, buffer);
	fflush(stdout);
}

void LOG_DEBUG_ZZ(const char *file, int line, const char *format, ...)
{
	va_list args;
	char buffer[256];

	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	char timestamp[30];
	struct timespec tv;
	clock_gettime(CLOCK_REALTIME, &tv);
	struct tm *timePointerEnd = localtime(&tv.tv_sec);
	size_t nbytes = strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timePointerEnd);
	snprintf(timestamp + nbytes, sizeof(timestamp) - nbytes, ".%.9ld", tv.tv_nsec);

	char threadname[30];
	threadname[0] = '\"';
	prctl(PR_GET_NAME, &threadname[1], (30 - 2));
	int len = strlen(threadname);
	threadname[len] = '\"';
	threadname[len + 1] = '\0';

	char fileline[64];
	snprintf(fileline, 64, "%s:%d", file, line);

	char cpuid_buffer[5];
	unsigned int cpuid = 999;
	getcpu(&cpuid, NULL);
	if(cpuid != 999)
	{
		sprintf(cpuid_buffer, "%d", cpuid);
	} else
	{
		cpuid_buffer[0] = '-';
		cpuid_buffer[1] = '\0';
	}

	fprintf(stdout, "%-30s %-10s cpu=%-5s %-20s %-25s msg: %-s", timestamp, "DEBUG", cpuid_buffer, threadname, fileline, buffer);
	fflush(stdout);
}

