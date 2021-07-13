// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_UI_COMBO_HPP_
#define CLIENT_QTOPENGL_UI_COMBO_HPP_

#include <QGraphicsItem>
#include <QPointF>

#include "../core/Node.hpp"

namespace UI {
class Combo : public Node {
 public:
  static Combo* Create(QString pix, Node *parent = nullptr);
  ~Combo();
  void SetText(const QString &text);
  void SetOutlineColor(const QColor &color);

  void MousePressEvent(const QPointF &point, bool &press_validated) override;
  void MouseReleaseEvent(const QPointF &point, bool &prev_validated) override;
  void MouseMoveEvent(const QPointF &point) override;
  void KeyPressEvent(QKeyEvent *event, bool &event_trigger) override;
  void KeyReleaseEvent(QKeyEvent *event, bool &event_trigger) override;
  void SetCurrentIndex(uint8_t index);
  uint8_t CurrentIndex();
  void AddItem(const QString &item);

  void Draw(QPainter *painter) override;

 protected:
  explicit Combo(QString pix, Node *parent = nullptr);

 private:
  QString background_;
  bool pressed_;
  QPainterPath *text_path_;
  QFont *font_;
  QColor outline_color_;
  uint8_t percent_;
  QString text_;
  bool checkable_;
  bool checked_;
  uint8_t current_index_;
};
}  // namespace UI

#endif  // CLIENT_QTOPENGL_UI_COMBO_HPP_
