#ifndef COMMAND_H
#define COMMAND_H

/**
 * @todo write docs
 */
#include <memory>
#include <string>
#include <future>

#include <dev/device.h>

namespace dev {
  
  enum class Status {
    SPIRIT,
    NEW,
    MIRROR,
    FLOW,
    COMPLETION,
    DONE
  };
  
  class Command {
    private:
      std::size_t id = 0;
      Status status = Status::SPIRIT;
    protected:
      std::string name = "";
      std::uint8_t kind = 0;
      std::uint8_t devId = 0;
      std::size_t index = 0;
      std::size_t subIndex = 0;
      dev::TransmitData transmitData;
    public:
      explicit Command(std::uint8_t const& devId, std::string const& name);
      explicit Command(std::uint8_t const& devId, std::string const& name, std::size_t const& index, std::size_t const& subIndex);
      Command() = delete;
      virtual ~Command ();
      
      virtual auto setId(std::size_t id) -> void final;
      virtual auto getId() const -> std::size_t final;
      virtual auto setStatus(Status const& status) -> void final;
      virtual auto getStatus() const -> Status final;
      virtual auto getTransmitData() const -> dev::TransmitData final;
      virtual auto response(dev::TransmitData const& transmitData) -> void final;
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
      virtual auto getKind() const -> std::uint8_t {
        return kind;
      };
      virtual auto getIndex() const -> std::size_t {
        return index;
      };
      virtual auto getSubIndex() const->std::size_t {
        return subIndex;
      };
    private:
      virtual auto responseProcessing(dev::TransmitData const& transmitData) -> void = 0;
  };
  
  namespace com {
    
    class SetValueCommand : public dev::Command {
        mutable std::promise<void> p;
        mutable std::shared_ptr<std::shared_future<void>> result;
        
      public:
        SetValueCommand(std::uint8_t const& devId, std::uint16_t value): dev::Command(devId, "SetValue", 1, 8772) {
          this->kind = 3;
          struct Des {
            unsigned char minor;
            unsigned char major;
          };

          union {
            std::uint16_t src;
            struct Des des;
          } val;

          val.src = value;
          /*
          std::size_t length = sizeof(decltype(value));
          for (auto i = 0; i < length; i++) {
            this->transmitData.push_back((reinterpret_cast<unsigned char*>(value))[i]);
          }
          */
          this->transmitData.push_back(val.des.minor);
          this->transmitData.push_back(val.des.major);
          result = std::make_shared<std::shared_future<void>>(p.get_future());
        }
        
        std::shared_ptr<std::shared_future<void>> operator()() const {
          return result;
        }
      private: 
        virtual auto responseProcessing(dev::TransmitData const& transmitData) -> void override final {
          if (transmitData.size() > 0) {
            p.set_value();
          }
        };
    };
    typedef std::shared_ptr<SetValueCommand> SetValueCommandPtr;
    
    class IncCommand : public dev::Command {
        mutable std::promise<std::uint8_t> p;
        mutable std::shared_ptr<std::shared_future<std::uint8_t>> result;
        std::uint8_t value;
      public:
        IncCommand(std::uint8_t const& devId, std::uint8_t const& value): dev::Command(devId, "Inc"), value(value) {
          this->transmitData.push_back(value);
          result = std::make_shared<std::shared_future<std::uint8_t>>(p.get_future());
        }
        IncCommand(IncCommand const&) = delete;
        virtual ~IncCommand() { }
        std::shared_ptr<std::shared_future<std::uint8_t>> operator()() const {
          return result;
        }
      private: 
        virtual auto responseProcessing(dev::TransmitData const& transmitData) -> void override final {
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
