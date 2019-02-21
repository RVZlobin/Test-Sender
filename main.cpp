#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <dev/serialdevice.h>
#include <dev/serialdevicedriver.h>
 
int main(int argc, char **argv) {
  dev::SerialDevicePtr dev = std::make_shared<dev::SerialDevice>("/dev/ttyUSB0");
  dev::drivers::rs232::SerialDeviceDriver driver(dev);
  
  for(int i = 0; i < 10; i++) {
  int incr = driver.inc(0);
    std::cout << "i = " << i <<" incr =" << incr << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }
  
  int a = 0;
  std::cin >> a;
  return 0;
}

