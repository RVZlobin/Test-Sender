#include <iostream>
#include <string>
#include <future>
#include <thread>
#include <chrono>
#include <dev/serialdevice.h>
#include <dev/serialdevicedriver.h>

int main(int argc, char **argv) {
  std::string portName("/dev/ttyUSB0");
  setlocale(LC_ALL, "Russian");
  if (argc > 1) {
    portName = std::string(argv[1]);
  }
  std::cout << "use port - " << portName << std::endl;
  dev::SerialDevicePtr dev =std::make_shared<dev::SerialDevice>(portName);
  int errorcode = dev->open();
  std::cout << "errorcode - " << errorcode << std::endl;
  
  dev::drivers::rs232::SerialDeviceDriver driver(dev);
  //driver.setValue(1, 5455);
  //driver.setValue(1, 5471);
  driver.setValue(1, 8772);
  /*
  for(int i = 0; i < 255; i++) {
    auto beginTime = std::chrono::high_resolution_clock::now();
    int incr = driver.inc(1, static_cast<std::uint8_t>(i));
    if(i + 1 != incr) {
      std::cout << "Ошибка расчетов: i = " << i <<" incr =" << incr << std::endl;
      return 0;
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    auto timed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count();
    std::cout << "Выполнено за " << timed << "ms " << "i = " << i <<" incr =" << incr << std::endl;
    //std::this_thread::sleep_for(std::chrono::milliseconds(2));
    if(incr == 255)
      i = 0;
  }
  */
  int a = 0;
  std::cin >> a;
  return 0;
}

