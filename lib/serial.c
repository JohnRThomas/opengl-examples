/* Copyright (c) 2015 Scott Kuhl. All rights reserved.
 * License: This code is licensed under a 3-clause BSD license. See
 * the file named "LICENSE" for a full copy of the license.
 */

/**
   @file

    Provides an easy-to-use interface to set up and do input/output
    with a serial connection.

    @author Scott Kuhl
    @author Evan Hauck (contributed early versions of read/write functions, now largely rewritten)
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>

#include "msg.h"
#include "serial.h"

#define SERIAL_DEBUG 0

/**
   Reliably write bytes to a file descriptor. Exits on failure.

   @param fd File descriptor to write to.
   @param buf Buffer containing bytes to write.
   @param numBytes Number of bytes in the buffer that should be written.
 */
void serial_write(const int fd, const char* buf, size_t numBytes)
{
	while (numBytes > 0)
	{
		ssize_t result = write(fd, buf, numBytes);
		if(result < 0)
		{
			msg(FATAL, "write error %s", strerror(errno));
			exit(EXIT_FAILURE);
		}
		// write() wrote none, some, or all of the bytes we wanted to write.
		buf += result;
		numBytes -= (size_t)result;
	}
}

/**
   Reliably read bytes from a file descriptor. Exits on failure.

   @param fd File descriptor to read from.
   @param buf Buffer containing bytes to read.
   @param numBytes Number of bytes in the buffer that be read.
   
   @param options If SERIAL_CONSUME bit is set and there are many
   bytes avaialable to read, discard some multiple of numBytes and
   return the last set of numBytes. If SERIAL_CACHED is set, the
   return value will indicate if data was read or not.

   @return Number of bytes read (which should always match
   numBytes). 0 can only be returned if SERIAL_NONBLOCK is set. -1 if
   there was a read() error.
 */
int serial_read(int fd, char* buf, size_t numBytes, int options)
{
	char *ptr = buf;
	size_t totalBytesRead = 0;

	// Determine how many bytes there are to read.
	size_t bytesAvailable = 0;
	ioctl(fd, FIONREAD, &bytesAvailable);
	if(SERIAL_DEBUG)
		printf("serial_read(): Avail to read: %d\n", (int)bytesAvailable);

	/* If SERIAL_NONBLOCK is set, and if there are not enough bytes
	 * available to read, return 0 so the caller can instead return a
	 * cached value. */
	if(bytesAvailable < numBytes &&
	   options & SERIAL_NONBLOCK)
	{
		if(SERIAL_DEBUG)
			printf("serial_read(): Timeout\n");
		return 0;
	}
	
	/* If SERIAL_CONSUME is set and if there are more than numBytes*2
	   bytes available, repeatedly read numBytes. This will eventually
	   lead to having numBytes or slightly more than numBytes
	   available for us to actually read. */
	if(options & SERIAL_CONSUME)
	{
		while(bytesAvailable >= numBytes*2)
		{
			ssize_t r = read(fd, ptr, numBytes);
			if(r == 0)
			{
				// We can get here if the USB cord is disconnected
				// from the computer. Treat it as a read error.
				if(SERIAL_DEBUG)
					printf("serial_read(): Did serial cable get disconnected?\n");
				return -1;
			}
			else if(r < 0)
			{
				if(SERIAL_DEBUG)
					printf("serial_read(): read error %s\n",  strerror(errno));
				return -1;
			}
			else
				bytesAvailable -= r;
			
			if(SERIAL_DEBUG)
			{
				// Print the bytes we consumed
				printf("serial_read(): consumed a total of %4d bytes: ", (int)r);
				for(ssize_t i=0; i<r; i++)
					printf("%x ", (unsigned char) ptr[i]);
				printf("\n");
			}
		} // end consume bytes loop

		if(SERIAL_DEBUG)
			printf("serial_read(): Avail to read after consumption: %d\n", (int)bytesAvailable);
	} // end if consume bytes


	/* Actually read the data */
	while(totalBytesRead < numBytes)
	{
		/* If SERIAL_NONBLOCK was specified and if there are not
		 * enough bytes to read, we will have returned---so it is
		 * impossible for us to get here and potentially have read()
		 * block. */
		ssize_t bytesRead = read(fd, ptr, numBytes-totalBytesRead);
		if(bytesRead == 0)
		{
			/* This can happen if the USB cable gets disconnected from the computer. */
			if(SERIAL_DEBUG)
				printf("serial_read(): Did serial cable get disconnected?\n");
			return -1;
		}
		else if(bytesRead < 0)
		{
			if(SERIAL_DEBUG)
				printf("serial_read(): read error %s\n",  strerror(errno));
			return -1;
		}
		else
		{
			// read() either read all or some of the bytes we wanted to read.
			ptr += bytesRead;
			totalBytesRead += bytesRead;
		}
	}

	if(SERIAL_DEBUG)
	{
		// Print the bytes we read
		printf("serial_read(): Read a total of %4d bytes: ", (int) totalBytesRead);
		for(size_t i=0; i<numBytes; i++)
			printf("%x ", (unsigned char) buf[i]);
		printf("\n");
	}

	/* If we didn't read the full numBytes, we would have exited. */
	return numBytes;
}


