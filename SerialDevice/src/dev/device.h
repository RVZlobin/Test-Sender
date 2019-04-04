#ifndef DEVICE_H
#define DEVICE_H

/**
 * @todo write docs
 */

#include <string>
#include <vector>
#include <memory>

#include <serialdevice_export.h>

namespace dev {
  
  typedef std::vector<unsigned char> TransmitData;
  
  class SERIALDEVICE_API Device {
      
    public:
      virtual ~Device() { }
      virtual int open() = 0;
      virtual int close() = 0;
      virtual int write(TransmitData const& data) const = 0;
      virtual TransmitData reead(std::size_t const& size = 1) const = 0;
      
      virtual auto getName () const -> std::string = 0;
      virtual auto operator== (Device const& r) const -> bool = 0;
  };
  typedef std::shared_ptr<Device> DevicePtr;
  
}
#endif // DEVICE_H
