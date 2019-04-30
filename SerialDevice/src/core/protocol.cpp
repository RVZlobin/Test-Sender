#include <algorithm>
#include <iostream>
#include <iterator>
#include <core/protocol.h>
  
dev::rs232::Protocol::Protocol() {
  isRun = true;
  devMutexPtr = std::make_shared<std::shared_mutex>();

  std::function<int()> writer = [&]() -> int {
    auto localdevMutexPtr = devMutexPtr;
    while (isRun) {
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
      std::shared_lock<std::shared_mutex> lock(*localdevMutexPtr);
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
        lock.unlock();
        cmd->second->setStatus(dev::Status::NEW);
        if (cmd->second->getKind() == 6) {
          //FIX тип команд не предполагающих ответа
          cmd->second->response({ });
          std::unique_lock<std::shared_mutex> lock(*localdevMutexPtr);
          commandsExec.erase(cmd);
        }
      }
    }
    return 0;
  };
  std::packaged_task<int()> writerTsk(writer);
  std::thread writerTskThread(std::move(writerTsk));
  writerTskThread.detach();
}

dev::rs232::Protocol::~Protocol() {
  isRun = false;
  try {
    for (auto& item : responseRepository) { 
      if (item.first) {
        item.first->close();
        item.second.wait();
      }
    }
  } catch (...) { }
}

void dev::rs232::Protocol::runCommand(DevicePtr const& dev, CommandPtr const& cmd) {
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
          //FIX добавить лимит времени на получение данных (сброс не полного вектора)
          std::cout << "Get  signalQuantum." << std::endl;
          dev::TransmitData signalQuantum(devLocal->reead());
          std::move(signalQuantum.begin(), signalQuantum.end(), std::back_inserter(tempData));
          if (tempData.size() == minSizeParcel) {
            dataLength = static_cast<std::size_t>(tempData[minSizeParcel - 1]);
          } else if (tempData.size() == getPakageLength()) {
            std::function<void(dev::TransmitData&&)> rmFun = ([&](dev::TransmitData&& data) -> void {
              try {
                responseManager(devLocal->getName(), std::move(data));
              } catch(...) {
              }
            });
            std::thread responseManagerThread(std::move(rmFun), std::move(tempData));
            //освобождение потока с задачей обработки команд от устройства.
            responseManagerThread.detach();
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
  std::unique_lock<std::shared_mutex> lock(*devMutexPtr);
  commandsExec.push_back(std::make_pair(dev, cmd));
}

inline auto dev::rs232::Protocol::getPakageLength() -> std::size_t const {
  return (devIdLength
          + cmdLength
          + transmitDataLength
          + indexLength
          + subIndexLength
          + dataLength
          + sumLength);
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

    std::shared_lock<std::shared_mutex> lock(*devMutexPtr);
    auto cmd = std::find_if(commandsExec.begin(), commandsExec.end(), 
                               [&](auto const& item) {
                                 return item.first->getName() == devName /*&& item.second->getId() == cmdId*/ ;
                              });
    if(cmd != commandsExec.end()) {
      cmd->second->response(std::move(data));
      cmd->second->setStatus(dev::Status::COMPLETION);
      lock.unlock();
      std::unique_lock<std::shared_mutex> lock(*devMutexPtr);
      commandsExec.erase(cmd);
    }
  }
}

