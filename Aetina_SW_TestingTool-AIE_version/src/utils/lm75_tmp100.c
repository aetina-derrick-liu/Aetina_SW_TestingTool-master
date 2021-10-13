#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

int i2c_read_data(int* i2c_bus_id, char* i2c_address, char* buffer){
  int file;
  char i2c_bus[40] = {0};
  
  sprintf(i2c_bus, "/dev/i2c-%d", i2c_bus_id);
  if ((file = open(i2c_bus, O_RDWR)) < 0){
    return -1;
  }

  if (ioctl(file, I2C_SLAVE_FORCE, i2c_address) < 0){
    close(file);
    return -2;
  }

  char reg[1] = {0x00};
  write(file, reg, 1);
  if (read(file, buffer, sizeof(buffer)) != sizeof(buffer)){
    printf("Error : Input/Output error \n");
  }

  close(file);
  return 0;
}