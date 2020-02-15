#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <future>

#include <dev/device.h>
#include <core/command.h>
#include <serialdevice_export.h>
/**
 * @todo write docs
 */
namespace dev {
  namespace rs232 {
  
  class SERIALDEVICE_API Protocol {

    struct LowNibbleHighNibble {
      unsigned char HighNibble;
      unsigned char LowNibble;
    };

    union {
      std::uint16_t src;
      struct LowNibbleHighNibble des;
    } from16bit;

    union {
      std::uint16_t des;
      struct LowNibbleHighNibble src;
    } to16bit;

    struct CurrentTransaction {
      dev::TransmitData transactionData = { }; //набранный массив байт
      std::size_t dataLength = 0; //ожидаемая длинна данных (полезная нагрузка пакета)
    };

    std::atomic<bool> isRun;
    mutable std::shared_ptr<std::shared_mutex> devMutexPtr;
    std::once_flag init_flag;
    //хранить указатель или идентификатор.
    std::vector<std::pair<DevicePtr, CommandPtr>> commandsExec;
    std::vector<std::pair<DevicePtr, std::future<int>>> responseRepository;
    std::size_t sequenceId = 0;

    const std::size_t devIdLength = 1;        //длинна идентификатора устройства в байтах
    const std::size_t cmdIdLength = 2;        //длинна идентификатора команды в байтах
    const std::size_t cmdLength = 1;          //длинна типа команды в байтах
    const std::size_t transmitDataLength = 1; //длинна разрядности данных в байтах
    const std::size_t indexLength = 1;        //длинна индекса в байтах
    const std::size_t subIndexLength = 2;     //длинна под индекса в байтах
    //std::size_t dataLength = 0;               //длинна данных в байтах
    const std::size_t sumLength = 2;          //длинна контрольной суммы в байтах
    
    const std::size_t devIdPos = 0;        
    const std::size_t cmdIdPos = devIdPos + devIdLength;
    const std::size_t cmdPos = cmdIdPos + cmdIdLength;
    const std::size_t transmitDataPos = cmdPos + cmdLength;
    const std::size_t indexPos = transmitDataPos + transmitDataLength;
    const std::size_t subIndexPos = indexPos + indexLength;

    //минимальное количество байт необходимое для расчета длинны пакета.
    const std::size_t minSizeParcel = devIdLength + cmdIdLength + cmdLength + transmitDataLength;
  public:
    Protocol();
    Protocol(Protocol const&) = delete;
    ~Protocol() noexcept;
    
    void runCommand(DevicePtr const& dev, CommandPtr const& cmd);
    
  private:
    inline auto getPakageLength(std::size_t const& dataLength = 0) -> std::size_t const;
    //метод dataCheck проверяет контрольные суммы и сигнатуру ответа.
    auto dataCheck(dev::TransmitData const& data) -> const bool;
    //метод getId из принятого пакета извлекает идентификатор команды которой предназначен ответ.
    auto getCmdId(dev::TransmitData const& data) -> const std::size_t;
    virtual auto prepareCommand(CommandPtr const& cmd) -> dev::TransmitData;
    inline auto responseManager(std::string const& devName, dev::TransmitData&& data) -> void;
    void addResponse();
    void removeResponse();
  };
  
  typedef std::shared_ptr<Protocol> ProtocolPtr;
  
  }
}
#endif // PROTOCOL_H
