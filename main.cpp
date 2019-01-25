#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include "src/core/performer.h"

std::promise<double> promise;

void thread_func1() {
  for(auto i = 0; i <10; i++) {
    std::cout << "Ожидане обещанного значения " << i << std::endl;
    std::cout << promise.get_future().share().get() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    promise = std::promise<double>();
  }
}

void thread_func2() {
  //using namespace std::literals::chrono_literals;
  auto start = std::chrono::high_resolution_clock::now();
  for(auto i = 0; i <10; i++) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Приостановка потока " << i << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout << "Waited " <<  elapsed.count() << " ms" << std::endl;
    std::cout << "Отправка значения " << i << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    for(auto b = 0; b < 10; b++) {
      try {
        promise.set_value(elapsed.count());
        break;
      } catch(...){
        std::cout << "Футуре не установлен " << b << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    }
  }
}
 
int main(int argc, char **argv) {
  std::thread th1(thread_func1);
  std::thread th2(thread_func2);
    
  th1.join();
  th2.join();
  std::cout << "Hello, world!\n" << std::endl;
  
  int a = 0;
  std::cin >> a;
  return 0;
}

