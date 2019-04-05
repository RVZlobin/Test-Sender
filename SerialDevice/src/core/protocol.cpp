#include <algorithm>
#include <iostream>
#include <iterator>
#include <core/protocol.h>
  
dev::rs232::Protocol::Protocol() {
  isRun = true;
  std::function<int()> writer = [&]() -> int {
    while (isRun) {
      auto cmd = std::find_if(commandsExec.begin(), commandsExec.end(),
        [&](auto const& item) {
          return item.second->getStatus() == dev::Status::SPIRIT;
        });
      if (cmd != commandsExec.end() && cmd->first) {
        //запись команды в порт (отправка на исполнение)
        auto errorcode = cmd->first->write(prepareCommand(cmd->second));
        if (errorcode < 0) {
          throw std::runtime_error("Ошибка записи в порт");
        }
        cmd->second->setStatus(dev::Status::NEW);
        if (cmd->second->getKind() == 6) {
          //FIX тип команд не предполагающих ответа
          cmd->second->response({ });
          commandsExec.erase(cmd);
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    return 0;
  };
  std::packaged_task<int()> writerTsk(writer);
  std::thread writerTskThread(std::move(writerTsk));
  writerTskThread.detach();
}

dev::rs232::Protocol::~Protocol() {
  isRun = false;
}

void dev::rs232::Protocol::runCommand(DevicePtr const& dev, CommandPtr const& cmd) {
  std::unique_lock<std::mutex> lock(devMutex);
  //поиск сборщика ответов от устройства.
  auto presents = std::find_if(responseRepository.begin(), responseRepository.end(), 
                               [&](auto const& item) {
                                 return item.first == dev; 
                              });
  if(presents == responseRepository.end()) {
    //если обработчика нет то регистрируем новый (на устройство)
    std::function<int(DevicePtr const&)> reader = [&](DevicePtr const& devLocal) -> int {
      int errorCode = -1;
      dev::TransmitData tempData;
      try {
        while (isRun) {
          std::cout << "Get  signalQuantum." << std::endl;
          dev::TransmitData signalQuantum(devLocal->reead());
          std::move(signalQuantum.begin(), signalQuantum.end(), std::back_inserter(tempData));
          if(tempData.size() == sizeParcel) {
            std::mutex moveDataMutex;
            std::unique_lock<std::mutex> lock(moveDataMutex);
            std::thread responseManagerThread([&]() {
              try {
                dev::TransmitData responseData = std::move(tempData);
                lock.unlock();
                responseManager(devLocal->getName(), std::move(responseData));
              } catch(...) {
                lock.unlock();
              }
            });
            responseManagerThread.detach();
            //освобождение потока с задачей обработки команд от устройства.
            std::unique_lock<std::mutex> moveDataLock(moveDataMutex);
            tempData = { };
          }
          //std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
        errorCode = 0;
      } catch(std::exception const& exc) {
        errorCode = -1;
        std::cout << "responseManagerThread  Exception - " << exc.what() << std::endl;
      } catch(...) { 
        /* FIX добавить обработчик*/ 
        errorCode = -2;
      } 
      return errorCode;
    };
    std::cout << "Регистрация обработчика событий от устройства." << std::endl;
    std::packaged_task<int(DevicePtr const&)> responseTsk (reader);
    responseRepository.push_back(std::make_pair(dev, responseTsk.get_future()));
    std::thread presentsThread(std::move(responseTsk), dev);
    //освобождение потока с задачей сбора данных от устройства.
    presentsThread.detach();
  }
  std::cout << "помещение команды в очередь." << std::endl;
  cmd->setId(sequenceId++);
  commandsExec.push_back(std::make_pair(dev, cmd));
}

auto dev::rs232::Protocol::dataCheck(dev::TransmitData const& data) -> const bool {
    return true;
}

auto dev::rs232::Protocol::getId(dev::TransmitData const& data) -> const std::size_t {
  return 1;
}

auto dev::rs232::Protocol::prepareCommand(CommandPtr const& cmd) -> dev::TransmitData {
  dev::TransmitData tx;
  auto data = cmd->getTransmitData();
  unsigned char dataSize = static_cast<unsigned char>(data.size());
  tx.push_back(static_cast<unsigned char>(cmd->getDevId()));
  tx.push_back(static_cast<unsigned char>(cmd->getKind()));
  tx.push_back(dataSize);
  tx.push_back(static_cast<unsigned char>(cmd->getIndex()));
  subIndex.src = static_cast<std::uint16_t>(cmd->getSubIndex());
  tx.push_back(static_cast<unsigned char>(subIndex.des.major));
  tx.push_back(static_cast<unsigned char>(subIndex.des.minor));
  
  std::move(data.rbegin(), data.rend(), std::back_inserter(tx));
  std::size_t sum = 0;
  std::for_each(tx.begin(), tx.end(), [&](auto& item) { sum += item; });
  for (auto i = 0; i < 2; i++) {
    tx.push_back(~((sum >> (8 * i)) & 0xFF));
  }
  std::for_each(tx.begin(), tx.end(), [&](auto& item) { 
    //std::cout.unsetf(std::ios::dec);
    //std::cout.setf(std::ios::hex | std::ios::uppercase);
    printf(":%X:", item);
  });
  std::cout << std::endl;
  return tx;
}

auto dev::rs232::Protocol::responseManager(std::string const& devName, dev::TransmitData const& data) -> void {
  if(dataCheck(data)) {
    //auto cmdId = getId(data);
    std::cout << "Поиск команды в очереди для передачи ответа data.size() = " << data.size() << std::endl;
    if(data.size() > 0) 
      std::cout << "data[0]=" << static_cast<int>(data[0]) << std::endl;
    auto cmd = std::find_if(commandsExec.begin(), commandsExec.end(), 
                               [&](auto const& item) {
                                 return item.first->getName() == devName /*&& item.second->getId() == cmdId*/ ;
                              });
    if(cmd != commandsExec.end()) {
      cmd->second->response(std::move(data));
      commandsExec.erase(cmd);
    }
  }
}

