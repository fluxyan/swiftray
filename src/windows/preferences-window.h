#pragma once

#include <QDialog>
#include <widgets/base-container.h>

namespace Ui {
  class PreferencesWindow;
}

class PreferencesWindow : public QDialog, BaseContainer {
Q_OBJECT

public:
  explicit PreferencesWindow(QWidget *parent = nullptr);

  ~PreferencesWindow();

  bool isHighSpeedMode();

  void setSpeedMode(bool is_high_speed);

private:
  Ui::PreferencesWindow *ui;

  void setLanguageComboBox();

  void setSpeedOptimizationComboBox();

  void setTabWidget();

  bool is_high_speed_;

Q_SIGNALS:
  void speedModeChanged(bool is_high_speed);

  void fontSizeChanged(int font_size);
};
