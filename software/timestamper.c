/* 
 * This file is part of the TimeStamper distribution (https://github.com/tomwimmenhove/timestamper)
 * Copyright (c) 2019 Tom wimmenhove
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <getopt.h>
#include <termios.h>

#include "serial.h"

#define VERSION_MAJOR	0
#define VERSION_MINOR	1
#define VERSION_MICRO	0

#define PLL_HZ 100000000
#define NS_TOL 100 /* Warn when a capture with a value higher than NS + NS_TOL is received. */

#define NS 1000000000

// Command line options
int verbose = 0;
int hide_unreliable = 0;
int prec = 9;
char* time_format = "%a, %d %b %Y %T.%N %z";

uint64_t GetUtcMicros()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	return (unsigned long long)(tv.tv_sec) * 1000000 + (unsigned long long)(tv.tv_usec);
}

void print_time(int64_t top, uint32_t frac, const char* format)
{
	time_t now_s = top;
	struct tm *now_tm;
	char s[256];

	now_tm = localtime(&now_s);
	if (strftime(s, sizeof(s) - 1, format, now_tm) <= 0)
	{   
		fprintf(stderr, "strftime() failed.\n");
		exit(1);
	}

	char* nsp = strstr(s, "%N");
	if (nsp)
	{   
		*nsp = 0;
	}
	printf("%s", s);
	if (nsp)
	{   
		char* right = nsp + 2;

		char s1[16];
		snprintf(s1, sizeof(s1), "%09d", frac);
		s1[prec] = 0; // truncate string

		printf("%s%s", s1, right);
	}
}

char* fp_str(int64_t x)
{
	static char s[32];

	if (x < 0)
	{
		x = -x;
		printf("-");
	}

	snprintf(s, sizeof(s), "%ld.%09ld", x / NS, x % NS);

	return s;
}

void handle_frac(uint32_t frac)
{
	int out_of_tol = frac > NS + NS_TOL;

	if (out_of_tol && verbose)
	{
		fprintf(stderr, "WARNING: read a capture count of %u, which is out of tolerance! (%u is the maximum bound)\n", frac, NS + NS_TOL);
	}

	frac %= NS;

	/* Get the local time in 'steps' of 1/NS */
	int64_t local = GetUtcMicros() * (NS / 1000000);

	/* Round to the nearest second */
	int64_t top_s = (local - frac + (NS / 2)) / NS;

	/* Get the actual timestamp in 'steps' of 1/NS */
	int64_t ts = top_s * NS + frac;

	/* Get the error */
	int64_t err = local - ts;

	int unreliable = err > 10000000 || err < -10000000 || out_of_tol;
	if (unreliable && hide_unreliable)
		return;

	if (verbose > 1) printf("\nLocal     : %ss\n", fp_str(local));
	if (verbose > 2) printf("Timestamp : %ss\n", fp_str(ts));

	if (unreliable)
		printf("*"); // Let the user know this is an untrustworthy measurement (> 100ms discrepancy)
	print_time(top_s, frac, time_format);

	if (verbose > 0) printf(" -- Error: %ss", fp_str(err));

	putchar('\n');
	fflush(stdout);
}

void handle_event(uint32_t x)
{
	/* Fixed point from counter value to nanoseconds */
	handle_frac((uint32_t) ((uint64_t) x * NS / PLL_HZ));
}

static inline uint32_t unpack(uint8_t* out)
{
	return (out[0] & 0x7f) | (out[1] << 7) | (out[2] << 14) | (out[3] << 21);
}

void print_usage(char* name)
{
	fprintf(stderr, "Usage: %s [options] <serial port>\n", name);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "\t--format,  -f : Specify the time format (default: \"%s\")\n", time_format);
	fprintf(stderr, "\t--help,    -h : This\n");
	fprintf(stderr, "\t--hide,    -r : Hide unreliable timestamps (instead of denoting them with an asterisk (*))\n");
	fprintf(stderr, "\t--prec,    -p : Set the number of digits after the decimal point (0..9)\n");
	fprintf(stderr, "\t--verbose, -v : Increase verbosity\n");
	fprintf(stderr, "\t--version, -V : Show version information\n");
}

void print_version()
{
	printf("Timestamper version %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);
}

int main(int argc, char** argv)
{
	int c;
	while (1)
	{
		static struct option long_options[] =
		{
			{"format",  required_argument, 0, 'f'},
			{"help",    no_argument,       0, 'h'},
			{"hide",    no_argument,       0, 'r'},
			{"prec",    required_argument, 0, 'p'},
			{"verbose", no_argument,       0, 'v'},
			{"version", no_argument,       0, 'V' },
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "vf:hrp:",
				long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c)
		{
			case 'f':
				time_format = optarg;
				break;
			case 'h':
				print_usage(argv[0]);
				return 0;
			case 'r':
				hide_unreliable = 1;
				break;
			case 'p':
				prec = atoi(optarg);
				if (prec < 0 || prec > 9)
				{
					print_usage(argv[0]);
					return 1;
				}

				break;
			case 'v':
				verbose++;
				break;
			case 'V':
				print_version();
				return 0;

			default:
				print_usage(argv[0]);
				return 1;
		}
	}

	if (argc != optind + 1)
	{
		print_usage(argv[0]);
		return 1;
	}

	char* serial = argv[optind];

	int serfd = open(serial, O_RDWR);
	if (serfd == -1)
	{

		int serfd = open(serial, O_RDWR);
		if (serfd == 0-1)
		{
			perror(serial);
			return 1;
		}
	}

	//if (set_interface_attribs(serfd, B38400) == -1)
	if (set_interface_attribs(serfd, 76800) == -1)
		return 1;

	tcflush(serfd,TCIOFLUSH);

	uint8_t buf[4096];
	uint8_t packet[4];
	int pos = 0;
	int n;
	while ((n = read(serfd, buf, sizeof(buf))) > 0)
	{
		for (int i = 0; i < n; i++)
		{
			uint8_t b = buf[i];

			/* Start of 'packet' */
			if (b & 0x80)
			{
				pos = 0;
			}
			if (pos < 4)
			{
				packet[pos] = b;
			}

			pos++;

			if (pos == 4)
			{
				uint32_t x = unpack(packet);

				handle_event(x);
			}
			if (pos > 4)
			{
				fprintf(stderr, "Packet too large! Data ignored.\n");
				return 1;
			}
		}
	}

	return n == 0 ? 0 : 1;
}
