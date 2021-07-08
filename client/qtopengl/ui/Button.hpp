// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_UI_BUTTON_HPP_
#define CLIENT_QTOPENGL_UI_BUTTON_HPP_

#include <QGraphicsItem>
#include <QPointF>

#include "../core/Node.hpp"

namespace UI {
class Button : public Node {
 public:
  static Button* Create(QString pix, Node *parent = nullptr);
  ~Button();
  void SetText(const QString &text);
  void SetOutlineColor(const QColor &color);
  void SetFont(const QFont &font);
  QFont GetFont() const;
  bool UpdateTextPercent(uint8_t percent);
  bool SetPixelSize(uint8_t size);
  void SetPressed(const bool &pressed);
  void SetEnabled(bool enabled);
  void SetCheckable(bool checkable);
  bool IsChecked() const;
  void SetSize(int width, int height);
  void SetPos(int x, int y);

  void MousePressEvent(const QPointF &point, bool &press_validated) override;
  void MouseReleaseEvent(const QPointF &point, bool &prev_validated) override;
  void MouseMoveEvent(const QPointF &point) override;
  void KeyPressEvent(QKeyEvent *event, bool &event_trigger) override;
  void KeyReleaseEvent(QKeyEvent *event, bool &event_trigger) override;

  void Draw(QPainter *painter) override;

 protected:
  void UpdateTextPath();

 private:
  QPixmap *cache_;
  QString background_;
  bool pressed_;
  QPainterPath *text_path_;
  QFont *font_;
  QColor outline_color_;
  uint8_t percent_;
  QString text_;
  bool checkable_;
  bool checked_;
  explicit Button(QString pix, Node *parent = nullptr);
};
}  // namespace UI

#endif  // CLIENT_QTOPENGL_UI_BUTTON_HPP_
