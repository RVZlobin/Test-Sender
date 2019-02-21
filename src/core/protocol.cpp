#include <algorithm>

#include <core/protocol.h>

//std::future<int> ret = tsk.get_future();    // get future
//std::thread th (std::move(tsk),10,0);       // spawn thread to count down from 10 to 0
//int value = ret.get();                      // wait for the task to finish and get result
//th.join();
  
dev::rs232::Protocol::Protocol() {
  isRun = true;
}

dev::rs232::Protocol::~Protocol() {
  
}

void dev::rs232::Protocol::runCommand(DevicePtr const& dev, CommandPtr const& cmd) {
  std::unique_lock<std::mutex> lock(devMutex);
  //поиск сборщика ответов от устройства.
  auto presents = std::find_if(responseRepository.begin(), responseRepository.end(), 
                               [&](std::pair<DevicePtr, std::future<int>> const& item) {
                                 return item.first == dev; 
                              });
  if(presents == responseRepository.end()) {
    //если обработчика нет то регистрируем новый (на устройство)
    std::function<int(DevicePtr const&)> reader = [&](DevicePtr const& devLocal) -> int {
      int errorCode = -1;
      dev::TransmitData tempData;
      try {
        while (isRun) {
          if(tempData.size() == sizeParcel){
            std::thread responseManagerThread([&](){
              responseManager(devLocal->getName(), std::move(tempData));
            });
            responseManagerThread.detach();
            //освобождение потока с задачей обработки команд от устройства.
            tempData = { };
          }
          dev::TransmitData signalQuantum(devLocal->reead());
          std::move(signalQuantum.begin(), signalQuantum.end(), std::back_inserter(tempData));
        }
        errorCode = 0;
      } catch(...) { /* FIX добавить обработчик*/ }
      return errorCode;
    };
    std::packaged_task<int(DevicePtr const&)> responseTsk (reader);
    responseRepository.push_back(std::make_pair(dev, responseTsk.get_future()));
    std::thread presentsThread(std::move(responseTsk), dev);
    //освобождение потока с задачей сбора данных от устройства.
    presentsThread.detach();
  }
  commandsExec.push_back(std::make_pair(dev, cmd));
  cmd->setId(sequenceId++);
  //запись команды в порт (отправка на исполнение)
  auto errorcode = dev->write(cmd->getTransmitData());
  if(errorcode < 0)
    throw std::runtime_error("Ошибка записи в порт");
  cmd->setStatus(dev::Status::NEW);
}

auto dev::rs232::Protocol::dataCheck(dev::TransmitData const& data) -> const bool {
    return true;
}

auto dev::rs232::Protocol::getId(dev::TransmitData const& data) -> const std::size_t {
  return 1;
}

auto dev::rs232::Protocol::responseManager(std::string const& devName, dev::TransmitData const& data) -> void {
  if(dataCheck(data)) {
    auto cmdId = getId(data);
    //поиск команды в очереди для передачи ответа.
    auto cmd = std::find_if(commandsExec.begin(), commandsExec.end(), 
                               [&](std::pair<dev::DevicePtr, CommandPtr> const& item) {
                                 return item.first->getName() == devName && item.second->getId() == cmdId;
                              });
    if(cmd != commandsExec.end()) {
      cmd->second->response(std::move(data));
    }
  }
}
















