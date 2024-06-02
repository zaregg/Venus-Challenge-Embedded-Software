#include <libpynq.h>
#include <stdio.h>
#include <stdlib.h>
#include <iic.h>

#include "QMC5883L.h"

int iic_scan(int iic) {
  int num_devices = 0;
  for (int addr = 0; addr < 128; addr++) {
    if (iic_write_register(iic, addr, 0x00, 0x00, 0) == 0) {
      printf("Device found at address 0x%02X\n", addr);
      num_devices++;
    }
  }
  return num_devices;
}

int main(void) {
  pynq_init();

  switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL);
	switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA);
	iic_init(IIC0);

  // Scan I2C bus
  int num_devices = iic_scan(IIC0);
  printf("Number of devices found: %d\n", num_devices);

  // Initialize QMC5883L
  QMC5883L compass;
  QMC5883L_init(&compass);
  printf("QMC5883L initialized\n");

  // Read data
  int r;
  int16_t x,y,z,t;
  r = QMC5883L_readRaw(&compass,&x,&y,&z,&t);
  printf("Read data\n");

  if (r == 0) {
    printf("Error reading data\n");
  }
  printf("x: %d\n", x);
  printf("y: %d\n", y);
  printf("z: %d\n", z);
  printf("t: %d\n", t);

  // QMC5883L_setSamplingRate(&compass, 10);

  int heading = QMC5883L_readHeading(&compass);

  printf("Heading: %d\n", heading);

  while (1) {
    if (QMC5883L_ready(&compass)) {
      // r = QMC5883L_readRaw(&compass,&x,&y,&z,&t);
      // printf("Read data\n");

      // if (r == 0) {
      //   printf("Error reading data\n");
      // }
      // printf("x: %d, ", x);
      // printf("y: %d, ", y);
      // printf("z: %d, ", z);
      // printf("t: %d\n", t);

      heading = QMC5883L_readHeading(&compass);
      printf("Heading: %d\n", heading);
      // sleep_msec(100);
    }
  }



  pynq_destroy();
  return EXIT_SUCCESS;
}
