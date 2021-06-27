#ifndef PARAM_SETTINGS_H
#define PARAM_SETTINGS_H

#include <QString>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QJsonDocument>


class ParamSettings {
public:
  struct ParamSet {
  public:
    QString name;
    int power;
    int speed;
    int repeat;
    double step_height;
    double target_height;
    bool use_diode;
    bool use_autofocus;

    ParamSet() :
         power(20),
         speed(20),
         repeat(1),
         step_height(0),
         target_height(0),
         use_diode(false),
         use_autofocus(false) {}

    static ParamSet fromJson(const QJsonObject &obj);

    QJsonObject toJson() const;
  };

  ParamSettings(QString machine_id = "default", bool force_default = false) : machine_id_(machine_id) {
    QSettings settings;
    QJsonObject obj = settings.value("parameters/user/" + machine_id).value<QJsonDocument>().object();
    if (obj["data"].isNull() || force_default) {
      QFile file(":/resources/parameters/default.json");
      file.open(QFile::ReadOnly);
      loadJson(QJsonDocument::fromJson(file.readAll()).object());
    } else {
      loadJson(obj);
    }
  }

  void loadJson(const QJsonObject &obj) {
    QJsonArray data = obj["data"].toArray();
    params_.clear();
    for (QJsonValue item : data) {
      ParamSet param = ParamSet::fromJson(item.toObject());
      params_ << param;
    }
  }

  QJsonObject toJson() {
    QJsonArray data;
    for (auto &param : params_) {
      data << param.toJson();
    }
    QJsonObject obj;
    obj["data"] = data;
    return obj;
  }

  const QList<ParamSet> &params() {
    return params_;
  }

  QList<ParamSet> params_;
  QString machine_id_;
};

#endif //PARAM_SETTINGS_H