/** Applies settings to a serial connection (sets baud rate, parity, etc).

    @param fd The file descriptor corresponding to an open serial connection.
    @param speed The baud rate to be applied to the connection.
    @param parity 0=no parity; 1=odd parity; 2=even parity
    @param vmin 0 = nonblocking; if >=1, block until we have received at least vmin bytes
    @param vtime If blocking, tenths of a second we should block until we give up.

    This code is partially based on:
    https://stackoverflow.com/questions/6947413
*/
static void serial_settings(int fd, int speed, int parity, int vmin, int vtime)
{
	/* get current serial port settings */
	struct termios toptions;
	memset(&toptions, 0, sizeof(struct termios));
	if(tcgetattr(fd, &toptions) == -1)
		msg(ERROR, "tcgetattr error: %s\n", strerror(errno));

	int baud = 0;
	switch(speed)
	{
		case 100: baud = B110; break;
		case 300: baud = B300; break;
		case 600: baud = B600; break;
		case 1200: baud = B1200; break;
		case 2400: baud = B2400; break;
		case 4800: baud = B4800; break;
		case 9600: baud = B9600; break;
			//case 14400: baud = B14400; break;
		case 19200: baud = B19200; break;
			//case 28800: baud = B28800; break;
		case 38400: baud = B38400; break;
			// case 56000: baud = B56000; break;
		case 57600: baud = B57600; break;
		case 115200: baud = B115200; break;
		// other rates: 
		//case 128000: baud = B128000; break;
			// case 153600: baud = B153600; break;
			//case 256000: baud = B256000; break;
		case 460800: baud = B460800; break;
		case 921600: baud = B921600; break;
		default:
			msg(FATAL, "Invalid baud rate specified: %d\n", speed);
			exit(EXIT_FAILURE);
	}

	/* 	   
	   You can verify that the settings are correct by doing:
	   stty -F /dev/ttyUSB0 sane
	   Running this program
	   stty -F /dev/ttyUSB0
	   stty -F /dev/ttyUSB0 cs8 115200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts

	   The output of the last two commands should match. The last
	   command is what is apparently what Arduino expects based on a
	   post at http://playground.arduino.cc/Interfacing/LinuxTTY
	*/

	/* Set baud rate both directions */
	if(cfsetispeed(&toptions, baud) == -1 ||
	   cfsetospeed(&toptions, baud) == -1)
	{
		msg(ERROR, "Unable to set baud rate to %d\n", speed);
	}

	// Input flags
	toptions.c_iflag |= IGNBRK;  // set bit to ignore break condition
	toptions.c_iflag &= ~BRKINT;
	toptions.c_iflag &= ~ICRNL;
	toptions.c_iflag &= ~IMAXBEL;
	toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	// Line processing
//	toptions.c_lflag = NOFLSH; // Disable flushing in/out queues for the INT, QUIT, and SUSP characters

	// Output flags
	toptions.c_oflag = 0;

	// Character processing
	toptions.c_cflag = (toptions.c_cflag & ~CSIZE) | CS8; // 8 bit	
	toptions.c_cflag |=  (CLOCAL | CREAD);  // ignore modem controls, enable reading
	toptions.c_cflag &= ~(PARENB | PARODD); // shut off parity
	if(parity == 1)
		toptions.c_cflag |= PARENB|PARODD; // enable odd parity
	else if(parity == 2)
		toptions.c_cflag |= PARENB; // enable even parity
	toptions.c_cflag &= ~CSTOPB;  // unset two stop bits, use one stop bit
	toptions.c_cflag &= ~CRTSCTS; // disable hardware flow control

	toptions.c_cc[VMIN] = vmin; 
	toptions.c_cc[VTIME] = vtime; // If blocking, max time to block in tenths of a second (5 = .5 seconds).

//	cfmakeraw(&toptions);
	
	// Apply our new settings, discard data in buffer
	if(tcsetattr(fd, TCSAFLUSH, &toptions) == -1)
		msg(ERROR, "tcgetattr error: %s\n", strerror(errno));
}

