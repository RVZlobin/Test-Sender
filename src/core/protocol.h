#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>
#include <future>
#include <functional>

#include "dev/serialdevice.h"
#include "command.h"
/**
 * @todo write docs
 */
namespace pfr {
  
  class Protocol {
    std::atomic<bool> isRun;
    std::mutex devMutex;
    std::once_flag init_flag;
    //FIX хранить указатель или идентификатор.
    std::vector<std::pair<dev::SerialDevice const&, CommandPtr>> commandsExec;
    std::vector<std::pair<dev::SerialDevice const&, std::future<int>>> responseRepository;
    std::size_t sizeParcel = 8;
    std::size_t transmitDataLength = 1;
    std::size_t segmentDirectionLength = 1;
    std::size_t segmentIdLength = 4;
    std::size_t sequenceId;
    
  public:
    Protocol();
    ~Protocol();
    void runCommand(dev::SerialDevice const& dev, CommandPtr const& cmd);
  private:
    auto dataCheck(dev::TransmitData const& data) -> const bool;
    auto getId(dev::TransmitData const& data) -> const std::size_t;
    auto responseManager(std::string const& devName, dev::TransmitData const& data) -> void;
    void addResponse();
    void removeResponse();
  };
  
  typedef std::shared_ptr<Protocol> ProtocolPtr;
  
};
#endif // PROTOCOL_H
