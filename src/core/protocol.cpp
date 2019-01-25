#include "protocol.h"

//std::future<int> ret = tsk.get_future();    // get future
//std::thread th (std::move(tsk),10,0);       // spawn thread to count down from 10 to 0
//int value = ret.get();                      // wait for the task to finish and get result
//th.join();
  
pfr::Protocol::Protocol() {
  isRun = true;
}

pfr::Protocol::~Protocol() {
  
}

void pfr::Protocol::runCommand(dev::SerialDevice const& dev, CommandPtr const& cmd) {
  std::unique_lock<std::mutex> lock(devMutex);
  //поиск сборщика ответов от устройства.
  auto presents = std::find_if(responseRepository.begin(), responseRepository.end(), 
                               [&](std::pair<dev::SerialDevice const&, std::future<int>> const& item) {
                                 return item.first == dev; 
                              });
  if(presents != responseRepository.end()) {
    std::function<int(dev::SerialDevice const&)> reader = [&](dev::SerialDevice const& devLocal) -> int {
      int errorCode = -1;
      std::size_t bytesReceived = 0;
      dev::TransmitData tempData;
      while (isRun) {
        if(bytesReceived == sizeParcel){
          std::thread responseManagerThread([&](){
            responseManager(devLocal.getName(), std::move(tempData));
          });
          responseManagerThread.detach();
          //освобождение потока с задачей обработки команд от устройства.
          bytesReceived = 0;
          tempData = { };
        }
        dev::TransmitData signalQuantum(devLocal.reead());
        std::move(signalQuantum.begin(), signalQuantum.end(), std::back_inserter(tempData));
        //tempData.insert(tempData.end(), std::make_move_iterator(signalQuantum.begin()), std::make_move_iterator(signalQuantum.end()));
        bytesReceived++;
      }
      return errorCode;
    };
    std::packaged_task<int(dev::SerialDevice const&)> responseTsk (reader);
    responseRepository.push_back({ dev, responseTsk.get_future() });
    std::thread presentsThread(std::move(responseTsk), dev);
    //освобождение потока с задачей сбора данных от устройства.
    presentsThread.detach();
  }
  commandsExec.push_back({ dev, cmd });
  cmd->setId(sequenceId++);
  auto errorcode = dev.write(cmd->getTransmitData());
  if(errorcode < 0)
    throw std::runtime_error("Ошибка записи в порт");
  cmd->setStatus(pfr::Status::NEW);
}

auto pfr::Protocol::dataCheck(dev::TransmitData const& data) -> const bool {
    return true;
}

auto pfr::Protocol::getId(dev::TransmitData const& data) -> const std::size_t {
  return 1;
}

auto pfr::Protocol::responseManager(std::string const& devName, dev::TransmitData const& data) -> void {
  if(dataCheck(data)) {
    auto cmdId = getId(data);
    //поиск команды в очереди для передачи ответа.
    auto cmd = std::find_if(commandsExec.begin(), commandsExec.end(), 
                               [&](std::pair<dev::SerialDevice const&, CommandPtr> const& item) {
                                 return item.first.getName() == devName && item.second->getId() == cmdId;
                              });
    if(cmd != commandsExec.end()) {
      cmd->second->response(std::move(data));
    }
  }
}
















