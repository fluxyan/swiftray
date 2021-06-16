#include <QMainWindow>
#include <QQuickWindow>
#include <QQuickWidget>
#include <QListWidget>
#include <QToolButton>
#include <widgets/layer-list-item.h>
#include <widgets/transform-panel.h>
#include <widgets/layer-params-panel.h>
#include <canvas/vcanvas.h>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H


namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

  ~MainWindow();

  bool event(QEvent *e) override;

  void resizeEvent(QResizeEvent *event) override;

  void loadQML();

  void loadQSS();

  void loadWidgets();

private slots:

  void quickWidgetStatusChanged(QQuickWidget::Status);

  void sceneGraphError(QQuickWindow::SceneGraphError error, const QString &message);

  void updateLayers();

  void updateMode();

  void updateSidePanel();

  void openFile();

  void openImageFile();

  void layerOrderChanged(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                         const QModelIndex &destinationParent, int destinationRow);

private:
  Ui::MainWindow *ui;
  VCanvas *canvas_;
  Document *doc_;
  unique_ptr<LayerParamsPanel> layer_params_panel_;
  unique_ptr<TransformPanel> transform_panel_;
  unique_ptr<QToolButton> add_layer_btn_;
};

#endif // MAINWINDOW_H
