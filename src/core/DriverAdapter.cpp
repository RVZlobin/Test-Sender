#include <QMessageBox>
#include "DriverAdapter.h"

dev::drivers::rs232::DriverAdapter::
DriverAdapter(QObject* parent) : QObject(parent) {

}

dev::drivers::rs232::DriverAdapter::
DriverAdapter(dev::SerialDevicePtr const& rs232Dev, QObject* parent) : QObject(parent) {

}

dev::drivers::rs232::DriverAdapter::
~DriverAdapter() {
  try {
    dev->close();
    dev = nullptr;
    protocol = nullptr;
  } catch (...) { }
}

void dev::drivers::rs232::DriverAdapter::
setConnect(QString port) {
  if (dev)
    dev = nullptr;
  if(protocol)
    protocol = nullptr;
  dev = std::make_shared<dev::SerialDevice>(port.toStdString());
  protocol = std::make_shared<dev::rs232::Protocol>();
  int errorcode = dev->open();
  if (errorcode != 0) {
    QMessageBox::critical(nullptr, "Ошибка открытия порта", QString("Код ошибки %1").arg(errorcode).toStdString().c_str());
  }
}

void dev::drivers::rs232::DriverAdapter::
setValueR1(int value) {
  if(dev && protocol)
    setResistanceCommand(1, 0, static_cast<std::uint8_t>(value));
}

void dev::drivers::rs232::DriverAdapter::
setValueR2(int value) {
  if (dev && protocol)
    setResistanceCommand(1, 1, static_cast<std::uint8_t>(value));
}
