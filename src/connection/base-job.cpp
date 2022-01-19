#include <connection/base-job.h>

BaseJob::BaseJob(QObject *parent, QString endpoint, QVariant data) :
     QThread(parent) {}

BaseJob::~BaseJob() {}

BaseJob::Status BaseJob::status() {
  return status_;
}

void BaseJob::setStatus(BaseJob::Status status) {
  status_ = status;
  emit statusChanged(status_);
}

int BaseJob::progress() { return 1; };

void BaseJob::start() {};

void BaseJob::stop() {};

void BaseJob::pause() {};

void BaseJob::resume() {};

QString BaseJob::statusToString(BaseJob::Status status) {
  switch (status) {
    case BaseJob::Status::READY:
      return tr("READY");
    case BaseJob::Status::STARTING:
      return tr("STARTING");
    case BaseJob::Status::RUNNING:
      return tr("RUNNING");
    case BaseJob::Status::PAUSED:
      return tr("PAUSED");
    case BaseJob::Status::PAUSING:
      return tr("PAUSING");
    case BaseJob::Status::RESUMING:
      return tr("RESUMING");
    case BaseJob::Status::STOPPING:
      return tr("STOPPING");
    case BaseJob::Status::STOPPED:
      return tr("STOPPED");
    case BaseJob::Status::FINISHED:
      return tr("FINISHED");
    case BaseJob::Status::ERROR_STOPPING:
      return tr("ERROR STOPPING");
    case BaseJob::Status::ERROR_STOPPED:
      return tr("ERROR STOPPED");
    case BaseJob::Status::ERROR_PAUSED:
      return tr("ERROR PAUSED");
    default:
      return tr("UNDEFINED STATUS");
  }
}

void BaseJob::setTimestampList(QList<QTime> &&timestamp_list) {
  timestamp_list_ = timestamp_list;
}

void BaseJob::setTimestampList(const QList<QTime> &timestamp_list) {
  timestamp_list_ = timestamp_list;
}

QTime BaseJob::getTimestamp(int idx) {
  if (timestamp_list_.empty()) {
    return QTime{0, 0};
  } else if (idx >= timestamp_list_.count()) {
    return timestamp_list_.last();
  }
  return timestamp_list_.at(idx);
}

QTime BaseJob::getTotalRequiredTime() {
  if (timestamp_list_.empty()) {
    return QTime{0, 0};
  }
  return timestamp_list_.last();
}

QTime BaseJob::getElapsedTime() {
  if (timestamp_list_.count() <= getFinishedCmdIdx()) {
    return QTime{0, 0};
  }
  return timestamp_list_.at(getFinishedCmdIdx());
}
