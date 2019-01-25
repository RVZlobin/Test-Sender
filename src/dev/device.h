#ifndef DEVICE_H
#define DEVICE_H

/**
 * @todo write docs
 */

#include <string>
#include <vector>

namespace dev {
  
  typedef std::vector<unsigned char> TransmitData;
  
  class Device {
    
    public:    
      virtual int open() = 0;
      virtual int cloae() = 0;
      virtual int write(TransmitData const& data) const = 0;
      virtual TransmitData reead(std::size_t const& size = 1) const = 0;

  };
  
};
#endif // DEVICE_H
