#include <QLCDNumber>
#include <QPushButton>
#include <QSlider>

#include <ui_WControl.h>
#include "WControl.h"

OperatorInterface::WControl::WControl(QWidget* parent) : QWidget(parent), ui(new Ui::WControl) {
  ui->setupUi(this);

  QObject::connect(ui->connectionButton, &QPushButton::clicked, [&]() {
    emit sendConnect(ui->portName->text());
  });

  QObject::connect(ui->r1Slider, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), [&](int const& value) {
    //расчетное сопротивление первого резистора (отображение)
    double nominal = NOMINAL_RESISTANCE;
    auto divider = 1 << RESISTOR_BIT_DEPTH;
    QString display = QString("%1").arg(QString::number((nominal / (divider - 1)) * value, 'f', 3));
    ui->r1komNumber->display(display);
    emit sendValueR1(value);
  });

  QObject::connect(ui->r2Slider, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), [&](int const& value) {
    //расчетное сопротивление второго резистора (отображение)
    double nominal = NOMINAL_RESISTANCE;
    auto divider = 1 << RESISTOR_BIT_DEPTH;
    QString display = QString("%1").arg(QString::number((nominal / (divider - 1)) * value, 'f', 3));
    ui->r2komNumber->display(display);
    emit sendValueR2(value);
  });
}

OperatorInterface::WControl::~WControl() {

}

void OperatorInterface::WControl::attestationValueR1(int value) {
  
}

void OperatorInterface::WControl::attestationValueR2(int value) {

}
