/* rs232_send.c */
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyTHS1"
#define _POSIX_SOURCE 1
#define STOP '@'

int main()
{
	int fd, c=0, res;
	struct termios oldtio, newtio;
	char ch;
	static char s1[20];

	printf("Start...\n");
	fd = open(MODEMDEVICE, O_RDWR|O_NOCTTY);
	if (fd < 0) {
		perror(MODEMDEVICE);
		exit(1);
	}

	printf("Open...\n");
	tcgetattr(fd, &oldtio);
	bzero(&newtio, sizeof(newtio));

	newtio.c_cflag = BAUDRATE|CS8|CLOCAL|CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = ICANON;

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);

	printf("Writing...\n");

	while(1) {
		while((ch=getchar()) != STOP) {
			s1[0]=ch;
			res=write(fd, s1, 1); 
		}
		s1[0]=ch;
		s1[1]='\n';
		res = write(fd, s1, 2); 
		break;
	}
	printf("Close...\n");
	close(fd);
	tcsetattr(fd, TCSANOW, &oldtio);
	return 0;
}
