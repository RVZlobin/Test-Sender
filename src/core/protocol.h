#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <future>

#include <dev/device.h>
#include <core/command.h>
/**
 * @todo write docs
 */
namespace dev {
  namespace rs232 {
  
  class Protocol {
    std::atomic<bool> isRun;
    std::mutex devMutex;
    std::once_flag init_flag;
    //FIX хранить указатель или идентификатор.
    std::vector<std::pair<DevicePtr, CommandPtr>> commandsExec;
    std::vector<std::pair<DevicePtr, std::future<int>>> responseRepository;
    std::size_t sizeParcel = 1;
    std::size_t transmitDataLength = 1;
    std::size_t segmentDirectionLength = 1;
    std::size_t segmentIdLength = 4;
    std::size_t sequenceId;
    
  public:
    Protocol();
    Protocol(Protocol const&) = delete;
    ~Protocol();
    
    void runCommand(DevicePtr const& dev, CommandPtr const& cmd);
    
  private:
    //метод dataCheck проверяет контрольные суммы и сигнатуру ответа.
    auto dataCheck(dev::TransmitData const& data) -> const bool;
    //метод getId из принятого пакета извлекает идентификатор команды которой предназначен ответ.
    auto getId(dev::TransmitData const& data) -> const std::size_t;
    auto responseManager(std::string const& devName, dev::TransmitData const& data) -> void;
    void addResponse();
    void removeResponse();
  };
  
  typedef std::shared_ptr<Protocol> ProtocolPtr;
  
  }
}
#endif // PROTOCOL_H
