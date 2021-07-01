// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_UI_LABEL_HPP_
#define CLIENT_QTOPENGL_UI_LABEL_HPP_

#include <QGraphicsWidget>

#include "../core/Node.hpp"

namespace UI {
class Label : public Node {
 public:
  static Label *Create(Node *parent = nullptr);
  static Label *Create(QColor text, QColor border, Node *parent = nullptr);
  static Label *Create(QColor text, QLinearGradient border,
                       Node *parent = nullptr);
  static Label *Create(QLinearGradient text, QLinearGradient border,
                       Node *parent = nullptr);
  ~Label();

  void SetText(const QString &text);
  void SetFont(const QFont &font);
  QFont GetFont() const;
  void SetPixelSize(uint8_t size);
  void UpdateTextPath();

  void MousePressEventXY(const QPointF &point, bool &press_validated) override;
  void MouseReleaseEventXY(const QPointF &point, bool &prev_validated) override;
  void MouseMoveEventXY(const QPointF &point) override;

  void Draw(QPainter *painter) override;

 private:
  QPainterPath *text_path_;
  QPixmap *cache_;
  QFont *font_;
  QString text_;
  QLinearGradient pen_gradient_;
  QLinearGradient brush_gradient_;

  explicit Label(QLinearGradient pen, QLinearGradient brush,
                 Node *parent = nullptr);
};
}  // namespace UI
#endif  // CLIENT_QTOPENGL_UI_LABEL_HPP_
