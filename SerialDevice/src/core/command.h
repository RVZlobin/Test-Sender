#ifndef COMMAND_H
#define COMMAND_H

/**
 * @todo write docs
 */
#include <memory>
#include <string>
#include <future>

#include <dev/device.h>
#include <serialdevice_export.h>

namespace dev {
  
  enum class CommandKinds {
    PING  = 0,
    GET   = 2,
    SET   = 4,
    SINGL = 6,
    FLOW  = 8
  };

  enum class Status {
    SPIRIT,
    NEW,
    MIRROR,
    FLOW,
    COMPLETION,
    DONE
  };
  
  class SERIALDEVICE_API Command {
    private:
      std::size_t id = 0;
      Status status = Status::SPIRIT;
    protected:
      Command() = default;
      Command(Command const&) = delete;
      Command(Command &&) = default;
      Command& operator=(Command&&) = default;
      Command& operator=(Command const&) = delete;

      std::string name = "";
      CommandKinds kind = CommandKinds::PING;
      std::uint8_t devId = 0;
      std::size_t index = 0;
      std::size_t subIndex = 0;
      dev::TransmitData transmitData = { };
    public:
      explicit Command(std::uint8_t const& devId, std::string const& name) noexcept;
      explicit Command(std::uint8_t const& devId, std::string const& name, std::size_t const& index, std::size_t const& subIndex) noexcept;
      virtual ~Command() noexcept { };
      
      virtual auto setId(std::size_t id) -> void;
      virtual auto setStatus(Status const& status) -> void;

      auto getId() const -> std::size_t;
      auto getStatus() const -> Status;

      virtual auto getTransmitData() const -> dev::TransmitData;
      virtual auto response(dev::TransmitData&& transmitData) -> void;

      auto operator== (Command const& r) const -> bool {
        if(this->id == 0)
          return false;
        else if(this->id == r.getId())
          return true;
        return false;
      }
      virtual auto getDevId() const -> std::uint8_t {
        return devId;
      };
      virtual auto getKind() const -> CommandKinds {
        return kind;
      };
      virtual auto getIndex() const -> std::size_t {
        return index;
      };
      virtual auto getSubIndex() const->std::size_t {
        return subIndex;
      };
    private:
      virtual auto responseProcessing(dev::TransmitData&& transmitData) -> void = 0;
  };
  
  namespace com {
    
    class IncCommand final: public dev::Command {
        mutable std::promise<std::uint8_t> p;
        mutable std::shared_ptr<std::shared_future<std::uint8_t>> result;
        std::uint8_t value;
      public:
        IncCommand(std::uint8_t const& devId, std::uint8_t const& value): dev::Command(devId, "Inc"), value(value) {
          this->transmitData.push_back(value);
          result = std::make_shared<std::shared_future<std::uint8_t>>(p.get_future());
          this->kind = CommandKinds::SET;
        }
    
        IncCommand(IncCommand const&) = delete;

        std::shared_ptr<std::shared_future<std::uint8_t>> operator()() const {
          return result;
        }
      private: 
        virtual auto responseProcessing(dev::TransmitData&& transmitData) -> void override final {
          if(transmitData.size() > 0) {
            p.set_value(static_cast<std::uint8_t>(transmitData[0]));
          }
        };
    };
    typedef std::shared_ptr<IncCommand> IncCommandPtr;
  }
  typedef std::shared_ptr<Command> CommandPtr;
}
#endif // COMMAND_H
