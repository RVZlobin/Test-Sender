#include "WPB30319.h"
 
OperatorInterface::WPB30319::WPB30319(QWidget* parent) : QMainWindow(parent) {
  driverAdapter = new dev::drivers::rs232::DriverAdapter();
  wControl = new WControl();
  QDockWidget *dockWidget = new QDockWidget(tr("Управление"), this);
  dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dockWidget->setWidget(wControl);
  addDockWidget(Qt::RightDockWidgetArea, dockWidget);
  
  QWidget *centralWidget = new QWidget(this);
  QVBoxLayout *layout = new QVBoxLayout();
  
  centralWidget->setLayout(layout);
  //layout->addWidget(w1);
  //layout->addWidget(w2);
  setCentralWidget(centralWidget);
  layout->setStretch(1, 9);

  QObject::connect(wControl, SIGNAL(sendConnect(QString)), driverAdapter, SLOT(setConnect(QString)));
  QObject::connect(wControl, SIGNAL(sendValueR1(int)), driverAdapter, SLOT(setValueR1(int)));
  QObject::connect(wControl, SIGNAL(sendValueR2(int)), driverAdapter, SLOT(setValueR2(int)));
}

OperatorInterface::WPB30319::~WPB30319() {

}

