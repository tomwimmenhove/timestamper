#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <getopt.h>

// Command line options
int verbose = 0;
int debug = 0;
char* time_format = "%a, %d %b %Y %T.%N %z";

// Thanks. https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
int set_interface_attribs (int fd, int speed, int parity)
{
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0)
	{
		fprintf(stderr, "error %d from tcgetattr", errno);
		return -1;
	}

	cfsetospeed (&tty, speed);
	cfsetispeed (&tty, speed);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tty.c_iflag &= ~IGNBRK;         // disable break processing
	tty.c_lflag = 0;                // no signaling chars, no echo,
	// no canonical processing
	tty.c_oflag = 0;                // no remapping, no delays
	tty.c_cc[VMIN]  = 0;            // read doesn't block
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
	// enable reading
	tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr (fd, TCSANOW, &tty) != 0)
	{
		fprintf(stderr, "error %d from tcsetattr", errno);
		return -1;
	}
	return 0;
}

uint64_t GetUtcMicros()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	return (unsigned long long)(tv.tv_sec) * 1000000 + (unsigned long long)(tv.tv_usec);
}

#define HZ 100000000 /* XXX: This is a macro for convenience. It won't work with different values, though! */

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
		printf("%08d%s\n", frac, right);
	}
	else
	{
		printf("\n");
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

	snprintf(s, sizeof(s), "%ld.%08ld", x / HZ, x % HZ);

	return s;
}

void handle_event(uint32_t frac)
{
	/* Get the local time in 'steps' of 1/HZ */
	int64_t local = GetUtcMicros() * (HZ / 1000000);

	/* Round to the nearest second */
	int64_t top_s = (local - frac + (HZ / 2)) / HZ;

	/* Get the actual timestamp in 'steps' of 1/HZ */
	int64_t ts = top_s * HZ + frac;

	/* Get the error */
	int64_t err = local - ts;

	if (verbose > 0) printf("\nError     : %ss\n", fp_str(err));
	if (verbose > 1) printf("Local     : %ss\n", fp_str(local));
	if (verbose > 2) printf("Timestamp : %ss\n", fp_str(ts));

	if (debug)
	{
		system("date \"+%a, %d %b %Y %T.%N %z SYSTEM DATETIME\"");
	}

	if (err > 10000000 || err < -10000000)
		printf("*"); // Let the user know this is an untrustworthy measurement (> 100ms discrepancy)
	print_time(top_s, frac, time_format);

	fflush(stdout);
}

static inline uint32_t unpack(uint8_t* out)
{
	return (out[0] & 0x7f) | (out[1] << 7) | (out[2] << 14) | (out[3] << 21);
}

void print_usage(char* name)
{
	fprintf(stderr, "Usage: %s [options] <serial port>\n", name);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "\t--verbose, -v : Increase verbosity\n");
	fprintf(stderr, "\t--debug,   -d : Debug (Print system date on each event)\n");
	fprintf(stderr, "\t--format,  -f : Specify the time format (default: \"%s\")\n", time_format);
	fprintf(stderr, "\t--help,    -h : This\n");
}

int main(int argc, char** argv)
{
	int c;
	while (1)
	{
		static struct option long_options[] =
		{
			{"verbose", no_argument,       0, 'v'},
			{"debug",   no_argument,       0, 'd'},
			{"format",  required_argument, 0, 'f'},
			{"help",    no_argument,       0, 'h'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "vdf:",
				long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c)
		{
			case 'v':
				verbose++;
				break;
			case 'd':
				debug = 1;
				break;
			case 'h':
				print_usage(argv[0]);
				return 0;
			case 'f':
				time_format = optarg;
				break;

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

	if (set_interface_attribs(serfd, B38400, 0) == -1)
		return 1;

	tcflush(serfd,TCIOFLUSH);

	uint8_t buf[4096];
	uint8_t packet[4];
	int pos = 0;
	int n;
	while ((n = read(serfd, buf, sizeof(buf))) != -1)
	{
		if (!n)
			continue;

//		printf("n: %d\n", n);

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
				handle_event(x % HZ);
			}
			if (pos > 4)
			{
				fprintf(stderr, "Packet too large! Data ignored.\n");
				return 1;
			}
		}
	}

	return 0;
}
