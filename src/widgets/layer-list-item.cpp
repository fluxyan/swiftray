#include <QDebug>
#include <QStyleOption>
#include <widgets/layer-list-item.h>
#include <canvas/canvas.h>
#include "ui_layer-list-item.h"
#include <undo.h>

/*LayerListItem::LayerListItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayerListItem) {
    ui->setupUi(this);
}*/

LayerListItem::LayerListItem(QWidget *parent, LayerPtr &layer, bool active) :
     QWidget(parent),
     ui(new Ui::LayerListItem),
     layer_(layer),
     active_(active) {
  ui->setupUi(this);
  ui->labelName->setText(layer->name());
  ui->comboBox->setCurrentIndex((int) layer->type());
  createIcon();
  loadStyles();
  registerEvents();
}

LayerListItem::~LayerListItem() {
  delete ui;
}

void LayerListItem::createIcon() {
  QPixmap pix(100, 100);
  pix.fill(QColor::fromRgba64(0, 0, 0, 0));
  QPainter paint(&pix);
  paint.setRenderHint(QPainter::Antialiasing, true);
  QPen pen(QColor(255, 255, 255, 255), 5);
  paint.setPen(pen);
  paint.setBrush(QBrush(layer_->color()));
  paint.drawRoundedRect(QRectF(30, 30, 40, 40), 10, 10);
  paint.end();
  ui->labelIcon->setPixmap(pix);
}

void LayerListItem::loadStyles() {
  if (active_) {
    ui->layerWidgetFrame->setStyleSheet("#layerWidgetFrame { background-color: #0091ff; }");
    ui->labelName->setStyleSheet("color: white;");
  }
}

void LayerListItem::registerEvents() {
  connect(ui->btnHide, &QAbstractButton::clicked, [=]() {
    layer_->setVisible(!layer_->isVisible());
    Canvas::document().addUndoEvent(
         new PropEvent<Layer, bool, &Layer::isVisible, &Layer::setVisible>(layer_.get(), !layer_->isVisible()));
  });
  connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
    auto undo = new PropEvent<Layer, Layer::Type, &Layer::type, &Layer::setType>(layer_.get(), layer_->type());
    layer_->setType((Layer::Type) index);
    Canvas::document().addUndoEvent(undo);
  });
}

void LayerListItem::paintEvent(QPaintEvent *event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
  QWidget::paintEvent(event);
}
