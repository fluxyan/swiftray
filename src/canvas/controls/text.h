#include <QMouseEvent>
#include <canvas/controls/canvas-control.h>
#include <shape/text-shape.h>

#ifndef TEXTCONTROL_H
#define TEXTCONTROL_H
namespace Controls {

  class Text : public CanvasControl {
  public:
    Text(Canvas *canvas) noexcept;

    bool mouseReleaseEvent(QMouseEvent *e) override;

    bool hoverEvent(QHoverEvent *e, Qt::CursorShape *cursor) override;

    bool keyPressEvent(QKeyEvent *e) override;

    void paint(QPainter *painter) override;

    bool isActive() override;

    void exit() override;

    TextShape &target();

    void setTarget(ShapePtr &target);

  private:
    ShapePtr target_;
  };

}
#endif // TEXTCONTROL_H
