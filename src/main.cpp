#include <iostream>
#include <string>
#include <future>
#include <thread>
#include <chrono>

#include <QtCore/QDebug>
#include <QtWidgets/QMessageBox>

#include <QApplication>
#include <QCoreApplication>
#include <QStringList>
#include <gui/WPB30319.h>

auto main(int argc, char* argv[]) -> int {
  QStringList paths = QCoreApplication::libraryPaths();
  paths.append(".");
  paths.append("iconengines");
  paths.append("imageformats");
  paths.append("platforms");
  //paths.append("platformthemes");
  //paths.append("sqldrivers");
  //paths.append("styles");
  QCoreApplication::setLibraryPaths(paths);

  QApplication app(argc, argv);
  OperatorInterface::WPB30319* mw = new OperatorInterface::WPB30319();
  mw->show();
  app.exec();

  return 0;
}