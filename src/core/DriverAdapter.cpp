#include <QMessageBox>
#include "DriverAdapter.h"
dev::drivers::rs232::DriverAdapter::
DriverAdapter(dev::SerialDevicePtr const& rs232Dev, QObject* parent) {

}

dev::drivers::rs232::DriverAdapter::
~DriverAdapter() {
  dev->cloae();
}

void dev::drivers::rs232::DriverAdapter::
setConnect(QString port) {
  dev = std::make_shared<dev::SerialDevice>(port.toStdString());
  protocol = std::make_shared<dev::rs232::Protocol>();
  int errorcode = dev->open();
  if (errorcode != 0) {
    QMessageBox::critical(nullptr, "Ошибка открытия порта", QString("Код ошибки %1").arg(errorcode).toStdString().c_str());
  }
}

void dev::drivers::rs232::DriverAdapter::
setValueR1(int value) {
  setResistanceCommand(1, 1, static_cast<std::uint8_t>(value));
}

void dev::drivers::rs232::DriverAdapter::
setValueR2(int value) {
  setResistanceCommand(1, 2, static_cast<std::uint8_t>(value));
}
