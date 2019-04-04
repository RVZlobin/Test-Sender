#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
#include <chrono>
#include <utility>

#include <QtCore/QDebug>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QTimer>
#include <core/DriverAdapter.h>
#include "WControl.h"

namespace OperatorInterface {
  
  class WPB30319 : public QMainWindow  {
    Q_OBJECT
  public:
    explicit WPB30319(QWidget* parent = nullptr);
    ~WPB30319();

    WControl* wControl;
    dev::drivers::rs232::DriverAdapter* driverAdapter;
  };
}
