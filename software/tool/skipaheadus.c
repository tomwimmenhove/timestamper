#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

int main(int argc, char** argv)
{
	struct timeval tv;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <microseconds>\n", argv[0]);

		return 1;
	}

	int skip = atoi(argv[1]);

	if (gettimeofday(&tv, NULL) != 0)
	{
		perror("gettimeofday()");
		return 1;
	}

	uint64_t t = (uint64_t) tv.tv_usec + (uint64_t) tv.tv_sec * 1000000;

	t += skip;

	tv.tv_usec = t % 1000000;
	tv.tv_sec = t / 1000000;

	if (settimeofday(&tv, NULL) != 0)
	{
		perror("settimeofday()");
		return 1;
	}

	return 0;
}
