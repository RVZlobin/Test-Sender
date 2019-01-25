#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

/**
 * @todo write docs
 */
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp> 

#include "device.h"

namespace dev {
  namespace RS232 {
    enum class FlowControl { none, software, hardware };
    enum class StopBits { one, onepointfive, two };
    enum class Parity { none, odd, even };
  };
  
  struct Options {
    unsigned int baud_rate = 115200;
    unsigned int character_size = 8;
    RS232::StopBits stop_bits = RS232::StopBits::one;
    RS232::FlowControl flow_control = RS232::FlowControl::none;
    RS232::Parity parity = RS232::Parity::none;
  };
  
  class SerialDevice: virtual dev::Device {
      std::string portName;
      Options options;
      boost::asio::io_service* io;
      std::shared_ptr<boost::asio::serial_port> portPtr;
    public:
      explicit SerialDevice (std::string const& portName, dev::Options const& options = Options());
      ~SerialDevice ();
      
      virtual auto open() -> int override;
      virtual auto cloae() -> int override;
      virtual auto write(dev::TransmitData const& data) const -> int override final;
      virtual auto reead(std::size_t const& size = 1) const -> dev::TransmitData override final;
      auto getName () const -> std::string;
      auto operator== (SerialDevice const& r) const -> bool {
        if(this->getName() == r.getName())
          return true;
        return false;
      }
  };
};
#endif // SERIALDEVICE_H
