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

namespace dev {
  namespace drivers {
    namespace rs232 {
      
      class SerialDeviceDriver {
        
      protected:
        dev::SerialDevicePtr dev;
        dev::rs232::ProtocolPtr protocol;
        
      public:
          /**
          * Default constructor
          */
        SerialDeviceDriver (dev::SerialDevicePtr const& rs232Dev);

          /**
          * Destructor
          */
        ~SerialDeviceDriver ();
        
        auto inc(std::uint8_t const& value) -> std::uint8_t;

      };
      
    }
  }
}

#endif // SERIALDEVICEDRIVER_H
