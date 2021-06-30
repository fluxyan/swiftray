#ifndef LAYERPANEL_H
#define LAYERPANEL_H

#include <QFrame>
#include <QToolButton>
#include <widgets/panels/layer-params-panel.h>

class MainWindow;

namespace Ui {
  class LayerPanel;
}

class LayerPanel : public QFrame {
Q_OBJECT

public:
  explicit LayerPanel(QWidget *parent, MainWindow *main_window_);

  void loadWidgets();

  void registerEvents();

  void resizeEvent(QResizeEvent *) override;

  ~LayerPanel();

private slots:

  void layerOrderChanged(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                         const QModelIndex &destinationParent, int destinationRow);


  void updateLayers();


private:
  Ui::LayerPanel *ui;

  QToolButton *add_layer_btn_;
  LayerParamsPanel *layer_params_panel_;
  MainWindow *main_window_;
};

#endif // LAYERPANEL_H
