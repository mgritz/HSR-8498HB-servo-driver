/** serial.h
 *
 *  Created on: 10.12.2014
 *      Author: martin
 *
 *      Source: www.mikrocontroller.net
 *      slightly modified to fit the Robonova's needs
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdio.h> // standard input / output functions
#include <string.h> // string function definitions
#include <fcntl.h> // File control definitions
#include <errno.h> // Error number definitions
#include <termios.h> // POSIX terminal control definitionss
#include <sys/ioctl.h> // IO-Control-headers
#include <unistd.h>

/* Open and initialise serial port */
static int fnInitV24(const char * device) {
	int iFd;
	int modelines = 0;
	struct termios settings;

	/* open and check filedescriptor */
	iFd = open(device, O_RDWR | O_NOCTTY);

	if (iFd < 0) {
		fprintf(stderr, "Can not open device %s (errno: %s)!\n", device,
				strerror(errno));
		return (iFd);
	}

	/* Check if filedescriptor belongs to a terminal */
	if (!isatty(iFd)) {
		fprintf(stderr, "Device %s is not a terminal device (errno: %s)!\n",
				device, strerror(errno));
		close(iFd);
		return (-1);
	}

	/* Set input flags */
	settings.c_iflag = IGNBRK /* Ignore BREAKS on Input */
			| IGNPAR; /* No Parity              */

	/* Set output flags */
	settings.c_oflag = 0;

	/* Set controlflags */
	settings.c_cflag = CS8 	/* 8 bits per byte */
			| CREAD 	/* characters may be read */
			| CLOCAL 	/* ignore modem state, local connection */
			| CSTOPB; 	/* set two stop bits */

	/* Set local flags */
	settings.c_lflag = 0;

	/* Set maximum wait time on input */
	settings.c_cc[VTIME] = 10;

	/* Set minimum bytes to read */
	settings.c_cc[VMIN] = 0;

	/* set speed */
	if (cfsetspeed(&settings, B19200) != 0) {
		fprintf(stderr, "Can not set communication speed to %s (errno: %s)!\n",
				device, strerror(errno));
		return (-1);
	}

	/* transfer setup to interface */
	if (tcsetattr(iFd, TCSANOW, &settings) < 0) {
		fprintf(stderr,
				"Error setting terminal attributes for %s (errno: %s)!\n",
				device, strerror(errno));
		return (-1);
	}

	/* Read IO parameter */
	if (ioctl(iFd, TIOCMGET, &modelines) < 0) {
		fprintf(stderr, "Error getting IO parameter from %s (errno: %s)!\n",
				device, strerror(errno));
		return (-1);
	}
	return (iFd);
}

/* Close serial port */
static int fnCloseV24(int iFd) {
	return (close(iFd));
}

/* Write to serial port */
static int fnWriteV24(int iFd, char *pszOut, size_t tLen) {
	return (write(iFd, pszOut, tLen));
}

/* Read from serial port */
static int fnReadV24(int iFd, char *pszIn, size_t tLen) {
	int iNrRead;
	iNrRead = read(iFd, pszIn, tLen);

	return (iNrRead);
}

/*  Set timeout on tty input to new value, returns old timeout */
static int fnSetTimeout(int iFd, int iTimeout) {
	struct termios stTerminal;
	int iOldTimeout;

	/* get current settings */
	tcgetattr(iFd, &stTerminal);

	/* get old timeout */
	iOldTimeout = stTerminal.c_cc[VTIME];

	/* set timeout in 10th of seconds */
	stTerminal.c_cc[VTIME] = iTimeout;

	/* set new status */
	tcsetattr(iFd, TCSANOW, &stTerminal);

	return (iOldTimeout);
}
#endif /* SERIAL_H_ */
