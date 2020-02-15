#include <algorithm>
#include <iostream>
#include <iterator>
#include <core/protocol.h>
#include <chrono>

namespace dev {
  namespace rs232 {

    template<class T, class C = void>
    class BytesToVal {
      union {
        std::uint8_t bytes[sizeof(T)];
        T value;
      } converter = { };
    protected:
      BytesToVal() = default;

    public: 
      BytesToVal(C const& begin, C const& end, std::uint8_t const& dop = 0) {
        for (auto it = begin; it != end; ++it)
          converter.bytes[std::distance(begin, it)] = static_cast<std::uint8_t>(*it);
        for (auto i = std::distance(begin, end); i < sizeof(T); i++)
          converter.bytes[i] = dop;
      }

      BytesToVal(dev::TransmitData const& data, std::uint8_t const& dop = 0) {
        for (auto i = 0; i < sizeof(T); ++i)
          i < data.size() ? converter.bytes[i] = static_cast<std::uint8_t>(data[i]) : converter.bytes[i] = dop;
      }
      BytesToVal(BytesToVal const&) = delete;
      BytesToVal(BytesToVal&&) = default;
      BytesToVal& operator=(BytesToVal const&) = delete;
      BytesToVal& operator=(BytesToVal&&) = default;
      ~BytesToVal() = default;
      T val() const { return converter.value; }
    };
  }
}

