#include "gcode-player.h"
#include "ui_gcode-player.h"
#include <QMessageBox>

#ifndef Q_OS_IOS

#include <QTimer>
#include <QSerialPortInfo>

#endif

GCodePlayer::GCodePlayer(QWidget *parent) :
     QFrame(parent),
#ifndef Q_OS_IOS
     thread_(SerialPortThread(this)),
#endif
     ui(new Ui::GCodePlayer) {
  ui->setupUi(this);
  loadSettings();
  registerEvents();

  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &GCodePlayer::loadSettings);
  timer->start(3000);
}

void GCodePlayer::loadSettings() {
#ifndef Q_OS_IOS
  const auto infos = QSerialPortInfo::availablePorts();
  ui->portComboBox->clear();
  for (const QSerialPortInfo &info : infos)
    ui->portComboBox->addItem(info.portName());
  ui->portComboBox->setCurrentIndex(ui->portComboBox->count() - 1);
#endif
}

void GCodePlayer::registerEvents() {
#ifndef Q_OS_IOS
  connect(ui->executeBtn, &QAbstractButton::clicked, [=]() {
    thread_.playGcode(ui->portComboBox->currentText(),
                      ui->baudComboBox->currentText().toInt(),
                      ui->gcodeText->toPlainText().split("\n"));
  });

  connect(&thread_, &SerialPortThread::error, this, &GCodePlayer::showError);
#endif
}

void GCodePlayer::showError(const QString &msg) {
  QMessageBox msgbox;
  msgbox.setText("Serial Port Error");
  msgbox.setInformativeText(msg);
  msgbox.exec();
}

void GCodePlayer::setGCode(const QString &gcode) {
  ui->gcodeText->setPlainText(gcode);
}

GCodePlayer::~GCodePlayer() {
  delete ui;
}
