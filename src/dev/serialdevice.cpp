#include "serialdevice.h"

dev::SerialDevice::SerialDevice(std::string const& portName, Options const& options) 
  : portName(portName),
    options(options),
    io(new boost::asio::io_service()),
    portPtr(std::make_shared<boost::asio::serial_port>(*io, portName))
{ }

dev::SerialDevice::~SerialDevice() { }
      
auto dev::SerialDevice::open() -> int {
  int errorCode = -1;
  try {
    if(!portPtr)
      return errorCode;
    
    portPtr->open(portName);
 
    if(portPtr->is_open()) {
        errorCode = 0;

        portPtr->set_option( boost::asio::serial_port_base::baud_rate( options.baud_rate ) );
        portPtr->set_option( boost::asio::serial_port_base::character_size( options.character_size ));
        auto flow_control = boost::asio::serial_port_base::flow_control::none;
        switch(options.flow_control) {
          case RS232::FlowControl::software:
            flow_control = boost::asio::serial_port_base::flow_control::software;
          case RS232::FlowControl::hardware:
            flow_control = boost::asio::serial_port_base::flow_control::hardware;
          default:
            break;
        }
        portPtr->set_option( boost::asio::serial_port_base::flow_control(flow_control));
        auto parity = boost::asio::serial_port_base::parity::none;
        switch(options.parity) {
          case RS232::Parity::even:
            parity = boost::asio::serial_port_base::parity::even;
          case RS232::Parity::odd:
            parity = boost::asio::serial_port_base::parity::odd;
          default:
            break;
        }
        portPtr->set_option( boost::asio::serial_port_base::parity(parity));
        auto stop_bits = boost::asio::serial_port_base::stop_bits::one;
        switch(options.stop_bits) {
          case RS232::StopBits::onepointfive:
            stop_bits = boost::asio::serial_port_base::stop_bits::onepointfive;
          case RS232::StopBits::two:
            stop_bits = boost::asio::serial_port_base::stop_bits::two;
          default:
            break;
        }
        portPtr->set_option( boost::asio::serial_port_base::stop_bits(stop_bits));
    }
    return errorCode;
  } catch(...) {
    return -1;
  }
  return errorCode;
}

auto dev::SerialDevice::cloae() -> int {
  int errorCode = -1;
  if(!portPtr)
      return errorCode;
  
  if(portPtr->is_open()) {
    portPtr->close();
    errorCode = 0;
  }
  return errorCode;
}

auto dev::SerialDevice::write(dev::TransmitData const& data) const -> int {
  int errorCode = -1;
  if(!portPtr)
    return errorCode;
  
  try {
    if(portPtr->is_open()) {
      boost::asio::write(*portPtr, boost::asio::buffer(data.data(), data.size()));
      errorCode = 0;
    }
  } catch(...) {
    return -1;
  }
  return errorCode;
}

auto dev::SerialDevice::reead(std::size_t const& size) const -> dev::TransmitData {
  TransmitData data(size, 0);
  if(!portPtr)
    return { };
  
  boost::asio::read(*portPtr, boost::asio::buffer(data.data(), data.size()));
  return data;
}

auto dev::SerialDevice:: getName() const -> std::string {
  return this->portName;
}
