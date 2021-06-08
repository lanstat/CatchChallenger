// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_UI_LABEL_HPP_
#define CLIENT_QTOPENGL_UI_LABEL_HPP_

#include <QGraphicsWidget>

namespace UI {
class Label : public QGraphicsItem {
 public:
  explicit Label(QGraphicsItem *parent = nullptr);
  ~Label();
  void SetText(const QString &text);
  void SetFont(const QFont &font);
  QFont GetFont() const;
  void SetColor(const QColor &color);
  QColor GetColor() const;
  void SetPixelSize(uint8_t size);

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *) override;
  QRectF boundingRect() const override;

 private:
  QPainterPath *text_path_;
  QPixmap *cache_;
  QFont *font_;
  QString text_;
  QColor *color_;

  QRectF bounding_rect_;
};
}  // namespace Ui
#endif  // CLIENT_QTOPENGL_UI_LABEL_HPP_
