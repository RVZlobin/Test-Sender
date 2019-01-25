#ifndef COMMAND_H
#define COMMAND_H

/**
 * @todo write docs
 */
#include <memory>
#include <string>
#include <future>

#include "dev/device.h"

namespace pfr {
  
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
      ~Command ();
      
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
    
    class SetValue : public pfr::Command {
        mutable std::promise<std::uint16_t> p;
        std::shared_ptr<std::shared_future<std::uint16_t>> result;
        
      public:
        SetValue(std::uint16_t const& value): pfr::Command("SetValue") { 
          result = std::make_shared<std::shared_future<std::uint16_t>>(p.get_future());
        }
        
        operator std::shared_ptr<std::shared_future<std::uint16_t>>() const {
          return result;
        }
      private: 
        virtual auto responseProcessing(dev::TransmitData const& transmitData) -> void override final {
          
        };
    };
    typedef std::shared_ptr<SetValue> SetValuePtr;
  }
  typedef std::shared_ptr<Command> CommandPtr;
};
#endif // COMMAND_H
































