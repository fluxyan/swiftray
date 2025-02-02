#include "gcode-player.h"
#include "ui_gcode-player.h"
#include <QMessageBox>
#include <QRegularExpression>

#ifndef Q_OS_IOS

#include <QTimer>
#include <connection/serial-port.h>
#include <globals.h>

#include <QDebug>

#endif

GCodePlayer::GCodePlayer(QWidget *parent) :
     QFrame(parent),
     ui(new Ui::GCodePlayer),
     BaseContainer() {
  ui->setupUi(this);
  initializeContainer();
}

void GCodePlayer::loadSettings() {}

void GCodePlayer::registerEvents() {
#ifndef Q_OS_IOS
  connect(ui->playBtn, &QAbstractButton::clicked, this, &GCodePlayer::startBtnClicked);
  connect(ui->stopBtn, &QAbstractButton::clicked, this, &GCodePlayer::stopBtnClicked);
  connect(ui->pauseBtn, &QAbstractButton::clicked, [=]() {
    // Pause / Resume
    if (status_ == BaseJob::Status::RUNNING) {
      emit pauseBtnClicked();
    } else {
      emit resumeBtnClicked();
    }
  });
  connect(ui->exportBtn, &QAbstractButton::clicked, this, &GCodePlayer::exportGcode);
  connect(ui->importBtn, &QAbstractButton::clicked, this, &GCodePlayer::importGcode);
  connect(ui->generateBtn, &QAbstractButton::clicked, this, &GCodePlayer::generateGcode);
  connect(ui->gcodeText, &QPlainTextEdit::textChanged, this, &GCodePlayer::checkGenerateGcode);

  connect(&serial_port, &SerialPort::connected, [=]() {
      qInfo() << "[SerialPort] Success connect!";
      ui->playBtn->setText(tr("Play"));
      if(!ui->gcodeText->toPlainText().isEmpty()) {
        ui->playBtn->setEnabled(true);
      }
  });
  connect(&serial_port, &SerialPort::disconnected, [=]() {
      qInfo() << "[SerialPort] Disconnected!";
      ui->playBtn->setText(tr("Play"));
      ui->playBtn->setEnabled(false);
  });
#endif
}

void GCodePlayer::hideEvent(QHideEvent *event) {
  emit panelShow(false);
}

void GCodePlayer::showEvent(QShowEvent *event) {
  emit panelShow(true);
}

void GCodePlayer::checkGenerateGcode() {
  if(ui->gcodeText->toPlainText().isEmpty()) {
    ui->playBtn->setEnabled(false);
  }
  else if(serial_port.isOpen()) {
    ui->playBtn->setEnabled(true);
  }
}

void GCodePlayer::showError(const QString &msg) {
  QMessageBox msgbox;
  msgbox.setText("Job Error");
  msgbox.setInformativeText(msg);
  msgbox.exec();
}

void GCodePlayer::onStatusChanged(BaseJob::Status new_status) {
  status_ = new_status;
  switch (status_) {
    case BaseJob::Status::READY:
      ui->pauseBtn->setEnabled(false);
      ui->playBtn->setEnabled(true);
      ui->stopBtn->setEnabled(false);
      break;
    case BaseJob::Status::STARTING:
      ui->pauseBtn->setEnabled(false);
      ui->playBtn->setEnabled(false);
      ui->stopBtn->setEnabled(false);
      break;
    case BaseJob::Status::RUNNING:
      qInfo() << "Running";
      ui->pauseBtn->setEnabled(true);
      ui->pauseBtn->setText(tr("Pause"));
      ui->playBtn->setEnabled(false);
      ui->stopBtn->setEnabled(true);
      //ui->playBtn->setText(tr("Stop"));
      break;
    case BaseJob::Status::PAUSED:
      qInfo() << "Paused";
      ui->pauseBtn->setEnabled(true);
      ui->pauseBtn->setText(tr("Resume"));
      break;
    case BaseJob::Status::FINISHED:
      qInfo() << "Finished";
      ui->pauseBtn->setEnabled(false);
      ui->pauseBtn->setText(tr("Pause"));
      ui->playBtn->setEnabled(true);
      ui->stopBtn->setEnabled(false);
      onProgressChanged(100);
      break;
    case BaseJob::Status::ALARM:
      ui->pauseBtn->setEnabled(false);
      ui->playBtn->setEnabled(false);
      ui->stopBtn->setEnabled(false);
      onProgressChanged(0);
      break;
    case BaseJob::Status::STOPPED:
    case BaseJob::Status::ALARM_STOPPED:
      qInfo() << "Stopped";
      ui->pauseBtn->setEnabled(false);
      ui->pauseBtn->setText(tr("Pause"));
      ui->playBtn->setEnabled(true);
      ui->stopBtn->setEnabled(false);
      onProgressChanged(0);
      break;
    default:
      break;
  }
  emit jobStatusReport(new_status);
}

void GCodePlayer::onProgressChanged(QVariant progress) {
  ui->progressBar->setValue(progress.toInt());
  ui->progressBarLabel->setText(ui->progressBar->text());
}

void GCodePlayer::setGCode(const QString &gcode) {
  ui->gcodeText->setPlainText(gcode);
}

QString GCodePlayer::getGCode() {
  return ui->gcodeText->toPlainText();
}

GCodePlayer::~GCodePlayer() {
  delete ui;
}

void GCodePlayer::attachJob(BaseJob *job) {
  job_ = job;
  qRegisterMetaType<BaseJob::Status>(); // NOTE: This is necessary for passing custom type argument for signal/slot
  connect(job_, &BaseJob::error, this, &GCodePlayer::showError);
  connect(job_, &BaseJob::progressChanged, this, &GCodePlayer::onProgressChanged);
  connect(job_, &BaseJob::statusChanged, this, &GCodePlayer::onStatusChanged);

  onStatusChanged(job_->status());
  onProgressChanged(0);
}
