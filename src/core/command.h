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
      dev::TransmitData transmitData;
    public:
      explicit Command(std::string const& name);
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
    private:
      virtual auto responseProcessing(dev::TransmitData const& transmitData) -> void = 0;
  };
  
  namespace com {
    
    class SetValueCommand : public dev::Command {
        mutable std::promise<std::uint16_t> p;
        std::shared_ptr<std::shared_future<std::uint16_t>> result;
        
      public:
        SetValueCommand(std::uint16_t const& value): dev::Command("SetValue") { 
          result = std::make_shared<std::shared_future<std::uint16_t>>(p.get_future());
        }
        
        operator std::shared_ptr<std::shared_future<std::uint16_t>>() const {
          return result;
        }
      private: 
        virtual auto responseProcessing(dev::TransmitData const& transmitData) -> void override final {
          
        };
    };
    typedef std::shared_ptr<SetValueCommand> SetValueCommandPtr;
    
    class IncCommand : public dev::Command {
        mutable std::promise<std::uint8_t> p;
        mutable std::shared_ptr<std::shared_future<std::uint8_t>> result;
        std::uint8_t value;
      public:
        IncCommand(std::uint8_t const& value): dev::Command("Inc"), value(value) {
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
































