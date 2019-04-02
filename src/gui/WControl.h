#pragma once
#include <QWidget>

namespace Ui {
  class WControl;
}

namespace OperatorInterface {
  #define NOMINAL_RESISTANCE 10;
  #define RESISTOR_BIT_DEPTH 8;

  class WControl: public QWidget {
    Q_OBJECT
  public:
    explicit WControl(QWidget* parent = nullptr);
    ~WControl();

    Q_SIGNAL void sendConnect(QString port);
    Q_SIGNAL void sendValueR1(int value);
    Q_SIGNAL void sendValueR2(int value);

    
    Q_SLOT void attestationValueR1(int value);
    Q_SLOT void attestationValueR2(int value);

  private:
    Ui::WControl *ui;
  };
}
