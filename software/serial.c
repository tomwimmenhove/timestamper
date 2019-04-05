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

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "serial.h"

#ifdef __linux__

#include <asm/termios.h>
#include <stropts.h>

// Thanks. https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
int set_interface_attribs (int fd, int speed)
{
    struct termios2 tty;
    memset (&tty, 0, sizeof tty);
    //if (tcgetattr (fd, &tty) != 0)
    if(ioctl(fd, TCGETS2, &tty) == -1)
    {   
        fprintf(stderr, "error %d from tcgetattr", errno);
        return -1;
    }

    tty.c_cflag &= ~CBAUD;
    tty.c_cflag |= BOTHER;

    tty.c_ispeed = speed;
    tty.c_ospeed = speed;

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | ECHONL | IEXTEN);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL );
    tty.c_oflag &= ~OPOST;
    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 0;

    if(ioctl(fd, TCSETS2, &tty) == -1)
    {
        fprintf(stderr, "error %d from tcsetattr", errno);
        return -1;
    }
    return 0;
}

#else

#include <termios.h>

int set_interface_attribs (int fd, int speed)
{
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0)
	{
		fprintf(stderr, "error %d from tcgetattr", errno);
		return -1;
	}

	cfsetospeed(&tty, speed);
	cfsetispeed(&tty, speed);

	tty.c_cflag |= (CLOCAL | CREAD);
	tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | ECHONL | IEXTEN);
	tty.c_cflag &= ~PARENB;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;
	//tty.c_cflag &= ~CNEW_RTSCTS;
	tty.c_cflag &= ~CRTSCTS;
	tty.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL );
	tty.c_oflag &= ~OPOST;
	tty.c_cc[VMIN]  = 1;
	tty.c_cc[VTIME] = 0;

	if (tcsetattr (fd, TCSANOW, &tty) != 0)
	{
		fprintf(stderr, "error %d from tcsetattr", errno);
		return -1;
	}
	return 0;
}

#endif
