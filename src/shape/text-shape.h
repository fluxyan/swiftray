#include <QFont>
#include <shape/path-shape.h>

#ifndef TEXTSHAPE_H
#define TEXTSHAPE_H

class TextShape : public PathShape {
public:
  TextShape() noexcept;

  TextShape(QString text, QFont font);

  void paint(QPainter *painter) const override;

  Shape::Type type() const override;

  shared_ptr<Shape> clone() const override;

  QString text();

  void setText(QString text);

  const QFont &font() const;

  void setFont(const QFont &font);

  float lineHeight();

  void setLineHeight(float line_height);

  void makeCursorRect(int cursor);

  bool isEditing();

  void setEditing(bool editing);

private:
  bool editing_;
  float line_height_;
  QStringList lines_;
  QFont font_;
  QRectF cursor_rect_;

  void makePath();
};

typedef shared_ptr<TextShape *> TextShapePtr;

#endif // TEXTSHAPE_H
