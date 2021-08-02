#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include "usart.h"


int Serial_init(void)
{
	Serial_fd = 0;
	Serial_addr = "/dev/ttyAMA0";
    Serial_fd = open(Serial_addr, O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == Serial_fd)
    {
        // ERROR
        return (-1);
    }
    return 0;
}

void Serial_close(void)
{
    close(Serial_fd);
}

void Serial_Transmit(const void* buff, int len)
{
    write(Serial_fd, buff, len);
}

void Serial_Set(void)
{
	struct termios options;
	tcgetattr(Serial_fd,&options);
	cfsetispeed(&options,B19200);
	cfsetospeed(&options,B19200);
	options.c_cflag |= CLOCAL | CREAD;
	options.c_oflag &= ~OPOST;
	options.c_lflag &= ~(ICANON | ISIG | ECHO | ECHOE | ECHOK | ECHONL);
	options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	tcflush(Serial_fd,TCIFLUSH);
	tcsetattr(Serial_fd,TCSANOW,&options);
}
