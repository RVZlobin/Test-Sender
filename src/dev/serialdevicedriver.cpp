#include <dev/serialdevicedriver.h>

dev::drivers::rs232::SerialDeviceDriver::
SerialDeviceDriver(dev::SerialDevicePtr const& rs232Dev)
  :dev(rs232Dev)
{
  protocol = std::make_shared<dev::rs232::Protocol>();
}

dev::drivers::rs232::SerialDeviceDriver::
~SerialDeviceDriver() {

}

auto dev::drivers::rs232::SerialDeviceDriver::
setValue(std::uint8_t const & devId, std::uint16_t const & value) -> void {
  dev::com::SetValueCommandPtr setValueCommand = std::make_shared<dev::com::SetValueCommand>(devId, value);
  protocol->runCommand(dev, setValueCommand);
  (*setValueCommand)()->get();
}

auto dev::drivers::rs232::SerialDeviceDriver::
inc(std::uint8_t const& devId, std::uint8_t const& value) -> std::uint8_t {
  dev::com::IncCommandPtr incCommand = std::make_shared<dev::com::IncCommand>(devId, value);
  protocol->runCommand(dev, incCommand);
  return (*incCommand)()->get();
}
