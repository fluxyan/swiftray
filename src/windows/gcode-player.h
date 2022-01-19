#pragma once

#include <QDialog>
#include <QFrame>
#include <QThread>
#include <QQuickItem>

#ifndef Q_OS_IOS

#include <connection/base-job.h>
#include <widgets/base-container.h>
#include <QTime>

#endif

namespace Ui {
  class GCodePlayer;
}

class GCodePlayer : public QFrame, BaseContainer {
Q_OBJECT

public:

  explicit GCodePlayer(QWidget *parent = nullptr);

  ~GCodePlayer();

  QList<QTime> calcRequiredTime();

  void setGCode(const QString &gcodes);
  QString getGCode();

  void showError(const QString &string);

  void attachJob(BaseJob *job);

public slots:
  void onStatusChanged(BaseJob::Status);
  void onProgressChanged(QVariant);
  
private:

  void loadSettings() override;

  void registerEvents() override;

  Ui::GCodePlayer *ui;

  BaseJob::Status status_;
  BaseJob *job_;

signals:
  void exportGcode();

  void importGcode();

  void generateGcode();

  void startBtnClicked();
  void stopBtnClicked();
  void pauseBtnClicked();
  void resumeBtnClicked();
};