dev::rs232::Protocol::Protocol() {
  isRun = true;
  devMutexPtr = std::make_shared<std::shared_mutex>();

  std::function<int()> writer = [&, devMutexPtr = devMutexPtr]() -> int {
    while (isRun) {
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
      std::shared_lock<std::shared_mutex> lock(*devMutexPtr);
      auto cmd = std::find_if(commandsExec.begin(), commandsExec.end(),
        [](auto const& item) {
          return item.second->getStatus() == dev::Status::SPIRIT;
        });
      if (cmd != commandsExec.end() && cmd->first) {
        //запись команды в порт (отправка на исполнение)
        auto errorcode = cmd->first->write(prepareCommand(cmd->second));
        if (errorcode < 0) {
          throw std::runtime_error("Ошибка записи в порт");
        }
        cmd->second->setStatus(dev::Status::NEW);
        if (cmd->second->getKind() == CommandKinds::SINGL) {
          //FIX тип команд не предполагающих ответа
          cmd->second->setStatus(dev::Status::COMPLETION);
          cmd->second->response({ });
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

dev::rs232::Protocol::~Protocol() noexcept {
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
      CurrentTransaction transaction = { };
      try {
        while (isRun) {
          std::cout << "Get  signalQuantum." << std::endl;
          std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
          dev::TransmitData signalQuantum(devLocal->reead());
          std::chrono::system_clock::time_point newDataTime = std::chrono::system_clock::now();
          if (std::chrono::duration_cast<std::chrono::milliseconds>(newDataTime - startTime).count() > 300) {
            // лимит времени на получение данных(сброс не полного вектора)
            // очищаем буфер если данных долго нет (для удаления запчастей неполностью принятых ответов)
            transaction = { };
          }
          std::move(signalQuantum.begin(), signalQuantum.end(), std::back_inserter(transaction.transactionData));
          if (transaction.transactionData.size() == minSizeParcel) {
            transaction.dataLength = static_cast<std::size_t>(transaction.transactionData[minSizeParcel - 1]);
          } else if (transaction.transactionData.size() == getPakageLength(transaction.dataLength)) {
            std::function<void(dev::TransmitData&&)> rmFun = ([&](dev::TransmitData&& data) -> void {
              try {
                responseManager(devLocal->getName(), std::move(data));
              } catch(...) {
              }
            });
            std::thread responseManagerThread(std::move(rmFun), std::move(transaction.transactionData));
            //освобождение потока с задачей обработки команд от устройства.
            responseManagerThread.detach();
            transaction = { };
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
  cmd->setId(++sequenceId);
  std::cout << "помещение команды в очередь id-" << sequenceId << std::endl;
  std::unique_lock<std::shared_mutex> lock(*devMutexPtr);
  commandsExec.push_back(std::make_pair(dev, cmd));
}

inline auto dev::rs232::Protocol::getPakageLength(std::size_t const& dataLength) -> std::size_t const {
  return (devIdLength
          + cmdIdLength
          + cmdLength
          + transmitDataLength
          + indexLength
          + subIndexLength
          + dataLength
          + sumLength);
}

auto dev::rs232::Protocol::dataCheck(dev::TransmitData const& data) -> const bool {
  std::size_t r_sum = 0;
  std::for_each(data.begin(), data.end() - sumLength, [&](auto& item) { r_sum += item; });
  auto f_sum = BytesToVal<std::size_t, decltype(data.begin())>(data.end() - sumLength, data.end(), 255).val();
  r_sum = ~r_sum;
  return (r_sum == f_sum);
}

auto dev::rs232::Protocol::getCmdId(dev::TransmitData const& data) -> const std::size_t {
  std::uint16_t cmdId = 0;
  if (data.size() > cmdIdPos + 1) {
    to16bit.src = { data[cmdIdPos], data[cmdIdPos + 1] };
    cmdId = to16bit.des;
  }
  return static_cast<std::size_t>(cmdId);
}

auto dev::rs232::Protocol::prepareCommand(CommandPtr const& cmd) -> dev::TransmitData {
  dev::TransmitData tx;
  dev::TransmitData txSum;
  auto data = cmd->getTransmitData();
  auto dataSize = static_cast<std::uint8_t>(data.size());
  tx.push_back(static_cast<std::uint8_t>(cmd->getDevId()));
  from16bit.src = static_cast<std::uint16_t>(cmd->getId());
  tx.push_back(static_cast<std::uint8_t>(from16bit.des.HighNibble));
  tx.push_back(static_cast<std::uint8_t>(from16bit.des.LowNibble));
  tx.push_back(static_cast<std::uint8_t>(cmd->getKind()));
  tx.push_back(dataSize);
  tx.push_back(static_cast<std::uint8_t>(cmd->getIndex()));
  from16bit.src = static_cast<std::uint16_t>(cmd->getSubIndex());
  tx.push_back(static_cast<std::uint8_t>(from16bit.des.HighNibble));
  tx.push_back(static_cast<std::uint8_t>(from16bit.des.LowNibble));
  
  std::move(data.rbegin(), data.rend(), std::back_inserter(tx));
  std::size_t sum = 0;
  std::for_each(tx.begin(), tx.end(), [&](auto& item) { sum += item; });
  for (auto i = 0; i < 2; i++) {
    txSum.push_back(~((sum >> (8 * i)) & 0xFF));
  }
  std::move(txSum.rbegin(), txSum.rend(), std::back_inserter(tx));
  std::for_each(tx.begin(), tx.end(), [&](auto& item) { 
    printf(":%X:", item);
  });
  std::cout << std::endl;
  return tx;
}

auto dev::rs232::Protocol::responseManager(std::string const& devName, dev::TransmitData&& data) -> void {
  if(dataCheck(data)) {
    auto cmdId = getCmdId(data);
    std::cout << "Поиск команды в очереди для передачи ответа data.size() = " << data.size() << std::endl;
    if(data.size() > 0) 
      std::cout << "data[0]=" << static_cast<int>(data[0]) << std::endl;

    std::shared_lock<std::shared_mutex> lock(*devMutexPtr);
    auto cmd = std::find_if(commandsExec.begin(), commandsExec.end(), 
                               [&](auto const& item) {
                                 return item.first->getName() == devName && item.second->getId() == cmdId;
                              });
    if(cmd != commandsExec.end()) {
      if (cmd->second->getStatus() == dev::Status::SPIRIT)
        cmd->second->setStatus(dev::Status::NEW);
      else if (cmd->second->getStatus() == dev::Status::NEW) {
        cmd->second->setStatus(dev::Status::COMPLETION);
        cmd->second->response(std::move(data));
        commandsExec.erase(cmd);
      }
    }
  } else {
    std::cout << "Ошибка проверки данных = " << data.size() << std::endl;
  }
}

