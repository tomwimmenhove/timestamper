#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>

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

static inline uint32_t unpack(uint8_t* out)
{
	return (out[0] & 0x7f) | (out[1] << 7) | (out[2] << 14) | (out[3] << 21);
}


int main(int argc, char** argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <serial point>\n", argv[0]);

		return 1;
	}

	char* serial = argv[1];

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
				printf("%d\n", x);
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
