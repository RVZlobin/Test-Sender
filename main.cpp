#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <dev/serialdevice.h>
#include <dev/serialdevicedriver.h>

//#include <ntfs-3g/logging.h>
//#include <boost/log/core.hpp>
//#include <boost/log/trivial.hpp>

void init() {
  
}

int main(int argc, char **argv) {
  init();
  dev::SerialDevicePtr dev =std::make_shared<dev::SerialDevice>("/dev/ttyUSB0");
  int errorcode = dev->open();
  std::cout << "errorcode - " << errorcode << std::endl;
  //BOOST_LOG_TRIVIAL(debug) << "Подготовка порта завершена.";
  
  dev::drivers::rs232::SerialDeviceDriver driver(dev);
  //BOOST_LOG_TRIVIAL(debug) << "Подготовка драйвера завершена.";
  
  for(int i = 0; i < 10; i++) {
    int incr = driver.inc(static_cast<std::uint8_t>(i));
    std::cout << "i = " << i <<" incr =" << incr << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }
  
  int a = 0;
  std::cin >> a;
  return 0;
}