/** Reads bytes until a specific byte pattern is found in the
 * stream. Any bytes after the pattern will not be read.

 @param fd The file descriptor to read from.
 @param bytes The bytes we are looking for.
 @param len The number of bytes in bytes.
 @param maxbytes The maximum number of bytes to read while we are looking for the pattern. Set to -1 to keep reading until pattern is found.
 @param return 1 if the pattern was found, 0 otherwise. -1 on error.
 */
int serial_find(int fd, char *bytes, int len, int maxbytes)
{
	int readbytes = 0;

	int matchIndex = 0;
	while(maxbytes < 0 || readbytes < maxbytes)
	{
		char val;
		if(serial_read(fd, &val, 1, SERIAL_NONE) == -1)
			return -1;
		readbytes++;
		
		if(*(bytes+matchIndex) == val)
		{
			matchIndex++;
			if(matchIndex == len)
				return 1;
		}
		else
			matchIndex = 0;
	}
	return 0;
}


/** Discards any bytes that are received but not read and written but
    not transmitted. */
void serial_discard(int fd)
{
	if(SERIAL_DEBUG)
		printf("serial_discard()\n");

	tcflush(fd, TCIOFLUSH);
}

/** Close a serial connection.

 @param fd The file descriptor to close.
*/
void serial_close(int fd)
{
	if(close(fd) == -1)
		msg(ERROR, "close: %s\n", strerror(errno));
}

/** Open a serial connection and applies settings to the connection.

    @param deviceFile The serial device to open (often /dev/ttyUSB0 or /dev/ttyACM0)
    @param speed The baud rate to be applied to the connection.
    @param parity 0=no parity; 1=odd parity; 2=even parity
    @param vmin 0 = nonblocking; if >1, block until we have received at least vmin bytes
    @param vtime If blocking, tenths of a second we should block until we give up.

    @return The file descriptor for the serial connection.
*/
int serial_open(const char *deviceFile, int speed, int parity, int vmin, int vtime)
{
	msg(DEBUG, "Opening serial connection to %s at %d baud\n", deviceFile, speed);
	int fd = 0;

	for(int i=0; i<10; i++)
	{
		if(i > 0 && fd == -1)
		{
			msg(ERROR, "Could not open serial connection to '%s', retrying...\n", deviceFile);
			sleep(1); // Give user time to plug in cable.
		}

#ifndef __MINGW32__
		fd = open(deviceFile, O_RDWR | O_NOCTTY);
#else
		fd = open(deviceFile, O_RDWR);
#endif
	}
	if(fd == -1)
	{
		msg(ERROR, "Failed to connect to '%s', giving up.\n", deviceFile);
		exit(EXIT_FAILURE);
	}
	
	if(!isatty(fd))
	{
		msg(FATAL, "'%s' is not a tty.\n");
		exit(EXIT_FAILURE);
	}

#ifndef __MINGW32__
	serial_settings(fd, speed, parity, vmin, vtime);
#endif

	msg(DEBUG, "Serial connection to '%s' is open on fd=%d.\n", deviceFile, fd);
	return fd;
}
