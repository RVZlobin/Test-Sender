#ifndef DRIVER_ADAPTER_H
#define DRIVER_ADAPTER_H

#include <QObject>
#include <QString>
#include <dev/serialdevice.h>
#include <dev/serialdevicedriver.h>

namespace dev {
  namespace drivers {
    namespace rs232 {

      class DriverAdapter: public QObject, private SerialDeviceDriver {
        Q_OBJECT
      public:
        explicit DriverAdapter(QObject* parent = nullptr);
        explicit DriverAdapter(dev::SerialDevicePtr const& rs232Dev, QObject* parent = nullptr);
        ~DriverAdapter();

        Q_SIGNAL void attestationValueR1(int value);
        Q_SIGNAL void attestationValueR2(int value);

        Q_SLOT void setConnect(QString port);
        Q_SLOT void setValueR1(int value);
        Q_SLOT void setValueR2(int value);

      };
    }
  }
}

#endif // DRIVER_ADAPTER_H