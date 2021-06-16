// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_UI_LABEL_HPP_
#define CLIENT_QTOPENGL_UI_LABEL_HPP_

#include <QGraphicsWidget>

#include "../core/Node.hpp"

namespace UI {
class Label : public Node {
 public:
  static Label *Create(QGraphicsItem *parent = nullptr);
  ~Label();

  void SetText(const QString &text);
  void SetFont(const QFont &font);
  QFont GetFont() const;
  void SetColor(const QColor &color);
  QColor GetColor() const;
  void SetPixelSize(uint8_t size);

  void MousePressEventXY(const QPointF &point, bool &press_validated) override;
  void MouseReleaseEventXY(const QPointF &point, bool &prev_validated) override;
  void MouseMoveEventXY(const QPointF &point) override;

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *) override;

 private:
  QPainterPath *text_path_;
  QPixmap *cache_;
  QFont *font_;
  QString text_;
  QColor *color_;

  explicit Label(QGraphicsItem *parent = nullptr);
};
}  // namespace UI
#endif  // CLIENT_QTOPENGL_UI_LABEL_HPP_
