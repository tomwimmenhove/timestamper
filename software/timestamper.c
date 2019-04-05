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
#include <math.h>

#include "serial.h"
#include "cdce925.h"
#include "command.h"

#define VERSION_MAJOR	0
#define VERSION_MINOR	1
#define VERSION_MICRO	2

#define NS_TOL 100 /* Warn when a capture with a value higher than NS + NS_TOL is received. */

#define NS 1000000000

// Command line options
#define DBG_DUMB_OPT 1000
#define DBG_DELTA_OPT 1001
int verbose = 0;
int hide_unreliable = 0;
int prec = 9;
int dumb = 0;
int delta = 0;
char* time_format = "%a, %d %b %Y %T.%N %z";
double ref_clk = 10e6;

int clock_freq;

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
	int min = 0;

	if (x < 0)
	{
		x = -x;
		min = 1;
	}

	snprintf(s, sizeof(s), "%s%ld.%09ld", min ? "-" : "" , x / NS, x % NS);

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

	int unreliable = err > 100000000 || err < -100000000 || out_of_tol;
	if (unreliable && hide_unreliable)
		return;

	if (verbose > 1) printf("\nLocal     : %ss\n", fp_str(local));
	if (verbose > 2) printf("Timestamp : %ss\n", fp_str(ts));

	if (unreliable)
		printf("*"); // Let the user know this is an untrustworthy measurement (> 100ms discrepancy)
	print_time(top_s, frac, time_format);

	if (verbose > 0) printf(" -- Error: %ss", fp_str(err));

	putchar('\n');
}

void handle_event(uint32_t x)
{
	/* Fixed point from counter value to nanoseconds */
	handle_frac((uint32_t) ((uint64_t) x * NS / clock_freq));
}

void print_usage(char* name)
{
	fprintf(stderr, "Usage: %s [options] <serial port>\n", name);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "\t--format,  -f : Specify the time format (default: \"%s\")\n", time_format);
	fprintf(stderr, "\t--help,    -h : This\n");
	fprintf(stderr, "\t--hide,    -r : Hide unreliable timestamps (instead of denoting them with an asterisk (*))\n");
	fprintf(stderr, "\t--prec,    -p : Set the number of digits after the decimal point (0..9) [%d]\n", prec);
	fprintf(stderr, "\t--refclk   -c : Set the reference clock in Hz [%f]\n", ref_clk);
	fprintf(stderr, "\t--sysclk   -s : Set the PLL output clock in the format of \"PLL:DIV\" in Hz (I.E. 200e6:2 would be 100MHz)\n");
	fprintf(stderr, "\t--auxclk   -a : Same as --sysclk, but for the auxiliary output\n");
	fprintf(stderr, "\t--pllinit, -i : Write 'factory defaults' to PLL\n");
	fprintf(stderr, "\t--write,   -W : Write the PLL configuration to non-volatile memory\n");
	fprintf(stderr, "\t--exit,    -e : Don't output timesamps; exit after initialization and configuration\n");
	fprintf(stderr, "\t--dbgdumb     : Debug: Only print the counter value\n");
	fprintf(stderr, "\t--dbgdelta    : Debug: Only print the differences between counter values\n");

	fprintf(stderr, "\t--verbose, -v : Increase verbosity\n");
	fprintf(stderr, "\t--version, -V : Show version information\n");
}

