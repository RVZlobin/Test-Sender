#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

/**
 * @todo write docs
 */
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp> 

#include <dev/device.h>

namespace dev {
  namespace rs232 {
    enum class FlowControl { none, software, hardware };
    enum class StopBits { one, onepointfive, two };
    enum class Parity { none, odd, even };
  }
  
  struct Options {
    unsigned int baud_rate = 115200;
    unsigned int character_size = 8;
    rs232::StopBits stop_bits = rs232::StopBits::one;
    rs232::FlowControl flow_control = rs232::FlowControl::none;
    rs232::Parity parity = rs232::Parity::none;
  };
  
  class SerialDevice: public virtual dev::Device {
      std::string portName;
      Options options;
      boost::asio::io_service* io;
      std::shared_ptr<boost::asio::serial_port> portPtr;
    public:
      explicit SerialDevice (std::string const& portName, dev::Options const& options = Options());
      SerialDevice(SerialDevice const&) = delete;
      virtual ~SerialDevice ();
      
      virtual auto open() -> int override;
      virtual auto cloae() -> int override;
      virtual auto write(dev::TransmitData const& data) const -> int override final;
      virtual auto reead(std::size_t const& size = 1) const -> dev::TransmitData override final;
      auto getName () const -> std::string override final;
      auto operator== (Device const& r) const -> bool override final {
        if(this->getName() == r.getName())
          return true;
        return false;
      }
  };
  typedef std::shared_ptr<SerialDevice> SerialDevicePtr;
}
#endif // SERIALDEVICE_H
