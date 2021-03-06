#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/select.h>

int configure_serial_port(int fd){
  struct termios port_settings;

  if (tcgetattr(fd, &port_settings) < 0) {
    printf("Error from tcgetattr: %s\n", strerror(errno));
    return -1;
  }

  cfsetispeed(&port_settings, B115200);
  cfsetospeed(&port_settings, B115200);

  port_settings.c_cflag |= (CLOCAL | CREAD);
  port_settings.c_cflag |= PARENB; //Enable Parity Bit

  port_settings.c_oflag &= ~OPOST; //Disable Output Process

  // Timeout
  port_settings.c_cc[VMIN] = 0;
  port_settings.c_cc[VTIME] = 10;

  // Set Data Bits Size is 8 Bit
  port_settings.c_cflag &= ~CSIZE;
  port_settings.c_cflag |= CS8;

  // Clear flush
  tcflush(fd, TCIOFLUSH);
  // Set Port Setting
  tcsetattr(fd, TCSANOW, &port_settings);
}

int serial_test(char *pszDevName, char *data){
  char buffer[255];
  char *pbuffer;
  char szCMD[255] = {0};
  int nbytes;
  int result = -1;
  memset(buffer, 0, sizeof(buffer));

  int fd = open (pszDevName, O_RDWR | O_NOCTTY | O_NDELAY);
 
  printf("pszDevName:%s\n",pszDevName);
  if (fd < 0)
  {
  #ifdef _DEBUG_
    printf ("open(%s) failed, error(%d) : %s", pszDevName, errno, strerror(errno));
  #endif
    return -1;
  }
  else{
    fcntl(fd, F_SETFL, 0);
  }
  //printf("fd:%d\n",fd);
  //printf("configure_serial_port(fd):%d\n",configure_serial_port(fd));

  if (configure_serial_port(fd) < 0){
    printf("error");
    return -1;
  }

  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(fd, &read_fds);
  struct timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  for(int count=0;count<3;count++){ 
    //printf("count:%d\n",count);
    int debug;
    int debug2;
    debug = write(fd, "Aetina\r",7);
    //printf("debug:%d\n",debug);
    if (debug < 7)
      continue;
    //if (write(fd, "Aetina\r",7) < 7)
      //continue;
    //printf("select...\n");
    debug2 = select(fd+1, &read_fds, NULL, NULL, &timeout);
    //printf("select-debug:%d\n",debug2);
    if (select(fd+1, &read_fds, NULL, NULL, &timeout) > 0){
      
      pbuffer = buffer;
      printf("buffer:%s\n",pbuffer);
      while((nbytes = read(fd, pbuffer, buffer+sizeof(buffer)-pbuffer-1))>0){
        pbuffer += nbytes;
        if (pbuffer[-1] == '\n' || pbuffer[-1] == '\r'){
          break;
        }
      }
    
      *pbuffer = '\0';
      if (strncmp(buffer, "Aetina", 6) == 0){
      #ifdef _DEBUG_
        printf("Response:%s\n", buffer);
      #endif
        result = 0;
        break;
      }
    } 
  
  }

  close(fd);
  return result;
}

int serial_test_for_CSI(char *pszDevName, char *data){
  char buffer[255];
  char *pbuffer;
  char szCMD[255] = {0};
  int nbytes;
  int result = -1;
  memset(buffer, 0, sizeof(buffer));

  int fd = open (pszDevName, O_RDWR | O_NOCTTY | O_NDELAY);
  sprintf(szCMD,"i2cset -f -y 0 0x77 0x3 0xfb");
	if(SystemCMD(szCMD) < 0){
		printf("poweron-failed\n");}
	else{ 
		printf("poweron\n");}
  printf("pszDevName:%s\n",pszDevName);
  if (fd < 0)
  {
  #ifdef _DEBUG_
    printf ("open(%s) failed, error(%d) : %s", pszDevName, errno, strerror(errno));
  #endif
    return -1;
  }
  else{
    fcntl(fd, F_SETFL, 0);
  }
  //printf("fd:%d\n",fd);
  //printf("configure_serial_port(fd):%d\n",configure_serial_port(fd));

  if (configure_serial_port(fd) < 0){
    printf("error");
    return -1;
  }

  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(fd, &read_fds);
  struct timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  for(int count=0;count<3;count++){ 
    //printf("count:%d\n",count);
    int debug;
    int debug2;
    debug = write(fd, "Aetina\r",7);
    //printf("debug:%d\n",debug);
    if (debug < 7)
      continue;
    //if (write(fd, "Aetina\r",7) < 7)
      //continue;
    //printf("select...\n");
    debug2 = select(fd+1, &read_fds, NULL, NULL, &timeout);
    //printf("select-debug:%d\n",debug2);
    if (select(fd+1, &read_fds, NULL, NULL, &timeout) > 0){
      //printf("select is true.");
      pbuffer = buffer;
      //printf("buffer:%s\n",pbuffer);
      while((nbytes = read(fd, pbuffer, buffer+sizeof(buffer)-pbuffer-1))>0){
        pbuffer += nbytes;
        if (pbuffer[-1] == '\n' || pbuffer[-1] == '\r'){
          break;
        }
      }
     sprintf(szCMD,"i2cset -f -y 0 0x77 0x3 0xf9");
      //if(SystemCMD(szCMD) < 0){
		//printf("power off Failed\n");}
     // else{ 
		//printf("poweroff\n");
	}
      *pbuffer = '\0';
      if (strncmp(buffer, "Aetina", 6) == 0){
      #ifdef _DEBUG_
        printf("Response:%s\n", buffer);
      #endif
        result = 0;
        break;
      }
    } 
//printf("select...done\n");   

  close(fd);
  return result;
}
