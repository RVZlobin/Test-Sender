#ifndef COMMAND_GET_ADC_DATA_COLLECTED_H
#define COMMAND_GET_ADC_DATA_COLLECTED_H

/**
 * @todo write docs
 */
#include <memory>
#include <string>
#include <future>

#include <dev/device.h>
#include <core/command.h>
#include <serialdevice_export.h>

namespace dev {  
  namespace com {

    class GetADCDataCollectedCommand final : public dev::Command {
        mutable std::promise<void> p;
        mutable std::shared_ptr<std::shared_future<void>> result;
        std::uint8_t value = 0;
      public:
        GetADCDataCollectedCommand() = default;
        explicit GetADCDataCollectedCommand(std::uint8_t const& devId, std::size_t subIndex, std::uint8_t const& value)
          : dev::Command(devId, "GetADCDataCollected", 1, subIndex), value(value)
        {
          this->kind = CommandKinds::GET;
          this->transmitData.push_back(value);
          result = std::make_shared<std::shared_future<void>>(p.get_future());
        }
        ~GetADCDataCollectedCommand() = default;
        GetADCDataCollectedCommand(GetADCDataCollectedCommand const&) = delete;
        GetADCDataCollectedCommand& operator=(GetADCDataCollectedCommand const&) = delete;
        
        std::shared_ptr<std::shared_future<void>> operator()() const {
          return result;
        }
      private: 
        virtual auto responseProcessing(dev::TransmitData&& transmitData) -> void override final {
          p.set_value();
        };
    };
    typedef std::shared_ptr<GetADCDataCollectedCommand> GetADCDataCollectedCommandPtr;
  }
}
#endif // COMMAND_GET_ADC_DATA_COLLECTED_H
