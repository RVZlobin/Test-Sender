#ifndef SERIALDEVICEDRIVER_H
#define SERIALDEVICEDRIVER_H

/**
 * @todo write docs
 */
#include <memory>

#include <core/protocol.h>
#include <dev/device.h>
#include <core/command.h>
#include <dev/serialdevice.h>
#include <serialdevice_export.h>

namespace dev {
  namespace drivers {
    namespace rs232 {
      
      class SERIALDEVICE_API SerialDeviceDriver {
        
      protected:
        SerialDeviceDriver() { }
        dev::SerialDevicePtr dev = nullptr;
        dev::rs232::ProtocolPtr protocol = nullptr;

      public:
        SerialDeviceDriver (dev::SerialDevicePtr const& rs232Dev);
        virtual ~SerialDeviceDriver ();

        auto setValue(std::uint8_t const& devId, std::uint16_t const& value) -> void;
        auto setResistanceCommand(std::uint8_t const& devId, std::size_t subIndex, std::uint8_t const& value) -> void;
        auto inc(std::uint8_t const& devId, std::uint8_t const& value) -> std::uint8_t;

      };
      
    }
  }
}

#endif // SERIALDEVICEDRIVER_H