void print_version()
{
	printf("Timestamper version %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);
}

double read_pll_config(int fd, int pll_id)
{
	int pdiv = pll_id == 0 ? cdce925_get_clockdiv(fd) : cdce925_get_auxdiv(fd);

    struct cdce_nrqp nrqp;
    cdce925_get_nrqp(&nrqp, fd, pll_id);

    int m = cdce925_calc_m(nrqp.p, nrqp.q, nrqp.r, nrqp.n);
    double pll = (double) ref_clk * nrqp.n / m;
    double f = ((double) pll / pdiv);

    if (verbose)
    {
        printf("%s PLL Configuration: \n", pll_id == 0 ? "System clock" : "Auxiliary clock");
        printf("  Ref   : %fHz\n", ref_clk);
        printf("  N     : %d\n", nrqp.n);
		if (verbose > 1)
		{
			printf("  R     : %d\n", nrqp.r);
			printf("  Q     : %d\n", nrqp.q);
			printf("  P     : %d\n", nrqp.p);
		}
        printf("  M     : %d\n", m);
        printf("  Osc   : %fHz\n", pll);
        printf("  Div   : %d\n", pdiv);
        printf("  Clock : %fHz\n", f);
    }

	return f;
}

void parse_divclock(char* s, double* f, int* div)
{
	char* del = strstr(s, ":");
	if (del == NULL)
	{
		fprintf(stderr, "Clock not given in the correct format!\n");
		exit(1);
	}
	
	*del = 0;

	*f = atof(s);
	*div = atoi(del + 1);

	if (*f < 80e6 || *f > 230e6)
	{
		fprintf(stderr, "PLL frequency must be between 80 and 230MHz\n");
		exit(1);
	}
}

void print_exclusive_pll_set_error()
{
	fprintf(stderr, "Can't use --sysclk or --auxclk together with --pllinit\n");
	exit(1);
}

int main(int argc, char** argv)
{
	int set_sys = 0;
	int set_aux = 0;

	int pll_init = 0;
	int pll_write = 0;

	double sys_clk;
	int sys_div;
	double aux_clk;
	int aux_div;

	int pll_wait = 0;
	int exit_after = 0;

	int c;
	while (1)
	{
		static struct option long_options[] =
		{
			{"format",  required_argument, 0, 'f'},
			{"help",    no_argument,       0, 'h'},
			{"hide",    no_argument,       0, 'r'},
			{"exit",    no_argument,       0, 'e'},
			{"dbgdumb", no_argument,       0, DBG_DUMB_OPT },
			{"dbgdelta",no_argument,       0, DBG_DELTA_OPT },
			{"prec",    required_argument, 0, 'p'},
			{"refclk",  required_argument, 0, 'c'},
			{"sysclk",  required_argument, 0, 's'},
			{"auxclk",  required_argument, 0, 'a'},
			{"pllinit", no_argument,       0, 'i'},
			{"write",   no_argument,       0, 'W'},
			{"verbose", no_argument,       0, 'v'},
			{"version", no_argument,       0, 'V' },
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "vf:hrp:c:s:a:iWe",
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
			case 'c':
				ref_clk = atof(optarg);
				break;
			case 'p':
				prec = atoi(optarg);
				if (prec < 0 || prec > 9)
				{
					print_usage(argv[0]);
					return 1;
				}
				break;

			case 's':
				if (pll_init)
					print_exclusive_pll_set_error();

				parse_divclock(optarg, &sys_clk, &sys_div);
				set_sys = 1;
				if (sys_div < 1 || sys_div > 255)
				{
					fprintf(stderr, "System clock divider must be between 1 and 255\n");
					return 1;
				}
				break;

			case 'a':
				if (pll_init)
					print_exclusive_pll_set_error();

				parse_divclock(optarg, &aux_clk, &aux_div);
				set_aux = 1;
				if (aux_div < 1 || aux_div > 127)
				{
					fprintf(stderr, "Auxiliary lock divider must be between 1 and 127\n");
					return 1;
				}
				break;

			case 'i':
				if (set_aux || set_sys)
					print_exclusive_pll_set_error();
				pll_init = 1;
				break;

			case 'W':
				pll_write = 1;
				break;

			case DBG_DUMB_OPT:
				dumb = 1;
				break;

			case DBG_DELTA_OPT:
				dumb = delta = 1;
				break;

			case 'e':
				exit_after = 1;
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
		perror(serial);
		return 1;
	}

	if (set_interface_attribs(serfd, 76800) == -1)
		return 1;

	tcflush(serfd,TCIOFLUSH);

	/* Wait until the device is ready... */
	wait_reply(serfd, 0x8000000);

	if (pll_init)
		send_command(serfd, 0x8050000);
	
	if (set_sys)
	{
		struct cdce_nrqp nrqp;
		double f = cdce925_find_best(&nrqp, ref_clk, sys_clk);
		if (f != sys_clk)
			fprintf(stderr, "WARNING: An exact PLL configuration for the requested system clock frequency could not be found. Using closest match: %fHz\n", f);

		cdce925_set_nrqp(&nrqp, serfd, 0, cdce925_getrange(f));
		cdce925_set_clockdiv(serfd, sys_div);

		pll_wait = 1;
	}

	/* Read system clock PLL configuration */
	double f = read_pll_config(serfd, 0);
	clock_freq = (int) round(f);
	if (f != clock_freq)
		fprintf(stderr, "WARNING: resulting PLL frequency is not an interger value. The value will be rounded!\n");

	if (set_aux)
	{
		struct cdce_nrqp nrqp;
		double f = cdce925_find_best(&nrqp, ref_clk, aux_clk);
		if (f != aux_clk)
			fprintf(stderr, "WARNING: An exact PLL configuration for the requested auxiliary clock frequency could not be found. Using closest match: %fHz\n", f);

		cdce925_set_nrqp(&nrqp, serfd, 1, cdce925_getrange(f));
		cdce925_set_auxdiv(serfd, aux_div);

		pll_wait = 1;
	}
	
	/* Read Auxiliary clock configuration */
	read_pll_config(serfd, 1);

	if (pll_write)
		send_command(serfd, 0x8060000);

	if (exit_after)
		return 0;

	/* Give PLL time to settle */
	if (pll_wait)
		usleep(1000000);

	/* Start */
	send_command(serfd, 0x8010000);

	uint8_t packet[4];
	int pos = 0;
	uint8_t buf[4096];
	ssize_t n;
	uint32_t last_value = 0;
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

				if (dumb)
				{
					int32_t v = x;

					if (delta)
					{
						v -= last_value;
						if (v < 0)
							v += 134217728;
					}

					printf("%d\n", v);
					last_value = x;
				}
				else
				{
					handle_event(x);
				}

				fflush(stdout);
			}
			if (pos > 4)
			{
				fprintf(stderr, "Packet too large!\n");// Data ignored.\n");
				exit(1);
			}
		}
	}

	return n != -1 ? 0 : 1;
}
