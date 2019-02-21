#ifndef DEVICE_H
#define DEVICE_H

/**
 * @todo write docs
 */

#include <string>
#include <vector>
#include <memory>

namespace dev {
  
  typedef std::vector<unsigned char> TransmitData;
  
  class Device {
      
    public:    
      virtual int open() = 0;
      virtual int cloae() = 0;
      virtual int write(TransmitData const& data) const = 0;
      virtual TransmitData reead(std::size_t const& size = 1) const = 0;
      
      virtual auto getName () const -> std::string = 0;
      virtual auto operator== (Device const& r) const -> bool = 0;
  };
  typedef std::shared_ptr<Device> DevicePtr;
  
};
#endif // DEVICE_H
