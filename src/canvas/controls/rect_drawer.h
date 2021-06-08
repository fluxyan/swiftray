#include <QMouseEvent>
#include <canvas/controls/canvas_control.h>

#ifndef RECTDRAWER_H
#define RECTDRAWER_H

class RectDrawer : public CanvasControl {
  public:
    RectDrawer(Scene &scene_) noexcept : CanvasControl(scene_) {}
    bool mouseMoveEvent(QMouseEvent *e) override;
    bool mouseReleaseEvent(QMouseEvent *e) override;
    void paint(QPainter *painter) override;
    void reset();
    bool isActive() override;

  private:
    QRectF rect_;
};

#endif // RECTDRAWER_H
