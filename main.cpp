#include <iostream>
#include <future>
#include <thread>

#include <dev/serialdevice.h>
#include <dev/serialdevicedriver.h>

std::promise<double> promise;
 
int main(int argc, char **argv) {
  dev::SerialDevicePtr dev = std::make_shared<dev::SerialDevice>("/dev/ttyUSB0");
  dev::drivers::rs232::SerialDeviceDriver driver(dev);
  
  driver.inc(1);
  std::cout << "Hello, world!\n" << std::endl;
  
  int a = 0;
  std::cin >> a;
  return 0;
}

